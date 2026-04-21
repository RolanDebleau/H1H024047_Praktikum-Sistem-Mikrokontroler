Nama: Gerard Roland Kusuma Sarwoko  
NIM: H1H024047  
Shift Awal: B  
Shift Akhir: B  

# Modul 3 – Protokol Komunikasi

## Percobaan 3A: Komunikasi Serial (UART)

### 3.5 Pertanyaan Praktikum

---

**1. Jelaskan proses dari input keyboard hingga LED menyala/mati!**

Prosesnya berjalan sebagai berikut:

1. Pengguna mengetik karakter (`'1'` atau `'0'`) di Serial Monitor pada Arduino IDE, lalu menekan Enter.
2. Karakter tersebut dikirim dari komputer ke Arduino melalui koneksi USB menggunakan protokol UART.
3. Di dalam Arduino, fungsi `Serial.available()` mendeteksi bahwa ada data yang masuk ke buffer serial (nilai > 0).
4. Fungsi `Serial.read()` membaca satu karakter dari buffer tersebut dan menyimpannya ke variabel `data`.
5. Program mengecek nilai `data`:
   - Jika `data == '1'` → `digitalWrite(PIN_LED, HIGH)` → LED menyala, lalu Serial Monitor menampilkan `"LED ON"`.
   - Jika `data == '0'` → `digitalWrite(PIN_LED, LOW)` → LED mati, lalu Serial Monitor menampilkan `"LED OFF"`.
   - Jika karakter lain (bukan `'1'`, `'0'`, `'\n'`, atau `'\r'`) → Serial Monitor menampilkan `"Perintah tidak dikenal"`.

---

**2. Mengapa digunakan `Serial.available()` sebelum membaca data? Apa yang terjadi jika baris tersebut dihilangkan?**

`Serial.available()` digunakan untuk mengecek apakah ada data yang sudah masuk ke buffer serial sebelum mencoba membacanya. Fungsi ini mengembalikan jumlah byte yang tersedia di buffer.

Jika baris `Serial.available()` dihilangkan dan program langsung memanggil `Serial.read()`, maka:
- Ketika tidak ada data dari komputer, `Serial.read()` akan mengembalikan nilai `-1` (karena buffer kosong).
- Nilai `-1` ini akan dievaluasi di dalam kondisi `if`, dan karena tidak cocok dengan `'1'` maupun `'0'`, program akan terus-menerus mencetak `"Perintah tidak dikenal"` ke Serial Monitor secara berulang dan sangat cepat, sehingga Serial Monitor menjadi banjir pesan yang tidak perlu dan membebani sistem.

Kesimpulannya, `Serial.available()` berfungsi sebagai gerbang (guard) agar `Serial.read()` hanya dipanggil saat data memang benar-benar tersedia.

---

**3. Modifikasi program agar LED berkedip (blink) ketika menerima input `'2'`, dan LED terus berkedip sampai perintah selanjutnya diberikan. (Kode + penjelasan setiap baris)**

**Source Code:**

```cpp
#include <Arduino.h>

const int PIN_LED = 8;        // Mendefinisikan pin 8 sebagai pin untuk LED

bool blinkMode = false;       // Flag untuk menandai apakah mode blink sedang aktif
unsigned long previousMillis = 0; // Menyimpan waktu terakhir LED berubah status
const long interval = 500;    // Interval kedip LED: 500 ms (0.5 detik)
bool ledState = false;        // Menyimpan status LED saat ini (menyala/mati)

void setup() {
  Serial.begin(9600);         // Memulai komunikasi serial dengan baud rate 9600 bps
  Serial.println("Ketik '1' untuk menyalakan LED, '0' untuk mematikan, '2' untuk blink");
  pinMode(PIN_LED, OUTPUT);   // Mengatur pin LED sebagai output
}

void loop() {
  // --- Bagian 1: Cek input dari Serial Monitor ---
  if (Serial.available() > 0) {          // Cek apakah ada data yang masuk ke buffer serial
    char data = Serial.read();           // Baca satu karakter dari buffer serial

    if (data == '1') {                   // Jika input adalah '1'
      blinkMode = false;                 // Matikan mode blink
      digitalWrite(PIN_LED, HIGH);       // Nyalakan LED
      Serial.println("LED ON");          // Tampilkan konfirmasi ke Serial Monitor
    }
    else if (data == '0') {              // Jika input adalah '0'
      blinkMode = false;                 // Matikan mode blink
      digitalWrite(PIN_LED, LOW);        // Matikan LED
      Serial.println("LED OFF");         // Tampilkan konfirmasi ke Serial Monitor
    }
    else if (data == '2') {              // Jika input adalah '2'
      blinkMode = true;                  // Aktifkan mode blink
      Serial.println("LED BLINK MODE ON"); // Tampilkan konfirmasi ke Serial Monitor
    }
    else if (data != '\n' && data != '\r') { // Jika bukan '1', '0', '2', Enter, atau CR
      Serial.println("Perintah tidak dikenal"); // Tampilkan pesan error
    }
  }

  // --- Bagian 2: Jalankan blink jika mode blink aktif ---
  if (blinkMode) {                                    // Hanya masuk blok ini jika mode blink aktif
    unsigned long currentMillis = millis();           // Ambil waktu saat ini (ms sejak Arduino menyala)
    if (currentMillis - previousMillis >= interval) { // Cek apakah sudah lewat interval 500ms
      previousMillis = currentMillis;                 // Simpan waktu sekarang sebagai referensi berikutnya
      ledState = !ledState;                           // Toggle status LED (nyala→mati atau mati→nyala)
      digitalWrite(PIN_LED, ledState ? HIGH : LOW);  // Terapkan status LED ke pin fisik
    }
  }
}
```

