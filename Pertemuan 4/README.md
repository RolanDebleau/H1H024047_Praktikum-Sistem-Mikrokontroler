Nama: Gerard Roland Kusuma Sarwoko  
NIM: H1H024047  
Shift Awal: B  
Shift Akhir: B  

# Modul 4 — ADC dan PWM

## Percobaan 4A — Analog to Digital Converter (ADC)

### 4.5 Pertanyaan Praktikum

**1. Apa fungsi perintah `analogRead()` pada rangkaian praktikum ini?**

Fungsi `analogRead(potensioPin)` bertugas membaca tegangan analog yang dihasilkan oleh potensiometer pada pin A0 dan mengonversinya menjadi nilai integer antara 0 sampai 1023 menggunakan ADC 10-bit internal Arduino. Ketika potensiometer diputar, resistansinya berubah secara bertahap sehingga tegangan yang masuk ke pin A0 pun berubah secara linier antara 0V dan 5V — perubahan tegangan inilah yang ditangkap oleh `analogRead()` dan dijadikan data numerik yang dapat diproses lebih lanjut oleh program. Dalam konteks praktikum ini, nilai tersebut menjadi representasi digital dari posisi fisik potensiometer yang kemudian digunakan untuk menentukan sudut servo.

**2. Mengapa diperlukan fungsi `map()` dalam program tersebut?**

Fungsi `map()` diperlukan karena ADC menghasilkan nilai dalam rentang 0–1023, sementara motor servo hanya menerima sudut dalam rentang 0–180 derajat — keduanya berbeda skala, sehingga tidak bisa langsung digunakan satu sama lain tanpa konversi. Fungsi `map(val, 0, 1023, 0, 180)` melakukan pemetaan linier proporsional: nilai ADC 0 akan menghasilkan sudut 0°, nilai 511 menghasilkan sekitar 90°, dan nilai 1023 menghasilkan 180°, sehingga gerak servo selalu selaras dan proporsional dengan putaran potensiometer. Tanpa `map()`, kita harus menghitung skala secara manual menggunakan operasi aritmatika, yang lebih rawan kesalahan dan kurang mudah dibaca.

**3. Modifikasi program berikut agar servo hanya bergerak dalam rentang 30° hingga 150°, meskipun potensiometer tetap memiliki rentang ADC 0–1023. Jelaskan program pada file README.md**

```cpp
#include <Servo.h>

Servo myservo;

const int potensioPin = A0;
const int servoPin = 9;

int pos = 0;
int val = 0;

void setup() {
  myservo.attach(servoPin);
  Serial.begin(9600);
}

void loop() {
  val = analogRead(potensioPin);

  // Mengubah batas sudut minimum dari 0 menjadi 30 dan maksimum dari 180 menjadi 150
  // sehingga meskipun potensiometer berputar penuh (ADC 0–1023), 
  // servo hanya akan bergerak dalam rentang 30° hingga 150°
  pos = map(val,
            0,    // Nilai minimum ADC
            1023, // Nilai maksimum ADC
            30,   // Sudut minimum servo (dibatasi 30°, bukan 0°)
            150); // Sudut maksimum servo (dibatasi 150°, bukan 180°)

  myservo.write(pos);

  Serial.print("ADC Potensio: ");
  Serial.print(val);
  Serial.print(" | Sudut Servo: ");
  Serial.println(pos);

  delay(15);
}
```

Modifikasi yang dilakukan hanya pada dua argumen terakhir fungsi `map()` — batas bawah diubah dari `0` menjadi `30` dan batas atas dari `180` menjadi `150`. Dengan perubahan ini, ketika potensiometer berada di posisi minimum (ADC = 0), servo diperintahkan ke 30° (bukan 0°), dan ketika potensiometer di posisi maksimum (ADC = 1023), servo hanya bergerak hingga 150° (bukan 180°). Hal ini berguna dalam situasi nyata seperti mengontrol klep, sendi robot, atau mekanisme lain yang secara fisik memiliki batas gerak aman yang lebih sempit dari rentang penuh servo.

---

## Percobaan 4B — Pulse Width Modulation (PWM)

### 4.6 Pertanyaan Praktikum

