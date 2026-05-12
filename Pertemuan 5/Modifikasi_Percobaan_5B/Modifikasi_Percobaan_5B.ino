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

  // Timpa slot (setara xQueueOverwrite — tidak peduli apakah sudah dibaca)
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

    // Hitung Heat Index (indeks panas)
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