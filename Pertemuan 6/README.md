Nama: Gerard Roland Kusuma Sarwoko  
NIM: H1H024047  
Shift Awal: B  
Shift Akhir: B 

# Modul 6 – Interrupt dan Timer pada Arduino Uno

## 6.5 Pertanyaan Praktikum

### Pertanyaan Percobaan 6A

**1. Jelaskan proses bagaimana tombol dapat mengubah kondisi LED menggunakan interrupt!**

Saat tombol ditekan, tegangan pada pin 2 turun dari HIGH (5V) ke LOW (0V) karena konfigurasi `INPUT_PULLUP`. Transisi ini (FALLING edge) terdeteksi oleh hardware interrupt controller pada mikrokontroler ATmega328P. Hardware interrupt controller mengirim sinyal ke CPU untuk menghentikan sementara program utama yang sedang berjalan di `loop()`. CPU menyimpan konteks (program counter, register) ke stack, lalu melompat ke alamat ISR (`tombolInterrupt()`). Di dalam ISR, nilai `ledState` di-toggle. Setelah ISR selesai, CPU memulihkan konteks dari stack dan melanjutkan eksekusi `loop()` dari titik terakhir berhenti. Loop kemudian membaca `ledState` yang baru dan menuliskannya ke LED.

**2. Apa fungsi `attachInterrupt()` pada program tersebut?**

`attachInterrupt()` adalah fungsi untuk mendaftarkan sebuah ISR ke sistem interrupt hardware Arduino. Fungsi ini menerima tiga argumen: (1) nomor interrupt yang dikaitkan dengan pin fisik melalui `digitalPinToInterrupt()`, (2) nama fungsi ISR yang akan dipanggil, dan (3) mode pemicu (FALLING, RISING, CHANGE, atau LOW). Tanpa `attachInterrupt()`, mikrokontroler tidak akan tahu fungsi mana yang harus dipanggil saat event interrupt terjadi.

**3. Mengapa pada ISR tidak disarankan menggunakan `delay()` dan `Serial.print()`?**

ISR harus selesai secepat mungkin karena selama ISR berjalan, program utama berhenti total. `delay()` menggunakan Timer0 dan mekanisme penghitungan internal yang dinonaktifkan selama ISR berjalan — memanggilnya di dalam ISR menyebabkan program hang (macet selamanya). `Serial.print()` bersifat blocking dan bergantung pada buffer serta interrupt UART yang juga dinonaktifkan selama ISR berjalan, sehingga dapat menyebabkan deadlock atau data rusak. Prinsipnya: ISR hanya boleh mengubah flag atau variabel, kemudian biarkan `loop()` yang menangani logika panjang.

**4. Apa fungsi keyword `volatile` pada variabel `ledState`?**

Keyword `volatile` memberitahu compiler C++ bahwa nilai variabel tersebut dapat berubah kapan saja di luar alur eksekusi program yang terlihat — dalam hal ini, dari dalam ISR. Tanpa `volatile`, compiler dapat "mengoptimasi" akses ke variabel dengan menyimpan salinannya di register CPU dan tidak selalu membacanya ulang dari RAM. Akibatnya, perubahan yang dilakukan ISR terhadap `ledState` mungkin tidak terlihat oleh `loop()`, sehingga LED tidak merespons interrupt. Dengan `volatile`, setiap akses ke variabel selalu membaca nilai aktual dari memori.

**5. Pada percobaan digunakan mode interrupt FALLING. Modifikasikan program menggunakan mode interrupt lain (RISING, CHANGE, atau LOW) kemudian:** 
- Jelaskan perbedaan cara kerja masing-masing mode interrupt tersebut
- Analisis perubahan perilaku LED yang terjadi pada setiap mode
- Sertakan source code dan penjelasan program dalam bentuk README.md 

Mode FALLING memicu interrupt saat sinyal berubah dari HIGH ke LOW, yaitu tepat saat tombol ditekan (karena INPUT_PULLUP membuat kondisi normal adalah HIGH). Mode RISING adalah kebalikannya, interrupt dipicu saat sinyal berubah dari LOW ke HIGH, artinya LED baru akan toggle ketika tombol dilepaskan, bukan saat ditekan. Hal ini menyebabkan sedikit keterlambatan respons yang terasa tidak natural bagi pengguna. Mode CHANGE memicu interrupt pada kedua transisi, baik dari HIGH ke LOW maupun dari LOW ke HIGH, sehingga LED akan toggle dua kali dalam satu siklus tekan-lepas tombol, yang membuat LED tampak seperti tidak berubah kondisinya secara konsisten. Mode LOW memicu interrupt secara terus-menerus selama sinyal pada pin bernilai LOW (selama tombol ditekan), sehingga ISR akan dipanggil berkali-kali secara cepat dan LED akan berkedip sangat cepat atau berperilaku tidak terduga selama tombol ditahan.
Berikut source code modifikasi untuk masing-masing mode beserta README.md:

