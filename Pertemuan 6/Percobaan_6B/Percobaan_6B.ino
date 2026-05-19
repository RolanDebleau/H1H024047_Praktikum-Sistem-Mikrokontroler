#include <Arduino.h>

// Menyimpan waktu (milidetik) saat terakhir kali LED berubah kondisi
// Tipe unsigned long karena millis() bisa mencapai angka sangat besar (~50 hari)
unsigned long previousMillis = 0;

// Interval kedip LED dalam milidetik (1000 ms = 1 detik)
const long interval = 1000;

// Status LED saat ini (true = ON, false = OFF)
bool ledState = false;

void setup() {
  // Konfigurasi pin 13 sebagai OUTPUT untuk LED
  pinMode(13, OUTPUT);
}

void loop() {
  // Ambil waktu saat ini sejak Arduino menyala (dalam milidetik)
  unsigned long currentMillis = millis();

  // Cek apakah selisih waktu sekarang dengan waktu terakhir sudah >= interval
  // Ini adalah teknik non-blocking: program tetap jalan, tidak berhenti seperti delay()
  if (currentMillis - previousMillis >= interval) {

    // Simpan waktu saat ini sebagai acuan interval berikutnya
    previousMillis = currentMillis;

    // Toggle status LED
    ledState = !ledState;

    // Tulis status baru ke pin LED
    digitalWrite(13, ledState);
  }

  // Di sini bisa ditambahkan kode lain dan tetap akan berjalan
  // karena program tidak terblokir oleh delay
}