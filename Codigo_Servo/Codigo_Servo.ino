#include <Servo.h>

Servo myServo;

void setup() {
  Serial.begin(9600);
  myServo.attach(9); // Se inidica que el servo va a usar el pin 9 de la placa.
}

void loop() {
  // Se inidican los grados del servo con el write(grados)
  // Opera de 0 a 180 grados.
  myServo.write(0);
  delay(1000);
  myServo.write(90);
  delay(1000);
  myServo.write(180);
  delay(1000);
}
