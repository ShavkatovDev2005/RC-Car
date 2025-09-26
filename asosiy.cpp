#include <Servo.h>

#define IBUS_BUFFSIZE 32
uint8_t ibus[IBUS_BUFFSIZE];
uint8_t idx = 0;

// Servo obyektlari
Servo servo1;
Servo servo2;
Servo steering;   // rul

// Pinlar
int motorPin = 5;     // motor uchun PWM chiqish
int signalPin = 7;    // CH5 signali

void setup() {
  Serial.begin(115200); // i-BUS shu portga ulangan

  // Servolar
  servo1.attach(9);    // CH3
  servo2.attach(10);   // CH4
  steering.attach(6);  // CH2 rul (servo)

  // Motor va signal
  pinMode(motorPin, OUTPUT);
  pinMode(signalPin, OUTPUT);
}

void loop() {
  while (Serial.available()) {
    uint8_t b = Serial.read();
    if (idx == 0 && b != 0x20) continue; // paket uzunligi
    ibus[idx++] = b;
    if (idx == IBUS_BUFFSIZE) {
      idx = 0;

      for (int i = 0; i < 10; i++) {
        int ch = ibus[2*i+2] | (ibus[2*i+3] << 8);
        Serial.print("CH"); Serial.print(i+1);
        Serial.print(": "); Serial.print(ch);
        Serial.print("  ");

        // CH1 -> motor (PWM)
        if (i == 0) {
          int pwmVal = map(ch, 1000, 2000, 0, 255); // 0–255 oralig‘ida
          analogWrite(motorPin, pwmVal);
        }

        // CH2 -> rul (±90°)
        if (i == 1) {
          int pos = map(ch, 1000, 2000, 0, 180); 
          steering.write(pos);
        }

        // CH3 -> servo1
        if (i == 2) {
          int pos = map(ch, 1000, 2000, 0, 180);
          servo1.write(pos);
        }

        // CH4 -> servo2
        if (i == 3) {
          int pos = map(ch, 1000, 2000, 0, 180);
          servo2.write(pos);
        }

        // CH5 -> signal
        if (i == 4) {
          if (ch > 1500) {
            digitalWrite(signalPin, HIGH);
          } else {
            digitalWrite(signalPin, LOW);
          }
        }
      }
      Serial.println();
    }
  }
}
