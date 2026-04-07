# Modul I – Percabangan dan Perulangan
## Pertanyaan Praktikum Percobaan 2A : Perulangan

1. Gambar rangkaian schematic 5 LED running

https://www.tinkercad.com/things/2HjYmh1EObS-test-lampu-kiri-kanan?sharecode=sTt4LE7Brfkh0PR6c94d0aPkYgHqvb0A_B-pyBC44o8

2. Bagaimana program membuat efek LED berjalan dari kiri ke kanan?

Program menggunakan loop for dengan variabel ledPin yang dimulai dari nilai 2 (pin paling kiri) dan bertambah 1 setiap iterasi hingga nilai 7 (pin paling kanan):
```cpp
for (int ledPin = 2; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH); // nyalakan LED saat ini
    delay(timer);               // tunggu 100ms
    digitalWrite(ledPin, LOW);  // matikan sebelum pindah ke berikutnya
}
```
Efeknya: LED 1 nyala → mati → LED 2 nyala → mati → LED 3 nyala → mati → LED 4 nyala → mati → LED 5 nyala → mati → LED 6 nyala → mati, menciptakan ilusi LED "berjalan" ke kanan.

3. Bagaimana program membuat LED kembali dari kanan ke kiri?

Program menggunakan loop for kedua dengan variabel ledPin yang dimulai dari nilai 7 (kanan) dan berkurang 1 setiap iterasi hingga nilai 2 (kiri):
```cpp
for (int ledPin = 7; ledPin >= 2; ledPin--) {
    digitalWrite(ledPin, HIGH); // nyalakan LED saat ini
    delay(timer);               // tunggu 100ms
    digitalWrite(ledPin, LOW);  // matikan sebelum pindah
}
```
Ini adalah kebalikan dari loop sebelumnya, sehingga efek berjalan terlihat dari kanan ke kiri.

4. Program LED 3 kanan dan 3 kiri menyala bergantian
```cpp
// Program: 3 LED kiri dan 3 LED kanan menyala bergantian

int timer = 500; // Durasi nyala setiap kelompok LED (ms)

// Definisi pin untuk kelompok kiri dan kanan
int ledKiri[] = {2, 3, 4};  // 3 LED kelompok kiri
int ledKanan[] = {5, 6, 7}; // 3 LED kelompok kanan

void setup() {
  // Inisialisasi semua pin LED sebagai OUTPUT
  for (int i = 0; i < 3; i++) {
    pinMode(ledKiri[i], OUTPUT);  // Set pin LED kiri sebagai output
    pinMode(ledKanan[i], OUTPUT); // Set pin LED kanan sebagai output
  }
}

// Fungsi untuk menyalakan semua LED dalam array
void nyalakanGrup(int pins[], int jumlah) {
  for (int i = 0; i < jumlah; i++) {
    digitalWrite(pins[i], HIGH); // Nyalakan setiap LED dalam grup
  }
}

// Fungsi untuk mematikan semua LED dalam array
void matikanGrup(int pins[], int jumlah) {
  for (int i = 0; i < jumlah; i++) {
    digitalWrite(pins[i], LOW); // Matikan setiap LED dalam grup
  }
}

void loop() {
  // Fase 1: Nyalakan 3 LED KIRI, matikan 3 LED KANAN
  nyalakanGrup(ledKiri, 3);   // LED pin 2,3,4 menyala
  matikanGrup(ledKanan, 3);   // LED pin 5,6,7 mati
  delay(timer);               // Tahan selama 500ms

  // Fase 2: Matikan 3 LED KIRI, nyalakan 3 LED KANAN
  matikanGrup(ledKiri, 3);    // LED pin 2,3,4 mati
  nyalakanGrup(ledKanan, 3);  // LED pin 5,6,7 menyala
  delay(timer);               // Tahan selama 500ms
}
```
```cpp
Deskripsi
Program ini membuat 6 LED (pin 2-7) menyala bergantian:
- 3 LED kiri (pin 2,3,4) menyala → mati
- 3 LED kanan (pin 5,6,7) menyala → mati
- Berulang terus menerus

Wiring
| LED | Pin Arduino |
|-----|-------------|
| LED Kiri 1   | Pin 2 |
| LED Kiri 2   | Pin 3 |
| LED Kiri 3   | Pin 4 |
| LED Kanan 1  | Pin 5 |
| LED Kanan 2  | Pin 6 |
| LED Kanan 3  | Pin 7 |

Penjelasan Baris Kode

| Bagian Kode | Penjelasan |
|---|---|
| `int timer = 500`               | Durasi nyala tiap kelompok LED = 500ms |
| `int ledKiri[] = {2,3,4}`       | Array berisi nomor pin untuk 3 LED kiri |
| `int ledKanan[] = {5,6,7}`      | Array berisi nomor pin untuk 3 LED kanan |
| `pinMode(ledKiri[i], OUTPUT)`   | Set setiap pin LED kiri sebagai output |
| `pinMode(ledKanan[i], OUTPUT)`  | Set setiap pin LED kanan sebagai output |
| `nyalakanGrup(pins[], jumlah)`  | Fungsi untuk menyalakan semua LED dalam grup |
| `matikanGrup(pins[], jumlah)`   | Fungsi untuk mematikan semua LED dalam grup |
| `digitalWrite(pins[i], HIGH)`   | Menyalakan LED pada pin tertentu |
| `digitalWrite(pins[i], LOW)`    | Mematikan LED pada pin tertentu |
| `nyalakanGrup(ledKiri, 3)`      | Nyalakan 3 LED kiri sekaligus |
| `matikanGrup(ledKanan, 3)`      | Matikan 3 LED kanan sekaligus |
| `delay(timer)`                  | Tahan kondisi selama 500ms |
```