---

**4. Tentukan apakah menggunakan `delay()` atau `millis()`! Jelaskan pengaruhnya terhadap sistem.**

**Jawaban: Gunakan `millis()`.**

Penjelasan perbandingan:

| Aspek | `delay()` | `millis()` |
|---|---|---|
| Cara kerja | Menghentikan seluruh eksekusi program selama waktu tertentu | Menghitung waktu yang telah berlalu tanpa menghentikan program |
| Pengaruh ke sistem | Selama delay berjalan, Arduino tidak dapat menerima input serial atau menjalankan tugas lain | Program tetap berjalan normal; Serial Monitor tetap bisa menerima input kapan saja |
| Responsivitas | Rendah – perintah baru diabaikan selama LED sedang delay | Tinggi – perintah baru langsung diproses di setiap iterasi `loop()` |
| Rekomendasi | Hanya untuk program sederhana tanpa multitasking | Wajib digunakan jika ada dua tugas berjalan bersamaan (seperti blink + baca serial) |

Pada percobaan ini, menggunakan `delay()` akan menyebabkan Arduino "buta" terhadap input serial saat LED sedang berkedip. Misalnya, jika pengguna mengirim `'0'` saat `delay(500)` berjalan, perintah itu akan terlambat atau bahkan terlewat. Dengan `millis()`, setiap perintah baru akan langsung diproses karena `loop()` tidak pernah berhenti.

---

## Percobaan 3B: Inter-Integrated Circuit (I2C)

### 3.6 Pertanyaan Praktikum

---

**1. Jelaskan bagaimana cara kerja komunikasi I2C antara Arduino dan LCD pada rangkaian tersebut!**

I2C menggunakan dua jalur kabel:
- SDA (Serial Data) → pin A4 pada Arduino Uno: jalur untuk mengirim dan menerima data.
- SCL (Serial Clock) → pin A5 pada Arduino Uno: jalur clock yang dihasilkan oleh Master untuk mensinkronkan pengiriman data.

Cara kerjanya:

1. Arduino bertindak sebagai Master, sedangkan LCD I2C bertindak sebagai Slave.
2. Setiap kali program memanggil fungsi seperti `lcd.print()`, library `LiquidCrystal_I2C` mengemas data tersebut menjadi paket I2C.
3. Arduino mengirimkan START condition di jalur SDA, lalu mengirimkan alamat I2C dari LCD (misalnya `0x27`) sebanyak 7 bit, diikuti bit R/W (0 = write).
4. LCD mengenali alamatnya dan mengirimkan ACK (acknowledge) sebagai tanda siap menerima.
5. Arduino kemudian mengirimkan byte data (perintah atau karakter) satu per satu, masing-masing dikonfirmasi dengan ACK dari LCD.
6. Setelah selesai, Arduino mengirimkan STOP condition untuk mengakhiri komunikasi.
7. Semua proses ini terjadi hanya melalui dua kabel (SDA & SCL), sehingga sangat efisien dalam penggunaan pin.

---

**2. Apakah pin potensiometer harus seperti itu? Jelaskan yang terjadi apabila pin kiri dan pin kanan tertukar!**

Pin potensiometer memiliki fungsi yang spesifik:
- Kaki kiri → GND (0V): sebagai referensi tegangan terendah.
- Kaki tengah → A0: output tegangan yang berubah sesuai posisi putaran (wiper).
- Kaki kanan → 5V: sebagai referensi tegangan tertinggi.

