# Modul 2 – Pemrograman Input dan Output
## Pertanyaan Praktikum Percobaan 2A : Seven Segment

1. Gambarkan rangkaian schematic yang digunakan pada percobaan!

https://www.tinkercad.com/things/cBZXKb9pJPV-seven-segment?sharecode=2c6dAi6zMdced0uSW-g0ZB_3NeuCIBqWOgoF4zlNDwI

3. Apa yang terjadi jika nilai num lebih dari 15?

Jika num lebih dari 15, maka terjadi akses array di luar batas (out-of-bounds array access). Array digitPattern hanya memiliki indeks 0–15 (16 elemen). Ketika num = 16 atau lebih, program Arduino akan membaca data dari lokasi memori yang tidak terdefinisi, sehingga output pada seven segment akan menampilkan pola acak/tidak karuan. Pada Arduino hal ini tidak menyebabkan crash (karena tidak ada proteksi memori), namun hasilnya tidak dapat diprediksi dan bisa merusak logika program.

3. Apakah program ini menggunakan common cathode atau common anode? Jelaskan alasannya!

Program ini menggunakan common cathode. Alasannya: pada digitPattern, nilai 1 berarti segmen menyala dan 0 berarti mati. Fungsi digitalWrite(segmentPins[i], digitPattern[num][i]) akan mengirimkan logika HIGH (5V) ke pin segmen yang ingin dinyalakan. Pada seven segment common cathode, kaki common terhubung ke GND, sehingga segmen akan menyala ketika pin-nya diberi logika HIGH. Sebaliknya, jika menggunakan common anode (common terhubung ke VCC), segmen menyala ketika pin diberi logika LOW — sehingga pola bit-nya harus dibalik (0 = nyala, 1 = mati).

4. Modifikasi program agar tampilan berjalan dari F ke 0

```cpp
#include <Arduino.h>

// Pin mapping segmen: a b c d e f g dp
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

// Pola segmen untuk 0–F (urutan: a b c d e f g dp)
// 1 = segmen menyala, 0 = segmen mati
byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, //0
  {0,1,1,0,0,0,0,0}, //1
  {1,1,0,1,1,0,1,0}, //2
  {1,1,1,1,0,0,1,0}, //3
  {0,1,1,0,0,1,1,0}, //4
  {1,0,1,1,0,1,1,0}, //5
  {1,0,1,1,1,1,1,0}, //6
  {1,1,1,0,0,0,0,0}, //7
  {1,1,1,1,1,1,1,0}, //8
  {1,1,1,1,0,1,1,0}, //9
  {1,1,1,0,1,1,1,0}, //A
  {0,0,1,1,1,1,1,0}, //b
  {1,0,0,1,1,1,0,0}, //C
  {0,1,1,1,1,0,1,0}, //d
  {1,0,0,1,1,1,1,0}, //E
  {1,0,0,0,1,1,1,0}  //F
};

// Fungsi menampilkan digit ke seven segment
void displayDigit(int num) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], digitPattern[num][i]);
  }
}

void setup() {
  // Set semua pin segmen sebagai OUTPUT
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
}

void loop() {
  // Berjalan mundur dari F (15) ke 0
  for (int i = 15; i >= 0; i--) {
    displayDigit(i);
    delay(1000); // Tunda 1 detik tiap angka
  }
}
```

Perubahan utama hanya pada fungsi loop(): loop diubah dari i=0; i<16; i++ menjadi i=15; i>=0; i-- agar urutan berjalan mundur dari F hingga 0.

```cpp
Deskripsi
Program ini menampilkan karakter heksadesimal dari F hingga 0 secara berurutan
pada seven segment display yang terhubung ke Arduino Uno.

Konfigurasi Pin
| Segmen | Pin Arduino |
|--------|------------|
| a      | 7          |
| b      | 6          |
| c      | 5          |
| d      | 11         |
| e      | 10         |
| f      | 8          |
| g      | 9          |
| dp     | 4          |

Penjelasan Kode

`segmentPins[8]`
Array yang menyimpan nomor pin Arduino untuk tiap segmen (a,b,c,d,e,f,g,dp).

`digitPattern[16][8]`
Array 2D berisi pola nyala/mati (1/0) untuk setiap segmen pada tiap digit 0–F.
Urutan kolom: a, b, c, d, e, f, g, dp.

`displayDigit(int num)`
Fungsi yang menerima angka 0–15, lalu menulis nilai HIGH/LOW ke masing-masing
pin segmen sesuai pola pada `digitPattern[num]`.

`setup()`
Mengatur semua 8 pin segmen sebagai OUTPUT menggunakan `pinMode`.

`loop()`
Loop berjalan dari i=15 (F) turun hingga i=0 (angka 0), memanggil
`displayDigit(i)` lalu menunggu 1000ms (1 detik) sebelum pindah ke digit berikutnya.
Setelah mencapai 0, loop mengulang kembali dari F.
```