``` cpp
// mode_rising.ino
// Interrupt dipicu saat tombol DILEPAS (sinyal LOW -> HIGH)
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle LED saat tombol dilepas
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, RISING);
}

void loop() {
  digitalWrite(13, ledState);
}
```
```cpp
// mode_change.ino
// Interrupt dipicu saat sinyal BERUBAH (tekan DAN lepas)
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle setiap ada perubahan sinyal
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, CHANGE);
}

void loop() {
  digitalWrite(13, ledState);
}
```
```cpp
// mode_low.ino
// Interrupt dipicu TERUS-MENERUS selama sinyal LOW (tombol ditekan)
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle terus selama tombol ditahan
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, LOW);
}

void loop() {
  digitalWrite(13, ledState);
}
```

---
# 6.6 Pertanyaan Praktikum

### Pertanyaan Percobaan 6B

**1. Jelaskan bagaimana fungsi `millis()` bekerja pada program tersebut!**

`millis()` mengembalikan jumlah milidetik yang telah berlalu sejak Arduino dinyalakan, yang dihasilkan oleh Timer0 interrupt secara internal. Pada setiap iterasi `loop()`, nilai `millis()` disimpan ke `currentMillis`. Selisih `currentMillis - previousMillis` merepresentasikan waktu yang telah berlalu sejak terakhir kali LED diubah. Jika selisih tersebut ≥ interval (1000 ms), LED di-toggle dan `previousMillis` diperbarui. Dengan demikian, LED berpindah state setiap 1 detik.

**2. Apa perbedaan utama antara `delay()` dan `millis()`?**

| Aspek | `delay()` | `millis()` |
|---|---|---|
| Sifat | Blocking | Non-blocking |
| CPU selama menunggu | Berhenti total | Tetap berjalan |
| Multitasking | Tidak mendukung | Mendukung |
| Akurasi | Cukup baik | Sangat baik (bergantung pada Timer0) |
| Penggunaan dalam ISR | Tidak boleh | Boleh (hanya baca) |

**3. Mengapa metode `millis()` disebut non-blocking?**

Disebut non-blocking karena program tidak berhenti menunggu waktu berlalu. CPU terus menjalankan iterasi `loop()` — hanya melakukan pengecekan kondisi (`if`) yang selesai dalam beberapa instruksi mesin. Jika interval belum tercapai, program langsung lanjut ke instruksi berikutnya. Akibatnya, task lain (misalnya membaca sensor, memproses komunikasi serial, atau mengontrol LED lain) tetap dapat berjalan secara bersamaan dalam satu `loop()`.

**4. Modifikasi program agar:**
- LED pertama berkedip setiap 1 detik
- LED kedua berkedip setiap 500 ms
- Tanpa menggunakan delay() 
## Berikan penjelasan setiap baris program dalam bentuk README.md.
```cpp
#include <Arduino.h>

// ============================================================
// PIN ASSIGNMENT
// ============================================================
const int PIN_LED1 = 13;   // LED pertama
const int PIN_LED2 = 12;   // LED kedua

// ============================================================
// TIMER LED 1 – interval 1000 ms (1 detik)
// ============================================================
unsigned long prevMillis1 = 0;
const long interval1      = 1000;
bool ledState1            = false;

// ============================================================
// TIMER LED 2 – interval 500 ms (0.5 detik)
// ============================================================
unsigned long prevMillis2 = 0;
const long interval2      = 500;
bool ledState2            = false;

void setup() {
  pinMode(PIN_LED1, OUTPUT);  // Pin 13 sebagai output untuk LED 1
  pinMode(PIN_LED2, OUTPUT);  // Pin 12 sebagai output untuk LED 2
}

void loop() {
  // Ambil waktu saat ini sekali, dipakai bersama oleh kedua timer
  // agar tidak terjadi perbedaan pembacaan antar timer
  unsigned long now = millis();

  // ----------------------------------------------------------
  // TIMER LED 1 – cek setiap iterasi loop, tanpa memblokir
  // ----------------------------------------------------------
  if (now - prevMillis1 >= interval1) {
    prevMillis1 = now;           // catat waktu terakhir toggle
    ledState1   = !ledState1;    // balik kondisi LED 1
    digitalWrite(PIN_LED1, ledState1);
  }

  // ----------------------------------------------------------
  // TIMER LED 2 – berjalan independen dari LED 1
  // Kedua timer bekerja serentak karena tidak ada delay()
  // ----------------------------------------------------------
  if (now - prevMillis2 >= interval2) {
    prevMillis2 = now;           // catat waktu terakhir toggle
    ledState2   = !ledState2;    // balik kondisi LED 2
    digitalWrite(PIN_LED2, ledState2);
  }

  // Di sini masih bisa ditambah task lain (baca sensor, dsb.)
  // karena loop() tidak pernah terblokir
}
```

