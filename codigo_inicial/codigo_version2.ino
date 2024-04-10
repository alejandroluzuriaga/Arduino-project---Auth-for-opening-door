// Librerías
#include <Servo.h> // Librería servo
#include <LiquidCrystal.h> // Librería lcd (pantalla)
#include <SoftwareSerial.h>  //Librería Bluetooth
#include <Adafruit_Fingerprint.h> // Librería de la huella dactilar

// Inicialización Bluetooth
SoftwareSerial BTSerial(2, 3);

// Inicialización LCD
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Inicialización Servo
Servo myServo;

// Inicialización Sensor de Huella dactilar
SoftwareSerial FingerSerial(13,11); // Serial para el sensor de huella
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FingerSerial);
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
  /* DE OLEK
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
  */
}

void setup_bluetooth() {
  BTSerial.begin(9600);
}

// Función para escribir en la pantalla LCD
void escribirEnLCD(String message) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(message);
}

void setup_lcd() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  lcd.cursor();
  lcd.print("App conectada!"); // Lo que se escribe en la pantalla
}

// Función para abrir y cerrar la puerta
void controlServo() {
  myServo.write(0);
  delay(500);
  myServo.write(90);
}

void setup_servo() {
  myServo.attach(A0); // Servo -> pin A0
}

void setup() {
  Serial.begin(9600);
  setup_bluetooth();
  setup_lcd();
  setup_servo();
  bluetoothData = leerDatosBluetooth();
  setup_tratarSenalApp(bluetoothData);
}

void loop() {// Leer del bluetooth todo el rato, escribir por LCD un mensaje y mover el servo
  bluetoothData = leerDatosBluetooth();
  escribirEnLCD("Nuevo mensaje");
  controlServo();
  tratarSenalApp(bluetoothData);
}

// Función para tratar la señal que viene del botón que se pulse en la app
void setup_tratarSenalApp(String bluetoothData) {
  if (bluetoothData.equals("1")) { // Crear huella
    while (!Serial);
    delay(100);
    setup_crear_huella();
  }
  else if (bluetoothData.equals("2")) { // Autenticarse
    while (!Serial);
    delay(100);
    setup_autenticarse();
  }
  else if (bluetoothData.equals("3")) { // Cancelar
  }
}

void detectar_sensor() {
  if (finger.verifyPassword()) { //verifyPassword() devuelve true si se detecta el sensor y está listo para usarse, sino devuelve false
    escribirEnLCD("El sensor de huella dactilar se encuentra");
  } 
  else {
    escribirEnLCD("El sensor de huella dactilar NO se encuentra");
    while (1) { 
      delay(1); 
    }
  }
}

void setup_crear_huella() {
  escribirEnLCD("\n\nEmpezamos con el registro de huellas dactilares");
  finger.begin(57600);
  
  detectar_sensor();
}

void setup_autenticarse() {
  escribirEnLCD("\n\n Empezamos a escanear la huella dactilar");
  finger.begin(57600);
  
  detectar_sensor();

  finger.getTemplateCount(); //getTemplateCount() devuelve el número de huellas dactilares almacenadas
  escribirEnLCD("El sensor tiene ");
  escribirEnLCD(finger.templateCount);
  escribirEnLCD(" huellas dactilares almacenadas");
  escribirEnLCD("Esperando una huella válida...");
}

// Función para tratar la señal que viene del botón que se pulse en la app
void tratarSenalApp(String bluetoothData) {
  if (bluetoothData.equals("1")) { // Crear huella
    loop_crear_huella();
  }
  else if (bluetoothData.equals("2")) { // Autenticarse
    loop_autenticarse();
  }
  else if (bluetoothData.equals("3")) { // Cancelar
  }
}

void loop_crear_huella() {
  escribirEnLCD("Listo para registrar la huella");
  escribirEnLCD("Introduce el ID # (desde 1 al 127) donde quieras guardar esta huella...");

  id = lecturaID();
  if (id == 0) { // No se puede registrar ninguna huella en el ID=0
    return;
  }
  escribirEnLCD("Registrando ID #");
  escribirEnLCD(id);
  
  while (!obtener_registro_huella());
} 

