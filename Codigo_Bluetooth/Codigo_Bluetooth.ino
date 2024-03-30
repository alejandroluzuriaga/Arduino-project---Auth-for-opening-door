#include <SoftwareSerial.h> // Se importa la libreria

SoftwareSerial BTSerial(2,3); // Se simula un Serial para el input y el output.

void setup() {
  // Incialización de los dos Serials
  Serial.begin(9600);
  BTSerial.begin(9600);
}

void loop() {
  //Búcle que comprueba si se ha escrito algo del bluetooth
  if(BTSerial.available()){
    Serial.write(BTSerial.read());
  }

  //Búcle que comprueba si se ha escrito algo del Serial Port del ordenador
  if(Serial.available()){
    BTSerial.write(Serial.read());
  }
}
