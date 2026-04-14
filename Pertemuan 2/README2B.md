# Modul 2 – Pemrograman Input dan Output
## Pertanyaan Praktikum Percobaan 2b : Kontrol Counter Dengan Push Button

1. Gambarkan rangkaian schematic yang digunakan pada percobaan!

https://www.tinkercad.com/things/i2RQvjwcB3Q-seven-segment-push-button?sharecode=cdP2J53NFayTGEbXSKq-F-3RJ3uJjfElaLF1q8AjMuE

2. Mengapa pada push button digunakan mode INPUT_PULLUP? Apa keuntungannya?

Mode INPUT_PULLUP mengaktifkan resistor pull-up internal Arduino (~20–50kΩ) yang menghubungkan pin ke VCC (5V) secara internal. Ketika tombol tidak ditekan, pin membaca logika HIGH. Ketika tombol ditekan dan menghubungkan pin ke GND, pin membaca logika LOW.

Keuntungannya dibanding rangkaian biasa (tanpa pull-up):

- Tanpa resistor eksternal — pin tidak dalam kondisi mengambang (floating) yang bisa membaca nilai acak. Mode ini mengeliminasi kebutuhan resistor pull-up fisik di breadboard.

- Lebih hemat komponen — tidak perlu menambahkan resistor 10kΩ eksternal.

- Sinyal lebih stabil — nilai logika terdefinisi dengan jelas (HIGH saat lepas, LOW saat ditekan), menghindari noise/glitch.

3. Jika salah satu LED segmen tidak menyala, apa kemungkinan penyebabnya?

Dari sisi hardware:

- Kabel jumper putus atau tidak terpasang dengan benar di breadboard

- Resistor 220Ω rusak atau nilai resistansi salah (terlalu besar sehingga arus terlalu kecil)

- LED segmen dalam seven segment terbakar/rusak

- Sambungan ke common cathode/anode longgar atau terputus

- Pin Arduino tersebut rusak secara fisik

Dari sisi software:

- Pemetaan pin di array segmentPins[] salah (nomor pin tidak sesuai kabel)

- Pola bit di digitPattern untuk segmen tertentu selalu bernilai 0 secara tidak sengaja

- pinMode() untuk pin tersebut tidak dipanggil atau salah dipanggil sebagai INPUT

4. Modifikasi dengan dua push button (increment & decrement) beserta README.md

```cpp
#include <Arduino.h>

// Pin mapping segmen: a b c d e f g dp
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

// Pin push button
const int btnUp   = 2; // tombol tambah (increment)
const int btnDown = 3; // tombol kurang (decrement)

// CC: 1 = ON, 0 = OFF — urutan segmen: a b c d e f g dp
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

int currentDigit = 0; // digit yang sedang ditampilkan (0–15)

// State tombol sebelumnya untuk edge detection
bool lastUpState   = HIGH;
bool lastDownState = HIGH;

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
  // Set pin tombol sebagai INPUT dengan pull-up internal
  pinMode(btnUp,   INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);

  displayDigit(currentDigit); // tampilkan angka awal (0)
}

void loop() {
  bool upState   = digitalRead(btnUp);
  bool downState = digitalRead(btnDown);

  // Tombol UP ditekan (falling edge: HIGH -> LOW)
  if (lastUpState == HIGH && upState == LOW) {
    currentDigit++;
    if (currentDigit > 15) currentDigit = 0; // wrap balik ke 0
    displayDigit(currentDigit);
  }

  // Tombol DOWN ditekan (falling edge: HIGH -> LOW)
  if (lastDownState == HIGH && downState == LOW) {
    currentDigit--;
    if (currentDigit < 0) currentDigit = 15; // wrap balik ke F
    displayDigit(currentDigit);
  }

  // Simpan state saat ini untuk iterasi berikutnya
  lastUpState   = upState;
  lastDownState = downState;
}
```

```cpp
Deskripsi
Program counter heksadesimal (0–F) yang dikendalikan oleh dua push button:
satu untuk increment (naik) dan satu untuk decrement (turun).

Konfigurasi Pin
| Komponen       | Pin Arduino |
|----------------|------------|
| Segmen a       | 7          |
| Segmen b       | 6          |
| Segmen c       | 5          |
| Segmen d       | 11         |
| Segmen e       | 10         |
| Segmen f       | 8          |
| Segmen g       | 9          |
| Segmen dp      | 4          |
| Tombol UP      | 2          |
| Tombol DOWN    | 3          |
| COM seven seg  | GND        |

Penjelasan Kode

 `btnUp = 2`, `btnDown = 3`
Mendefinisikan nomor pin untuk tombol increment dan decrement.

 `currentDigit`
Variabel integer menyimpan nilai yang sedang ditampilkan (range 0–15).

 `lastUpState`, `lastDownState`
Menyimpan kondisi tombol pada iterasi sebelumnya, digunakan untuk
mendeteksi *falling edge* (momen tombol baru saja ditekan).

 `displayDigit(int num)`
Menulis pola HIGH/LOW ke tiap pin segmen sesuai `digitPattern[num]`.

 `setup()`
- Mengatur semua pin segmen sebagai OUTPUT.
- Mengatur kedua pin tombol sebagai INPUT_PULLUP (menggunakan resistor
  pull-up internal Arduino, tidak perlu resistor eksternal).
- Menampilkan digit awal (0) saat program mulai.

 `loop()`
- Membaca kondisi kedua tombol setiap iterasi.
- **Increment**: jika tombol UP baru saja ditekan (lastUpState=HIGH,
  upState=LOW), naikkan currentDigit. Jika melebihi 15, wrap ke 0.
- **Decrement**: jika tombol DOWN baru saja ditekan (lastDownState=HIGH,
  downState=LOW), turunkan currentDigit. Jika kurang dari 0, wrap ke 15.
- Simpan state tombol saat ini ke lastUpState/lastDownState untuk
  deteksi edge pada iterasi berikutnya.

Cara Kerja Edge Detection
Teknik ini membandingkan state tombol saat ini dengan state sebelumnya.
Tombol dianggap "baru ditekan" hanya ketika transisi HIGH→LOW terjadi,
sehingga counter hanya naik/turun satu kali per tekanan, bukan terus-menerus.
```
