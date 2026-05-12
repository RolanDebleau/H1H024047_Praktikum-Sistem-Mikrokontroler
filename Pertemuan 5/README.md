Nama: Gerard Roland Kusuma Sarwoko  
NIM: H1H024047  
Shift Awal: B  
Shift Akhir: B  

# Modul 5 – Real-Time Operating System (RTOS)

### 5.5 Pertanyaan Praktikum

**1. Apakah ketiga task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!**

Task-task berjalan **secara bergantian** (bukan benar-benar bersamaan), namun karena pergantiannya sangat cepat maka tampak *seolah* bersamaan (*concurrent*). FreeRTOS menggunakan mekanisme **preemptive scheduling** berbasis time-slice: setiap task mendapat jatah waktu CPU. Ketika satu task memanggil `vTaskDelay()`, task tersebut masuk ke state *Blocked* dan scheduler segera mengeksekusi task lain yang siap (*Ready*). Dengan demikian, CPU tidak pernah menganggur selama ada task yang ready.

**2. Bagaimana cara menambahkan task keempat? Jelaskan langkahnya**

Langkah-langkahnya:
1. Deklarasikan prototipe fungsi task baru, misalnya `void TaskBaru(void *pvParameters);`
2. Tambahkan pemanggilan `xTaskCreate()` di dalam `setup()`:
   ```cpp
   xTaskCreate(TaskBaru, "taskBaru", 128, NULL, 1, NULL);
   ```
3. Implementasikan fungsi task di luar `loop()`:
   ```cpp
   void TaskBaru(void *pvParameters) {
     while (1) {
       // kode task
       vTaskDelay(400 / portTICK_PERIOD_MS);
     }
   }
   ```
4. Pastikan total stack yang dialokasikan tidak melebihi RAM Arduino Uno (2KB).

**3. Modifikasilah program dengan menambah sensor (misalnya potensiometer), lalu gunakan nilainya untuk mengontrol kecepatan LED! Bagaimana hasilnya? Jelaskan program pada file README.md.**

```cpp
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define POT_PIN  A0
#define LED1_PIN  8
#define LED2_PIN  7

SemaphoreHandle_t xMutex;
volatile int blinkSpeed = 200; // nilai default delay (ms)

void TaskReadPot(void *pvParameters);
void TaskBlink1(void *pvParameters);
void TaskBlink2(void *pvParameters);
void Taskprint(void *pvParameters);

void setup() {
  Serial.begin(9600);
  pinMode(POT_PIN, INPUT);

  xMutex = xSemaphoreCreateMutex();

  xTaskCreate(TaskReadPot, "ReadPot", 128, NULL, 2, NULL); // prioritas lebih tinggi
  xTaskCreate(TaskBlink1,  "task1",   128, NULL, 1, NULL);
  xTaskCreate(TaskBlink2,  "task2",   128, NULL, 1, NULL);
  xTaskCreate(Taskprint,   "task3",   128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {}

// Task membaca potensiometer dan update blinkSpeed
void TaskReadPot(void *pvParameters) {
  while (1) {
    int potValue = analogRead(POT_PIN);          // 0 - 1023
    int newSpeed = map(potValue, 0, 1023, 50, 1000); // mapping ke 50ms - 1000ms

    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      blinkSpeed = newSpeed;
      xSemaphoreGive(xMutex);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Task LED 1 - kecepatan dipengaruhi potensiometer
void TaskBlink1(void *pvParameters) {
  pinMode(LED1_PIN, OUTPUT);
  while (1) {
    int speed;
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      speed = blinkSpeed;
      xSemaphoreGive(xMutex);
    }

    Serial.println("Task1 - LED1 Blink");
    digitalWrite(LED1_PIN, HIGH);
    vTaskDelay(speed / portTICK_PERIOD_MS);
    digitalWrite(LED1_PIN, LOW);
    vTaskDelay(speed / portTICK_PERIOD_MS);
  }
}

// Task LED 2 - kecepatan 1.5x lebih lambat dari LED1
void TaskBlink2(void *pvParameters) {
  pinMode(LED2_PIN, OUTPUT);
  while (1) {
    int speed;
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      speed = blinkSpeed;
      xSemaphoreGive(xMutex);
    }

    Serial.println("Task2 - LED2 Blink");
    digitalWrite(LED2_PIN, HIGH);
    vTaskDelay((speed * 1.5) / portTICK_PERIOD_MS);
    digitalWrite(LED2_PIN, LOW);
    vTaskDelay((speed * 1.5) / portTICK_PERIOD_MS);
  }
}

// Task print counter + tampilkan blinkSpeed saat ini
void Taskprint(void *pvParameters) {
  int counter = 0;
  while (1) {
    int speed;
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      speed = blinkSpeed;
      xSemaphoreGive(xMutex);
    }

    counter++;
    Serial.print("Counter: ");
    Serial.print(counter);
    Serial.print(" | Blink Speed: ");
    Serial.print(speed);
    Serial.println(" ms");

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
```

