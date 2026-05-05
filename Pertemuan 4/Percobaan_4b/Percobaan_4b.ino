#include <Arduino.h>

// ===================== PIN SETUP =====================
const int potPin = A0;   // pin analog potensiometer
const int ledPin = 9;    // pin PWM LED

// ===================== VARIABEL =====================
int nilaiADC = 0;  // nilai pembacaan ADC
int pwm = 0;       // nilai PWM

void setup() {
  // ===================== OUTPUT SETUP =====================
  pinMode(ledPin, OUTPUT);

  // ===================== SERIAL MONITOR =====================
  Serial.begin(9600);
}

void loop() {
  // ===================== PEMBACAAN SENSOR =====================
  nilaiADC = analogRead(potPin);

  // ===================== PEMROSESAN DATA =====================
  pwm = map(nilaiADC,
            0,     // ADC min
            1023,  // ADC max
            0,     // PWM min
            255    // PWM max
           );

  // ===================== OUTPUT PWM =====================
  analogWrite(ledPin, pwm);

  // ===================== MONITORING =====================
  Serial.print("ADC: ");
  Serial.print(nilaiADC);

  Serial.print(" | PWM: ");
  Serial.println(pwm);

  // ===================== STABILISASI =====================
  delay(50);
}