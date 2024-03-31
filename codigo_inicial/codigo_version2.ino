//librerías
#include <Servo.h> //librería servo
#include <LiquidCrystal.h> //librería lcd (pantalla)
#include <SoftwareSerial.h> 
#include <Adafruit_Fingerprint.h> //librería de la huella dactilar

Servo myServo;
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);
SoftwareSerial BTSerial(2,3); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&BTSerial);
uint8_t id;

// Variable global para almacenar los datos recibidos por Bluetooth
String bluetoothData = "";

// Función para leer los datos recibidos por Bluetooth
String leerDatosBluetooth() {
  String data = "";
  if (BTSerial.available()) {
    while (BTSerial.available()) {
      char c = BTSerial.read();
      data += c;
    }
  }
  return data;
}

void setup_bluetooth() {
  BTSerial.begin(9600);
}

// Función para escribir en la pantalla LCD
void escribirEnLCD(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void setup_lcd() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  lcd.cursor();
  lcd.print("App conectada!"); //lo que se escribe en la pantalla
}

// Función para controlar el servo motor
void controlServo() {
  myServo.write(0);
  delay(1000);
  myServo.write(90);
  delay(1000);
  myServo.write(180);
  delay(1000);
}

void setup_servo() {
  myServo.attach(9); //servo -> pin 9
}

void setup() {
  Serial.begin(9600);
  setup_bluetooth();
  setup_lcd();
  setup_servo();
  bluetoothData = leerDatosBluetooth();
}

void loop() {// Leer del bluetooth todo el rato, escribir por LCD un mensaje y mover el servo
  bluetoothData = leerDatosBluetooth();
  escribirEnLCD("Nuevo mensaje");
  controlServo();
}