**Jika kaki kiri dan kaki kanan ditukar (kiri ke 5V, kanan ke GND):**

Arah pembacaan nilai ADC akan terbalik. Yang semula membaca nilai kecil (mendekati 0) ketika diputar ke kiri, sekarang akan membaca nilai besar (mendekati 1023), dan sebaliknya. Secara fisik komponen tidak rusak karena potensiometer hanya adalah resistor variabel, tetapi secara logika program, tampilan bar di LCD akan bergerak ke arah yang berlawanan dari arah putaran fisik. Jika diinginkan perilaku yang berbeda ini bisa dikoreksi di software dengan rumus `nilai = 1023 - analogRead(A0)`, tanpa harus mengubah kabel fisik.

---

**3. Modifikasi program menggabungkan UART dan I2C (keduanya sebagai output). (Kode + penjelasan setiap baris)**

**Source Code:**

```cpp
#include <Wire.h>               // Library untuk komunikasi I2C
#include <LiquidCrystal_I2C.h>  // Library untuk mengontrol LCD via I2C
#include <Arduino.h>

// Inisialisasi objek LCD dengan alamat 0x27, 16 kolom, 2 baris
// Ganti 0x27 dengan 0x3F atau 0x20 jika LCD tidak terdeteksi
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinPot = A0;  // Pin analog A0 terhubung ke kaki tengah potensiometer

void setup() {
  Serial.begin(9600);   // Inisialisasi komunikasi serial UART dengan baud rate 9600 bps

  lcd.init();           // Inisialisasi LCD I2C
  lcd.backlight();      // Nyalakan backlight LCD agar tampilan terlihat
}

void loop() {
  int nilai = analogRead(pinPot);  // Baca nilai ADC dari potensiometer (rentang 0–1023)

  // Hitung panjang bar untuk LCD (mapping dari 0–1023 ke 0–16 kolom LCD)
  int panjangBar = map(nilai, 0, 1023, 0, 16);

  // Hitung tegangan aktual berdasarkan nilai ADC (resolusi 10-bit, referensi 5V)
  float volt = (nilai / 1023.0) * 5.0;

  // Hitung persentase putaran potensiometer
  int persen = map(nilai, 0, 1023, 0, 100);

  // --- OUTPUT 1: Serial Monitor (UART) ---
  Serial.print("ADC: ");      // Cetak label ADC
  Serial.print(nilai);        // Cetak nilai ADC mentah (0–1023)
  Serial.print("  Volt: ");   // Cetak label Volt
  Serial.print(volt, 2);      // Cetak nilai tegangan dengan 2 desimal
  Serial.print(" V  Persen: ");// Cetak label Persen
  Serial.print(persen);       // Cetak nilai persentase
  Serial.println("%");        // Cetak simbol % dan pindah baris baru

  // --- OUTPUT 2: LCD I2C ---

  // Baris pertama LCD: tampilkan nilai ADC dan persentase
  lcd.setCursor(0, 0);        // Set kursor ke kolom 0, baris 0 (baris pertama)
  lcd.print("ADC:");          // Cetak label "ADC:"
  lcd.print(nilai);           // Cetak nilai ADC
  lcd.print(" ");             // Spasi pemisah
  lcd.print(persen);          // Cetak persentase
  lcd.print("%  ");           // Cetak simbol % + spasi untuk menghapus sisa karakter lama

  // Baris kedua LCD: tampilkan bar visual sesuai panjangBar
  lcd.setCursor(0, 1);        // Set kursor ke kolom 0, baris 1 (baris kedua)
  for (int i = 0; i < 16; i++) {       // Loop sebanyak 16 kolom (lebar LCD)
    if (i < panjangBar) {              // Jika kolom ke-i masuk dalam range bar
      lcd.print((char)255);            // Cetak karakter blok penuh (█) sebagai bar
    } else {                           // Jika di luar range bar
      lcd.print(" ");                  // Cetak spasi untuk menghapus karakter sebelumnya
    }
  }

  delay(200);  // Tunggu 200ms sebelum pembacaan berikutnya agar tampilan stabil
}
```

---

**4. Lengkapi tabel berdasarkan pengamatan pada Serial Monitor**

| ADC | Volt (V) | Persen (%) |
|-----|----------|------------|
| 1   | 0.00 V   | 0%         |
| 21  | 0.10 V   | 2%         |
| 49  | 0.24 V   | 5%         |
| 74  | 0.36 V   | 7%         |
| 96  | 0.47 V   | 9%         |

---

## Pertanyaan Umum Modul 3

---

