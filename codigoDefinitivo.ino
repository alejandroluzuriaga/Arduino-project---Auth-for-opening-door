// Librerías
#include <Servo.h>                 // Librería Servo
#include <LiquidCrystal.h>         // Librería LCD
#include <SoftwareSerial.h>        // Librería Bluetooth
#include <Adafruit_Fingerprint.h>  // Librería de la huella dactilar

//Def. Pines
const int bt_txd = 2, bt_rxd = 3, lcd_d7 = 4, lcd_d6 = 5, lcd_d5 = 6, lcd_d4 = 7, lcd_en = 8, lcd_rs = 9, finger_green = 13, finger_yellow = 11;

// Declaración Bluetooth
SoftwareSerial BTSerial(bt_txd, bt_rxd);

// Declaración LCD
LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

// Declaración Servo
Servo myServo;

// Declaración Sensor de Huella dactilar
SoftwareSerial FingerSerial(finger_green, finger_yellow);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FingerSerial);

int appData = 0;
uint8_t huellaEncontrada = -1;

void setup() {
  // Inicializar bluetooth
  BTSerial.begin(9600);

  // Inicializar pantalla LCD
  lcd.begin(16, 2);

  // Inicializar servo
  myServo.attach(A0);

  myServo.write(0);
  delay(1000);
  myServo.write(90);

  pantallaEspera();
}

void loop() {
  if (BTSerial.available()) {

    appData = BTSerial.read() - '0';
    
    if (appData == 1) { // Crear huella
      while (!BTSerial.available());
      String numString = "0";
      numString = leerID();
      delay(500);
      if (numString != "0") {
        int numInt = numString.toInt();
        uint8_t id = (uint8_t)numInt;
        while (id != 0 && !crearHuella(id));
        BTSerial.begin(9600);
        BTSerial.write("0");
        BTSerial.flush();
        lcd.clear();
        pantallaEspera();
      }
    } else if (appData == 2) { // Abrir puerta
      abrirPuerta();
      BTSerial.begin(9600);
      BTSerial.write("0");
      BTSerial.flush();
      lcd.clear();
      pantallaEspera();
    } else if (appData == 3) { //Conectado
      escribirEnLCDFijo("Conectado.");
      BTSerial.begin(9600);
      pantallaEspera();
    }
  }
  delay(100);
}

//Funciones auxiliares----------------------------------------------------------------------------

void pantallaEspera() {
  int contadorPuntos = 0;
  lcd.setCursor(0, 0);
  while (!BTSerial.available()) {
    lcd.clear();
    lcd.print("Esperando app"); 

    for (int i = 0; i < contadorPuntos; i++) {
      lcd.print(".");
    }

    contadorPuntos = (contadorPuntos + 1) % 4;
    delay(500);                                 
  }
}

//------------------------------------------------------------------------------------------------

String leerID() {
  String data = "";
  while (BTSerial.available()) {
    delay(500);
    char c = BTSerial.read();
    data += c;
  }
  return data;
}

//------------------------------------------------------------------------------------------------

void abrirPuerta() {
  finger.begin(57600);

  detectar_sensor();
  delay(500);
  huellaEncontrada = getFingerprintIDez();
  delay(500);
  if (huellaEncontrada != 255) {  //255 es el número de error en unit8_t
    escribirEnLCDFijo("Puerta abierta.");
    controlServo();
  } else {
    escribirEnLCDFijo("Error de huella.");
  }
}

//------------------------------------------------------------------------------------------------

void controlServo() {
  myServo.write(0);  // Abre la puerta
  delay(7000);
  myServo.write(90);  // Cierra la puerta
}

//------------------------------------------------------------------------------------------------

void detectar_sensor() {
  if (finger.verifyPassword()) {
  } else {
    escribirEnLCDFijo("Sensor NO OK. ");
    while (1) {
      delay(1);
    }
  }
}

//------------------------------------------------------------------------------------------------

void escribirEnLCDFijo(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(2000);
}

//------------------------------------------------------------------------------------------------

uint8_t getFingerprintIDez() {
  escribirEnLCDFijo("Pon el dedo.");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  escribirEnLCDFijo("Huella Valida. ");
  return finger.fingerID;
}

//------------------------------------------------------------------------------------------------

uint8_t crearHuella(uint8_t id) {
  int p = -1;
  finger.begin(57600);

  detectar_sensor();
  escribirEnLCDFijo("Pon el dedo.");

  //Se toma una captura de la huella (tantas veces cuantas hagas falta)
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  escribirEnLCDFijo("Mantenlo...");

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
      escribirEnLCDFijo("Error!");
      escribirEnLCDFijo("Repite proceso");
      return p;
  }

  escribirEnLCDFijo("Retiralo...");

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;

  escribirEnLCDFijo("Pon el dedo.");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  escribirEnLCDFijo("Mantenlo...");

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
        escribirEnLCDFijo("Error!");
        escribirEnLCDFijo("Repite proceso");
      return p;
  }

  escribirEnLCDFijo("Retiralo...");

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    escribirEnLCDFijo("Error!");
    escribirEnLCDFijo("Repite proceso");
    return p;
  }

  p = finger.storeModel(id);
  if (p != FINGERPRINT_OK) {
      escribirEnLCDFijo("Error!");
      escribirEnLCDFijo("Repite proceso");
    return p;
  }

  escribirEnLCDFijo("Huella guardada.");

  return true;
}