Program telah dimodifikasi dengan menambahkan `TaskReadPot` yang membaca nilai ADC dari potensiometer dan memetakannya ke rentang delay 50–1000ms. Hasilnya: kecepatan kedip LED berubah secara dinamis sesuai posisi potensiometer. Mutex digunakan untuk melindungi akses ke variabel `blinkSpeed` agar tidak terjadi *race condition* antar-task.

---

### 5.6 Pertanyaan Praktikum

**1. Apakah kedua task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!**

Kedua task (fungsi `task_read_data` dan `task_display`) berjalan **secara bergantian** dalam model **cooperative scheduling** melalui `loop()`. Berbeda dengan FreeRTOS yang preemptive, di sini tidak ada scheduler; kedua fungsi saling melepaskan kontrol secara manual. `task_read_data` melepaskan kontrol ke `task_display` setelah selesai mengisi `queueSlot`, dan `task_display` hanya bekerja jika flag `newData` aktif.

**2. Apakah program ini berpotensi mengalami race condition? Jelaskan!**

Pada versi FreeRTOS (dengan dua task terpisah), **ya, berpotensi race condition** apabila tidak menggunakan mekanisme sinkronisasi. Jika `read_data` menulis ke `queueSlot` bersamaan dengan `display` membacanya, data yang dibaca bisa korup. Solusi: gunakan `xQueueSend` / `xQueueReceive` (atomic operation di FreeRTOS) atau Mutex.

Pada versi modifikasi ini (single-loop cooperative), **race condition tidak terjadi** karena kedua fungsi tidak berjalan secara paralel — mereka dieksekusi satu per satu secara sekuensial dalam satu thread.

**3. Modifikasilah program dengan menggunakan sensor DHT sesungguhnya sehingga informasi yang ditampilkan dinamis. Bagaimana hasilnya? Jelaskan program pada file README.md.**

