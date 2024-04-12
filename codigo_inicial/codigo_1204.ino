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
uint8_t huellaEncontrada = -1;

void setup() {
  // Inicializar serial
  Serial.begin(9600);

  // Inicializar bluetooth
  BTSerial.begin(9600);

  // Inicializar pantalla LCD
  lcd.begin(16, 2);

  // Inicializar servo
  myServo.attach(A0);

  // LCDmensaje -> App conectada!
  lcd.print("App conectada!");
}

void loop() {
  if(BTSerial.available()) {
    appData = leerDatosApp();
  
    if(appData == "1") {
      delay(2000);
      String numString = "0";
      while(numString.equals("0")) {
        numString = leerDatosApp();
      }
      
      int numInt = numString.toInt();
      uint8_t id = (uint8_t) numInt;
      Serial.print("El id de la app es: "); Serial.println(id);
      while (id != 0 && !crearHuella(id));
    }
    else if(appData == "2") {
      abrirPuerta();
    }
  }
}

String leerDatosApp() {
  String data = "";
  if (BTSerial.available()) {
    while (BTSerial.available()) {
      char c = BTSerial.read();
      Serial.println(c);
      data += c;
    }
  }
  return data;
}

void abrirPuerta() {
  // Inicializar finger
  finger.begin(57600);

  detectar_sensor();
  delay(500);
  huellaEncontrada = getFingerprintIDez();
  delay(500);
  if(huellaEncontrada != 255) { //255 es el número de error en unit8_t
    escribirEnLCDFijo("Abriendo puerta. ");
    controlServo();
  }
  else {
    strcpy(mensaje, "Error de huella");
    escribirEnLCDDinamico(mensaje, strlen(mensaje));
  }
}

void controlServo() {
  myServo.write(0); // Abre la puerta
  delay(1000);
  myServo.write(90); // Cierra la puerta
}

uint8_t getFingerprintIDez() {
  strcpy(mensaje, "Introduce la huella. ");
  escribirEnLCDDinamico(mensaje, strlen(mensaje));
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  escribirEnLCDFijo("Huella OK. ");
  return finger.fingerID;
}

void detectar_sensor() {
  if (finger.verifyPassword()) {
    escribirEnLCDFijo("Sensor OK. ");
  } 
  else {
    escribirEnLCDFijo("Sensor no OK. ");
    while (1) { 
      delay(1); 
    }
  }
}

void escribirEnLCDDinamico(char *mensaje, int longitud) { // EL TEXTO SE MUEVE
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < longitud; ++i) {
    String textoDesplazado = String(mensaje).substring(i) + String(mensaje).substring(0, i);
    lcd.print(textoDesplazado);
    delay(200);
    lcd.setCursor(0, 0);
  }
  delay(500);
}

void escribirEnLCDFijo(String message) { // EL TEXTO ESTÁ FIJO
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(2000);
}

uint8_t crearHuella(uint8_t id) { // Es la función "getFingerprintEnroll" del fichero "enroll"
  int p = -1;
  // Inicializar finger
  finger.begin(57600);
  detectar_sensor();
  while (p != FINGERPRINT_OK) {
    strcpy(mensaje, "Pon el dedo.");
    escribirEnLCDFijo(mensaje);
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
    strcpy(mensaje, "Mantenlo...");
    escribirEnLCDFijo(mensaje);
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

  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); 
  Serial.println(id);
  p = -1;
  escribirEnLCDFijo("Retiralo...");

  while (p != FINGERPRINT_OK) {
    strcpy(mensaje, "Pon el dedo.");
    escribirEnLCDFijo(mensaje);
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

  escribirEnLCDFijo("Retiralo...");

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

  Serial.print("Creating model for #");  
  Serial.println(id);

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

  escribirEnLCDFijo("Huella guardada.");

  return true;
}