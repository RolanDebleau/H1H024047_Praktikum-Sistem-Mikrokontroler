# Modul I – Percabangan dan Perulangan
## Pertanyaan Praktikum Percobaan 1A : Percabangan

1. Pada kondisi apa program masuk ke blok if?
Program masuk ke blok if ketika nilai timeDelay <= 100. Artinya, setelah LED berkedip semakin cepat dan delay sudah mencapai 100ms atau kurang, program akan masuk ke blok ini untuk memberikan jeda 3 detik lalu mereset timeDelay kembali ke 1000.

2. Pada kondisi apa program masuk ke blok else?
Program masuk ke blok else ketika nilai timeDelay > 100. Setiap siklus kedip selesai, nilai timeDelay dikurangi 100ms, sehingga LED berkedip semakin cepat secara bertahap.

3. Apa fungsi dari perintah delay(timeDelay)?
Fungsi delay(timeDelay) digunakan untuk menghentikan eksekusi program sementara selama durasi waktu (dalam milidetik) yang ditentukan oleh nilai variabel timeDelay. Ini yang menyebabkan LED tampak menyala atau mati dalam durasi tertentu, sehingga kecepatan kedip LED bisa berubah-ubah sesuai nilai timeDelay.

4. Ubah alur dari cepat → sedang → mati (tanpa langsung reset)
```cpp
// Program LED dengan alur: mati → lambat → cepat → sedang → mati

const int ledPin = 6;     // LED terhubung ke pin 6
int timeDelay = 1000;     // Delay awal = 1000ms (lambat)
bool slowing = false;     // Flag untuk fase melambat (cepat → sedang)

void setup() {
  pinMode(ledPin, OUTPUT); // Set pin 6 sebagai OUTPUT
}

void loop() {
  // Nyalakan LED selama timeDelay ms
  digitalWrite(ledPin, HIGH);
  delay(timeDelay);

  // Matikan LED selama timeDelay ms
  digitalWrite(ledPin, LOW);
  delay(timeDelay);

  if (!slowing) {
    // FASE 1: Percepatan (lambat → cepat)
    if (timeDelay <= 100) {
      // Sudah mencapai kecepatan maksimum
      // Masuk ke fase melambat
      slowing = true;
      timeDelay = 500; // Reset ke kecepatan sedang
    } else {
      timeDelay -= 100; // Kurangi delay = percepat kedip
    }
  } else {
    // FASE 2: Perlambatan (cepat → sedang → mati)
    if (timeDelay >= 1000) {
      // Sudah kembali ke kecepatan lambat = berhenti (mati)
      digitalWrite(ledPin, LOW); // Pastikan LED mati
      delay(3000);               // Jeda 3 detik sebelum ulang
      timeDelay = 1000;          // Reset delay ke awal
      slowing = false;           // Kembali ke fase percepatan
    } else {
      timeDelay += 100; // Tambah delay = perlambat kedip
    }
  }
}
```

```cpp
Deskripsi
Program ini mengontrol LED pada pin 6 Arduino dengan pola:
mati → lambat → cepat → sedang → mati (tanpa langsung reset)

Penjelasan Baris Kode

| Bagian Kode | Penjelasan |
| `const int ledPin = 6`          | Mendefinisikan pin 6 sebagai pin LED |
| `int timeDelay = 1000`          | Delay awal 1000ms (LED kedip lambat) |
| `bool slowing = false`          | Flag penanda fase: false=percepatan, true=perlambatan |
| `pinMode(ledPin, OUTPUT)`       | Mengatur pin 6 sebagai output digital |
| `digitalWrite(ledPin, HIGH)`    | Menyalakan LED |
| `delay(timeDelay)`              | Menunggu selama timeDelay ms |
| `digitalWrite(ledPin, LOW)`     | Mematikan LED |
| `if (!slowing)`                 | Jika sedang dalam fase percepatan |
| `if (timeDelay <= 100)`         | Jika delay sudah mencapai minimum (cepat) |
| `slowing = true`                | Pindah ke fase perlambatan |
| `timeDelay = 500`               | Set ke kecepatan sedang |
| `timeDelay -= 100`              | Kurangi delay 100ms setiap siklus (percepat) |
| `if (timeDelay >= 1000)`        | Jika sudah kembali ke kecepatan lambat |
| `delay(3000)`                   | Jeda 3 detik sebelum mulai ulang |
| `timeDelay += 100`              | Tambah delay 100ms setiap siklus (perlambat) |

Alur Program
1. LED mulai mati
2. Mulai berkedip lambat (1000ms)
3. Semakin cepat hingga 100ms
4. Melambat ke 500ms (sedang)
5. Melambat terus hingga 1000ms → mati → jeda 3 detik → ulang
```