**1. Sebutkan dan jelaskan keuntungan dan kerugian menggunakan UART dan I2C!**

**UART (Universal Asynchronous Receiver-Transmitter):**

| | Penjelasan |
|---|---|
|  **Keuntungan** | Sangat sederhana, hanya butuh 2 kabel (TX dan RX). Tidak memerlukan sinyal clock eksternal (asinkron). Didukung oleh hampir semua mikrokontroler. Mudah di-debug via Serial Monitor. |
|  **Kerugian** | Hanya bisa menghubungkan 2 perangkat secara langsung (point-to-point). Kedua perangkat harus menggunakan baud rate yang sama persis. Tidak ada mekanisme alamat, sehingga tidak bisa dipakai untuk bus bersama. |

**I2C (Inter-Integrated Circuit):**

| | Penjelasan |
|---|---|
|  **Keuntungan** | Hanya butuh 2 kabel (SDA dan SCL) untuk menghubungkan banyak perangkat sekaligus dalam satu bus. Setiap perangkat memiliki alamat unik sehingga Master bisa memilih Slave yang dituju. Hemat pin mikrokontroler. |
|  **Kerugian** | Kecepatan transfer lebih lambat dibanding SPI. Jika dua perangkat memiliki alamat yang sama, akan terjadi konflik dan komunikasi gagal. Memerlukan resistor pull-up pada jalur SDA dan SCL. Makin banyak perangkat, makin kompleks pengelolaan alamat. |

---

**2. Bagaimana peran alamat I2C pada LCD (misalnya 0x27 vs 0x20)? Berikan penjelasan!**

Alamat I2C adalah identitas unik yang dimiliki setiap perangkat Slave dalam bus I2C, setara dengan "nomor kursi" di bus sekolah. Karena semua perangkat berbagi dua kabel yang sama (SDA dan SCL), alamat ini menjadi satu-satunya cara bagi Arduino (Master) untuk menentukan perangkat mana yang harus menerima data.

- 0x27 adalah alamat default untuk modul LCD I2C dengan chip PCF8574T buatan NXP. Ini adalah varian paling umum di pasaran.
- 0x20 adalah alamat default untuk chip PCF8574 (tanpa T) atau varian lain dengan konfigurasi pin A0–A2 yang berbeda.
- 0x3F adalah alamat default untuk chip PCF8574AT buatan Texas Instruments (TI).

Jika dalam program ditulis `LiquidCrystal_I2C lcd(0x27, 16, 2)` namun LCD fisik memiliki alamat 0x3F, LCD tidak akan merespons sama sekali dan layar tetap gelap. Untuk mengetahui alamat yang benar, dapat digunakan sketch I2C Scanner yang akan memindai semua alamat dari 0x00 hingga 0x7F dan melaporkan alamat perangkat yang terdeteksi.

---

**3. Jika UART dan I2C digabung dalam satu sistem (input dari Serial Monitor, output ke LCD), bagaimana alur kerja sistem tersebut? Bagaimana Arduino mengelola dua protokol sekaligus?**

**Alur kerja sistem gabungan UART + I2C:**

```
[Komputer] ──UART/USB──▶ [Arduino] ──I2C (SDA/SCL)──▶ [LCD]
               Serial Monitor          LiquidCrystal_I2C
```

1. Pengguna mengetik perintah di Serial Monitor → data dikirim ke Arduino via UART.
2. Di dalam `loop()`, Arduino mengecek `Serial.available()`. Jika ada data, `Serial.read()` membaca karakter tersebut.
3. Arduino memproses karakter yang diterima (misalnya mengubah mode atau nilai).
4. Arduino kemudian mengirimkan hasil pemrosesan ke LCD menggunakan **I2C** melalui fungsi `lcd.print()`.
5. Library `LiquidCrystal_I2C` mengurus semua detail protokol I2C (START, alamat, data, ACK, STOP) secara otomatis di background.

**Bagaimana Arduino mengelola dua protokol sekaligus:**

Arduino tidak menjalankan keduanya secara benar-benar bersamaan (tidak ada multithreading), melainkan secara sekuensial (bergantian) dalam satu siklus `loop()`. Namun karena kecepatan prosessor Arduino (16 MHz) jauh lebih cepat dari kecepatan manusia mengetik maupun kecepatan I2C (biasanya 100 kHz), dalam praktiknya keduanya terasa berjalan bersamaan. Kuncinya adalah: UART menggunakan buffer hardware sehingga data dari komputer tidak hilang meskipun Arduino sedang sibuk berkomunikasi I2C, dan data tersebut akan dibaca pada iterasi `loop()` berikutnya.