```cpp
#include <DHT.h>

#define DHTPIN   2        // Sesuai koneksi di diagram.json
#define DHTTYPE  DHT22    // Sesuai atribut "type":"DHT22" di diagram.json

DHT dht(DHTPIN, DHTTYPE);

struct readings {
  float temp;   // Suhu dalam derajat Celsius
  float h;      // Kelembaban relatif dalam persen
  bool  valid;  // Flag: true jika pembacaan berhasil
};

readings queueSlot;        // Slot penyimpanan data (setara queue size=1)
bool     newData = false;  // Flag pengganti xQueueOverwrite/Receive

unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 2000;  // ms — sama dengan vTaskDelay(2000)

void setup() {
  Serial.begin(9600);   // Baud rate sesuai serialMonitor di diagram.json
  dht.begin();

  Serial.println(F("=== DHT Monitor dimulai ==="));
  Serial.println(F("Menunggu pembacaan pertama..."));
  Serial.println(F("--------------------"));
}

void task_read_data() {
  unsigned long now = millis();
  if (now - lastReadTime < READ_INTERVAL) return;  // Belum waktunya
  lastReadTime = now;

  readings data;
  data.h    = dht.readHumidity();
  data.temp = dht.readTemperature();  // Celsius
  data.valid = !(isnan(data.h) || isnan(data.temp));

  queueSlot = data;
  newData   = true;
}

void task_display() {
  if (!newData) return;   // Tidak ada data baru, lewati
  newData = false;        // Konsumsi data (setara setelah Receive berhasil)

  readings data = queueSlot;

  if (data.valid) {
    Serial.print(F("Suhu      : "));
    Serial.print(data.temp, 1);
    Serial.println(F(" °C"));

    Serial.print(F("Kelembaban: "));
    Serial.print(data.h, 1);
    Serial.println(F(" %"));

    float hi = dht.computeHeatIndex(data.temp, data.h, false);
    Serial.print(F("Heat Index: "));
    Serial.print(hi, 1);
    Serial.println(F(" °C"));

    Serial.println(F("--------------------"));
  } else {
    Serial.println(F("ERROR: Gagal membaca sensor DHT!"));
    Serial.println(F("Periksa kabel dan koneksi sensor."));
    Serial.println(F("--------------------"));
  }
}

void loop() {
  task_read_data();   // "Task 1" — cek & baca sensor
  task_display();     // "Task 2" — tampilkan jika ada data baru
}
```

Program dimodifikasi dengan mengganti nilai statis (`temp=54, h=30`) menggunakan pembacaan nyata dari sensor DHT22 via library `DHT.h`. Hasilnya: data suhu dan kelembaban yang ditampilkan bersifat dinamis, berubah sesuai kondisi lingkungan. Ditambahkan pula perhitungan **Heat Index** dan penanganan error saat sensor gagal dibaca.

---

## 5.7 Pertanyaan Analisis

**1. Jelaskan perbedaan antara loop() pada Arduino biasa dengan sistem yang menggunakan RTOS!**

| Aspek            | Arduino biasa (`loop()`)                     | Arduino + RTOS                              |
|------------------|----------------------------------------------|---------------------------------------------|
| Eksekusi         | Sekuensial, satu per satu                    | Concurrent, banyak task "bersamaan"         |
| Blocking         | `delay()` memblokir seluruh program          | `vTaskDelay()` hanya memblokir task tertentu |
| Penjadwalan      | Manual oleh programmer (urutan di loop)     | Otomatis oleh kernel scheduler RTOS         |
| Responsivitas    | Menurun jika ada task panjang                | Tetap terjaga berkat preemptive scheduler   |
| Prioritas task   | Tidak ada                                    | Ada, dapat dikonfigurasi                    |

**2. Mengapa fungsi loop() dibiarkan kosong?**

Karena setelah `vTaskStartScheduler()` dipanggil di `setup()`, **kontrol CPU sepenuhnya diserahkan ke kernel FreeRTOS**. Scheduler RTOS mengambil alih eksekusi dan menjalankan task-task yang telah dibuat. Fungsi `loop()` pada dasarnya tidak pernah dieksekusi, atau jika pun dieksekusi (pada beberapa implementasi), ia akan segera di-*preempt* oleh task dengan prioritas lebih tinggi. Semua logika program sudah berada di dalam fungsi-fungsi task.

**3. Apa insight utama yang Anda pelajari dari praktikum ini?**

- **RTOS memungkinkan pemisahan tanggung jawab** (separation of concerns): setiap task menangani satu fungsi spesifik, membuat kode lebih modular dan mudah dikembangkan.
- **Preemptive scheduling** memberikan responsivitas yang jauh lebih baik dibanding polling sekuensial di `loop()`.
- **Sinkronisasi antar-task sangat kritis**: tanpa Mutex atau Queue, akses bersamaan ke variabel shared dapat menyebabkan *race condition* dan perilaku yang tidak terduga.
- **Trade-off memori**: setiap task di FreeRTOS membutuhkan stack tersendiri — pada Arduino Uno yang hanya memiliki 2KB RAM, jumlah dan ukuran stack task harus diperhitungkan dengan cermat.

---