int loop_autenticarse() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  escribirEnLCD("Encontrada la huella #"); 
  escribirEnLCD(finger.fingerID); 
  escribirEnLCD(" con coincidencia "); 
  escribirEnLCD(finger.confidence); //confidence nos dice el nivel de confianza, cuanto mayor sea el valor, mayor será la coincidencia
  
  delay(50); 
}

// Función para leer el ID que se introduce por pantalla
uint8_t lecturaID(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t obtener_registro_huella() {
  int p = -1;
  escribirEnLCD("Esperando la huella para registrarla en #"); 
  escribirEnLCD(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
    switch (p) {
    case FINGERPRINT_OK:
      escribirEnLCD("Huella detectada");
      break;
    case FINGERPRINT_NOFINGER:
      escribirEnLCD(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      escribirEnLCD("Error de comunicación");
      break;
    case FINGERPRINT_IMAGEFAIL:
      escribirEnLCD("Huella no detectada");
      break;
    default:
      escribirEnLCD("Error desconocido");
      break;
    }
  }

  //si se detecta correctamente la huella
  p = finger.image2Tz(1); //image2Tz() convierte la imagen de la huella digital en otro formato
  switch (p) {
    case FINGERPRINT_OK:
      escribirEnLCD("Escaneo correcto");
      break;
    case FINGERPRINT_IMAGEMESS:
      escribirEnLCD("Escaneo incorrecto");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      escribirEnLCD("Error de comunicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      escribirEnLCD("No se puede escanear esta huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      escribirEnLCD("No se puede escanear esta huella");
      return p;
    default:
      escribirEnLCD("Error desconocido");
      return p;
  }
  
  escribirEnLCD("Quita el dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
  }
  escribirEnLCD("ID "); 
  escribirEnLCD(id);
  p = -1;
  escribirEnLCD("Vuelve a poner el dedo");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
    switch (p) {
    case FINGERPRINT_OK:
      escribirEnLCD("Escaneo correcto");
      break;
    case FINGERPRINT_IMAGEMESS:
      escribirEnLCD("Escaneo incorrecto");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      escribirEnLCD("Error de comunicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      escribirEnLCD("No se puede escanear esta huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      escribirEnLCD("No se puede escanear esta huella");
      return p;
    default:
      escribirEnLCD("Error desconocido");
      return p;
    }
  }

  //si se detecta correctamente la huella
  p = finger.image2Tz(2); //image2Tz() convierte la imagen de la huella digital en otro formato
  switch (p) {
    case FINGERPRINT_OK:
      escribirEnLCD("Escaneo correcto");
      break;
    case FINGERPRINT_IMAGEMESS:
      escribirEnLCD("Escaneo incorrecto");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      escribirEnLCD("Error de comunicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      escribirEnLCD("No se puede escanear esta huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      escribirEnLCD("No se puede escanear esta huella");
      return p;
    default:
      escribirEnLCD("Error desconocido");
      return p;
  }
  
  //si se escanea correctamente la huella
  escribirEnLCD("Creando modelo para #");  
  escribirEnLCD(id);
  
  p = finger.createModel(); //createModel() crea el modelo de la huella
  if (p == FINGERPRINT_OK) {
    escribirEnLCD("Modelo creado correctamente");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    escribirEnLCD("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    escribirEnLCD("Modelo NO creado correctamente");
    return p;
  } else {
    escribirEnLCD("Error desconocido");
    return p;
  }   
  
  escribirEnLCD("ID "); 
  escribirEnLCD(id);
  p = finger.storeModel(id); //storeModel() almacena el modelo de huella digital 
  if (p == FINGERPRINT_OK) {
    escribirEnLCD("Huella guardada");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    escribirEnLCD("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    escribirEnLCD("No se puede guardar la huella en esa localización");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    escribirEnLCD("Error al escribir en la memoria flash");
    return p;
  } else {
    escribirEnLCD("Error desconocido");
    return p;
  } 
}