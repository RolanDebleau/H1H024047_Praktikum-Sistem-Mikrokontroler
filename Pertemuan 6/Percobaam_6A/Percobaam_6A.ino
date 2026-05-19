#include <Arduino.h>

// Variabel volatile agar nilainya bisa diubah di dalam ISR
// Tanpa volatile, compiler mungkin mengoptimasi dan mengabaikan perubahan dari ISR
volatile bool ledState = false;

// ISR (Interrupt Service Routine) - fungsi yang dipanggil otomatis saat interrupt terjadi
// Harus dibuat singkat, tidak boleh pakai delay() atau Serial.print()
void tombolInterrupt() {
  ledState = !ledState; // Toggle: jika true jadi false, jika false jadi true
}

void setup() {
  // Konfigurasi pin 13 sebagai OUTPUT untuk LED
  pinMode(13, OUTPUT);

  // Konfigurasi pin 2 sebagai INPUT dengan resistor pull-up internal
  // Artinya kondisi default pin 2 = HIGH, saat tombol ditekan = LOW
  pinMode(2, INPUT_PULLUP);

  // Daftarkan ISR pada interrupt di pin 2
  // digitalPinToInterrupt(2) = mengonversi pin 2 ke nomor interrupt (Interrupt 0)
  // tombolInterrupt           = nama fungsi ISR yang akan dijalankan
  // FALLING                   = interrupt dipicu saat sinyal berubah dari HIGH ke LOW
  attachInterrupt(
    digitalPinToInterrupt(2),
    tombolInterrupt,
    FALLING
  );
}

void loop() {
  // Program utama hanya membaca variabel ledState dan menulis ke pin LED
  // Tidak ada polling tombol di sini - semua ditangani oleh interrupt
  digitalWrite(13, ledState);
}