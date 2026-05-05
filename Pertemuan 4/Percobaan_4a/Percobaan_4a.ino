#include <Servo.h> // library untuk servo motor

Servo myservo; // membuat objek servo

// ===================== PIN SETUP =====================
const int potensioPin = A0;   // pin analog input
const int servoPin = 9;       // pin PWM servo

// ===================== VARIABEL =====================
int pos = 0; // sudut servo
int val = 0; // nilai ADC

void setup() {

  // Hubungkan servo ke pin
  myservo.attach(servoPin);

  // Serial monitor
  Serial.begin(9600);

}

void loop() {

  // ===================== PEMBACAAN ADC =====================
  val = analogRead(potensioPin);

  // ===================== KONVERSI DATA =====================
  pos = map(val,
            0,    // min ADC
            1023, // max ADC
            0,    // sudut min
            180   // sudut max
            );

  // ===================== OUTPUT SERVO =====================
  myservo.write(pos);

  // ===================== MONITORING =====================
  Serial.print("ADC Potensio: ");
  Serial.print(val);

  Serial.print(" | Sudut Servo: ");
  Serial.println(pos);

  // ===================== STABILISASI =====================
  delay(100);

}