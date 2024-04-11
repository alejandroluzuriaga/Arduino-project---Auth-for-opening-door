// Librerías
#include <Servo.h> // Librería servo
#include <LiquidCrystal.h> // Librería lcd (pantalla)
#include <SoftwareSerial.h>  //Librería Bluetooth
#include <Adafruit_Fingerprint.h> // Librería de la huella dactilar

// Declaración Bluetooth
SoftwareSerial BTSerial(2, 3);

// Declaración LCD
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Declaración Servo
Servo myServo;

// Declaración Sensor de Huella dactilar
SoftwareSerial FingerSerial(13, 11); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FingerSerial);

String appData = "";
char mensaje[100] = "";

void setup() {
  // Inicializar serial
  Serial.begin(9600);

  // Inicializar bluetooth
  BTSerial.begin(9600);

  // Inicializar pantalla LCD
  lcd.begin(16, 2);

  // Inicializar servo
  myServo.attach(A0);
 
  // Inicializar finger
  finger.begin(57600);

  // LCDmensaje -> App conectada!
  lcd.print("App conectada!");
}

void loop() {
  appData = leerDatosApp();
  
  if(appData == "1") {
    String numString = leerDatosApp();
    int numInt = numString.toInt();
    uint8_t id = (uint8_t) numInt;
    while (!crearHuella(id));
  }
  else if(appData == "2") {
    abrirPuerta();
  }
}

String leerDatosApp() {
  String data = "";
  if (BTSerial.available()) {
    while (BTSerial.available()) {
      char c = BTSerial.read();
      data += c;
    }
  }
  return data;
}

void abrirPuerta() {
  detectar_sensor();
  boolean huellaEncontrada = getFingerprintIDez();
  if(huellaEncontrada) {
    strcpy(mensaje, "Abriendo puerta");
    escribirEnLCD(mensaje, strlen(mensaje));
    controlServo();
  }
  else {
    strcpy(mensaje, "No estás autenticado/a!");
    escribirEnLCD(mensaje, strlen(mensaje));
  }
}

void controlServo() {
  myServo.write(0);
  delay(500);
  myServo.write(90);
}

boolean getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return false;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return false;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return false;

  strcpy(mensaje, "Huella encontrada");
  escribirEnLCD(mensaje, strlen(mensaje));
  return true;
}

void detectar_sensor() {
  if (finger.verifyPassword()) {
    strcpy(mensaje, "El sensor de huella dactilar se encuentra");
    escribirEnLCD(mensaje, strlen(mensaje));
  } 
  else {
    strcpy(mensaje, "El sensor de huella dactilar NO se encuentra");
    escribirEnLCD(mensaje, strlen(mensaje));
    while (1) { 
      delay(1); 
    }
  }
}

void escribirEnLCD(char *mensaje, int longitud) {
  for (int i = 0; i < longitud; ++i) {
    lcd.clear();
    lcd.setCursor(0, 0);

    String textoDesplazado = String(mensaje).substring(i) + String(mensaje).substring(0, i);

    lcd.print(textoDesplazado);
    delay(500); 
  }
}

uint8_t crearHuella(uint8_t id) { // Es la función "getFingerprintEnroll" del fichero "enroll"
  int p = -1;
  detectar_sensor();
  strcpy(mensaje, "Introduce la huella");
  escribirEnLCD(mensaje, strlen(mensaje));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  strcpy(mensaje, "Quita la huella");
  escribirEnLCD(mensaje, strlen(mensaje));
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;

  strcpy(mensaje, "Vuelve a introducir la huella");
  escribirEnLCD(mensaje, strlen(mensaje));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  strcpy(mensaje, "Huella almacenada correctamente");
  escribirEnLCD(mensaje, strlen(mensaje));

  return true;
}