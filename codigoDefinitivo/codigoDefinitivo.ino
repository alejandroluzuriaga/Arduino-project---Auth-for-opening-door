// Librerías
#include <Servo.h> // Librería servo
#include <LiquidCrystal.h> // Librería lcd (pantalla)
#include <SoftwareSerial.h>  // Librería Bluetooth
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

int appData = 0;
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
  
  escribirPuntos();
}

void loop() {
  if(BTSerial.available()) {
    appData = leerDatosApp();
    Serial.println(appData);
    int numString = 0;
  
    if(appData == 1) {
      while(!BTSerial.available());
      String numString = "0";
      numString = leerID();
      delay(500);
      if(numString != "0") {
        int numInt = numString.toInt();
        uint8_t id = (uint8_t) numInt;
        Serial.print("El id de la app es: "); 
        Serial.println(id);
        while (id != 0 && !crearHuella(id));
        BTSerial.begin(9600);
        lcd.clear();
        escribirPuntos();
      }
    }
    else if(appData == 2) {
      abrirPuerta();
      BTSerial.begin(9600);
      lcd.clear();
      escribirPuntos();
    }
  }
  delay(100);
}

void escribirPuntos() {
  int contadorPuntos = 0;
  while (!BTSerial.available()) {
    lcd.clear(); // Limpia la pantalla
    lcd.print("Esperando app"); // Escribe el mensaje

    // Bucle para escribir los puntos
    for (int i = 0; i < contadorPuntos; i++) {
      lcd.print(".");
    }

    contadorPuntos = (contadorPuntos + 1) % 4; // Incrementa y reinicia el contador de puntos
    delay(500); // Espera medio segundo
  }
}

int leerDatosApp() {
  return BTSerial.read() - '0';
}

String leerID() {
  String data = "";
  while (BTSerial.available()) {
    delay(500);
    char c = BTSerial.read();
    data += c;
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
    escribirEnLCDFijo("Error de huella.");
  }
}

void controlServo() {
  myServo.write(0); // Abre la puerta
  delay(10000);
  myServo.write(90); // Cierra la puerta
}

uint8_t getFingerprintIDez() {
  escribirEnLCDFijo("Pon el dedo.");
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

void escribirEnLCDFijo(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(2000);
}

uint8_t crearHuella(uint8_t id) {
  int p = -1;
  // Inicializar finger
  finger.begin(57600);

  detectar_sensor();
  escribirEnLCDFijo("Pon el dedo.");
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

  escribirEnLCDFijo("Retiralo...");
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;

  escribirEnLCDFijo("Pon el dedo.");
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
  escribirEnLCDFijo("Mantenlo...");

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

  escribirEnLCDFijo("Retiralo...");
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
  escribirEnLCDFijo("Huella guardada.");
  return true;
}