**1. Jelaskan mengapa LED dapat diatur kecerahannya menggunakan fungsi `analogWrite()`!**

Meskipun Arduino hanya menghasilkan sinyal digital (HIGH = 5V atau LOW = 0V), LED dapat terlihat redup atau terang dengan memanfaatkan sifat inersia visual mata manusia melalui teknik PWM. Fungsi `analogWrite()` menghasilkan sinyal yang berulang sangat cepat (sekitar 490–980 Hz pada Arduino Uno) di mana pin bergantian antara HIGH dan LOW dengan proporsi waktu tertentu yang disebut *duty cycle*. Ketika nilai PWM adalah 255, pin HIGH 100% waktu sehingga LED menyala penuh; ketika nilainya 128, pin HIGH hanya 50% waktu sehingga rata-rata daya yang diterima LED hanya setengahnya dan LED terlihat setengah terang; ketika nilainya 0, pin selalu LOW dan LED mati. Karena pergantian ini terjadi jauh lebih cepat dari kemampuan mata untuk mendeteksinya (manusia tidak bisa membedakan kedipan di atas ~50Hz), otak kita memproses hasilnya sebagai tingkat kecerahan yang kontinu dan bertahap, bukan sebagai kedipan.

**2. Apa hubungan antara nilai ADC (0–1023) dan nilai PWM (0–255)?**

Keduanya memiliki hubungan proporsional linier setelah melalui fungsi `map()`, di mana nilai ADC 0 akan selalu menghasilkan PWM 0, nilai ADC 1023 akan selalu menghasilkan PWM 255, dan nilai di antaranya dipetakan secara linier. Secara matematis, konversi ini setara dengan rumus `pwm = (nilaiADC / 1023.0) * 255`, yang pada intinya melakukan normalisasi nilai ADC ke rentang 0–1 lalu mengalikannya dengan 255. Perbedaan resolusinya perlu diperhatikan: ADC memiliki resolusi 10-bit (1024 langkah) sementara PWM hanya 8-bit (256 langkah), sehingga setiap satu langkah PWM setara dengan sekitar 4 langkah ADC — ini berarti ada sedikit kuantisasi ketika nilai ADC berubah kurang dari 4 langkah, PWM tidak akan berubah.

**3. Modifikasilah program berikut agar LED hanya menyala pada rentang kecerahan sedang, yaitu hanya ketika nilai PWM berada pada rentang 50 sampai 200. Jelaskan program pada file README.md.**

```cpp
#include <Arduino.h>

const int potPin = A0;
const int ledPin = 9;

int nilaiADC = 0;
int pwm = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  nilaiADC = analogRead(potPin);

  // Memetakan seluruh rentang ADC (0–1023) hanya ke rentang PWM 50–200
  // sehingga pada posisi minimum potensiometer pun LED tidak benar-benar mati (PWM = 50)
  // dan pada posisi maksimum LED tidak menyala penuh (PWM = 200)
  pwm = map(nilaiADC,
            0,    // Nilai minimum ADC
            1023, // Nilai maksimum ADC
            50,   // Nilai minimum PWM (kecerahan rendah, bukan mati)
            200); // Nilai maksimum PWM (kecerahan tinggi, bukan maksimum penuh)

  analogWrite(ledPin, pwm);

  Serial.print("ADC: ");
  Serial.print(nilaiADC);
  Serial.print(" | PWM: ");
  Serial.println(pwm);

  delay(50);
}
```

Modifikasi ini hanya mengubah dua argumen terakhir pada fungsi `map()` — batas bawah PWM diubah dari `0` menjadi `50` dan batas atas dari `255` menjadi `200`. Dengan demikian, meskipun potensiometer diputar ke posisi minimum (ADC = 0), LED tetap menyala dengan kecerahan rendah (PWM = 50, sekitar 20% duty cycle), dan meskipun potensiometer diputar ke posisi penuh (ADC = 1023), LED tidak akan menyala dengan intensitas tertinggi (PWM = 200, sekitar 78% duty cycle). Pendekatan ini sangat berguna dalam aplikasi nyata seperti lampu ambient yang tidak boleh terlalu redup agar tetap berfungsi sebagai penerangan, atau lampu indikator yang tidak boleh terlalu terang agar tidak menyilaukan, semuanya dikendalikan dengan mempersempit rentang duty cycle yang digunakan.