---
### 6.7 Pertanyaan Analisis

**1. Sebutkan dan jelaskan keuntungan menggunakan interrupt dibanding polling!**

| Aspek | Polling | Interrupt |
|---|---|---|
| Penggunaan CPU | Tinggi (sibuk terus mengecek) | Rendah (CPU bebas saat tidak ada event) |
| Latensi respons | Tergantung frekuensi polling, bisa lambat | Sangat rendah, langsung merespons |
| Konsumsi daya | Lebih tinggi (CPU selalu aktif) | Lebih rendah (bisa sleep) |
| Kerumitan kode | Sederhana | Perlu hati-hati (ISR, volatile) |
| Keandalan event | Bisa terlewat jika polling lambat | Tidak ada event yang terlewat |

Interrupt sangat unggul pada aplikasi di mana event bersifat asinkron dan langka (misalnya penekanan tombol), karena CPU dapat mengerjakan hal lain dan hanya "terganggu" saat event benar-benar terjadi.

**2. Mengapa timer penting dalam sistem embedded dan real-time?**

Dalam sistem real-time, waktu adalah faktor kritis. Timer menyediakan mekanisme untuk: (1) mengeksekusi task secara periodik dengan presisi tinggi tanpa memblokir program utama, (2) mengukur durasi event eksternal seperti lebar pulsa, (3) menghasilkan sinyal PWM untuk kontrol motor dan kecerahan LED, (4) mengimplementasikan *watchdog timer* untuk memulihkan sistem dari kondisi hang, dan (5) menjaga sinkronisasi antar task dalam sistem multitasking.

**3. Jika interrupt dan timer digabung dalam satu sistem, bagaimana alur kerja sistem tersebut?**

```
[Program Utama / loop()]
        |
        | (berjalan normal)
        |
   ┌────┴──────────────────────────────────────┐
   │  Timer Interrupt (periodik, misal 100 ms)  │
   │  → ISR Timer: perbarui counter, baca sensor│
   └────┬──────────────────────────────────────┘
        |
   ┌────┴──────────────────────────────────────┐
   │  External Interrupt (event: tombol tekan) │
   │  → ISR Tombol: set flag / toggle variabel │
   └────┬──────────────────────────────────────┘
        |
        | (loop() membaca flag dari ISR, mengambil tindakan)
```

Alurnya: program utama berjalan normal. Jika timer mencapai batas, Timer ISR dijalankan (misalnya: membaca sensor setiap 100 ms). Jika tombol ditekan, External Interrupt ISR dijalankan. Kedua jenis ISR menggunakan variabel `volatile` untuk berkomunikasi dengan `loop()`. Program utama di `loop()` membaca flag/variabel tersebut dan mengambil tindakan yang lebih panjang (yang tidak boleh dilakukan di dalam ISR).

**4. Apa yang terjadi jika ISR terlalu panjang atau kompleks?**

Dampak ISR yang terlalu panjang:
- **Interrupt lain terlewat:** Selama ISR berjalan, interrupt lain (termasuk timer interrupt untuk `millis()`) dinonaktifkan, sehingga event yang terjadi di waktu itu tidak akan ditangani.
- **`millis()` menjadi tidak akurat:** Jika Timer0 interrupt terlewat, penghitung `millis()` tidak bertambah, menyebabkan pengukuran waktu meleset.
- **Sistem menjadi tidak responsif:** Program utama berhenti dalam waktu lama, menyebabkan keterlambatan yang terasa pada output (LED, display, aktuator).
- **Stack overflow:** ISR yang dalam (misalnya memanggil banyak fungsi) berisiko menghabiskan stack memori yang terbatas pada AVR.
- **Deadlock:** Memanggil fungsi yang bergantung pada interrupt lain (seperti `Serial`) di dalam ISR dapat menyebabkan sistem hang.

---