---
## 4.7 Pertanyaan Analisis

**1. Bagaimana proses konversi sinyal analog menjadi digital pada Arduino?**

ADC (Analog to Digital Converter) pada Arduino bekerja dengan cara mengambil tegangan analog yang masuk melalui pin analog (A0–A5) dan mengonversinya menjadi nilai digital menggunakan teknik *successive approximation*. Arduino Uno memiliki resolusi ADC 10-bit, yang berarti tegangan referensi (defaultnya 5V) dibagi menjadi 2¹⁰ = 1024 tingkatan nilai diskrit, yaitu dari 0 hingga 1023. Ketika fungsi `analogRead()` dipanggil, Arduino mengukur tegangan pada pin tersebut lalu membandingkannya secara bertahap dengan tegangan referensi internal menggunakan komparator dan DAC internal, hasilnya adalah nilai integer yang merepresentasikan besar tegangan input — misalnya tegangan 2,5V akan menghasilkan nilai sekitar 512 karena berada tepat di tengah rentang 0–5V, sehingga nilai ini kemudian bisa digunakan dalam perhitungan atau pemetaan ke variabel lain seperti sudut servo atau kecerahan LED.

---

**2. Faktor apa saja yang dapat mempengaruhi keakuratan pembacaan ADC?**

Ada beberapa faktor yang secara nyata dapat menurunkan akurasi hasil pembacaan ADC pada Arduino. Pertama adalah *noise* pada jalur tegangan suplai (VCC) dan jalur ground — jika tegangan tidak stabil, maka tegangan referensi ADC pun ikut berfluktuasi sehingga hasil konversi menjadi tidak konsisten. Kedua adalah impedansi sumber sinyal yang terlalu tinggi; ADC Arduino membutuhkan impedansi sumber di bawah sekitar 10kΩ agar kapasitor sampling internal dapat terisi penuh dalam waktu satu siklus clock ADC, sehingga jika impedansi sumber terlalu tinggi maka hasil pembacaan akan bias ke nilai sebelumnya. Ketiga adalah resolusi ADC itu sendiri — dengan 10 bit dan referensi 5V, satu langkah ADC setara dengan sekitar 4,88mV, artinya perubahan tegangan lebih kecil dari itu tidak akan terdeteksi sama sekali. Keempat adalah interferensi elektromagnetik (EMI) dari komponen di sekitarnya seperti motor atau relay yang dapat menginduksi tegangan parasit ke jalur analog. Terakhir, suhu juga berpengaruh karena karakteristik elektronik internal ADC dapat bergeser seiring perubahan suhu, meskipun pada aplikasi praktikum sederhana faktor ini biasanya diabaikan.

---

**3. Apa kendala yang mungkin terjadi saat mengintegrasikan ADC dan PWM dalam satu sistem?**

Ketika ADC dan PWM dijalankan bersamaan dalam satu sistem, ada beberapa kendala yang perlu diperhatikan. Pertama, sinyal PWM yang dihasilkan oleh timer internal Arduino dapat menjadi sumber *noise* digital yang merambat ke jalur analog dan mengganggu pembacaan ADC — solusinya adalah menambahkan kapasitor filter pada pin analog atau memisahkan ground digital dan analog. Kedua, frekuensi sampling ADC dan frekuensi PWM bisa saling berinterferensi jika tidak diselaraskan, sehingga nilai ADC yang terbaca kadang tidak mencerminkan kondisi sesungguhnya. Ketiga, jika beban pada pin PWM (seperti motor) menyebabkan lonjakan arus, tegangan VCC bisa turun sesaat dan ini akan langsung memengaruhi akurasi ADC karena referensinya ikut berubah. Keempat, dalam program dengan `delay()` yang panjang, pembaruan nilai ADC dan respons PWM menjadi lambat sehingga sistem terasa *lag* — ini bisa diatasi dengan menggunakan teknik *non-blocking* menggunakan `millis()`.

---
