// Librerías
#include <Servo.h> // Librería servo
#include <LiquidCrystal.h> // Librería lcd (pantalla)
#include <SoftwareSerial.h> 
#include <Adafruit_Fingerprint.h> // Librería de la huella dactilar

Servo myServo;
LiquidCrystal lcd(9, 8, 7, 6, 5, 4, 2);
SoftwareSerial BTSerial(2,3); // Serial para el Bluetooth
SoftwareSerial FingerSerial(11,13); // Serial para el sensor de huella
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
  lcd.print("App conectada!"); // Lo que se escribe en la pantalla
}

// Función para abrir y cerra la puerta
void controlServo() {
  myServo.write(70);
  delay(1000);
  myServo.write(180);
  delay(1000);
}

void setup_servo() {
  myServo.attach(10); // Servo -> pin 10
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
  Serial.print("Esperando la huella para registrarla en #"); 
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Huella detectada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Huella no detectada");
      break;
    default:
      Serial.println("Error desconocido");
      break;
    }
  }

  //si se detecta correctamente la huella
  p = finger.image2Tz(1); //image2Tz() convierte la imagen de la huella digital en otro formato
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Escaneo correcto");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Escaneo incorrecto");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se puede escanear esta huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se puede escanear esta huella");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }
  
  Serial.println("Quita el dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
  }
  Serial.print("ID "); 
  Serial.println(id);
  p = -1;
  Serial.println("Vuelve a poner el dedo");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Escaneo correcto");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Escaneo incorrecto");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se puede escanear esta huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se puede escanear esta huella");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
    }
  }

  //si se detecta correctamente la huella
  p = finger.image2Tz(2); //image2Tz() convierte la imagen de la huella digital en otro formato
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Escaneo correcto");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Escaneo incorrecto");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se puede escanear esta huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se puede escanear esta huella");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }
  
  //si se escanea correctamente la huella
  Serial.print("Creando modelo para #");  
  Serial.println(id);
  
  p = finger.createModel(); //createModel() crea el modelo de la huella
  if (p == FINGERPRINT_OK) {
    Serial.println("Modelo creado correctamente");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Modelo NO creado correctamente");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }   
  
  Serial.print("ID "); 
  Serial.println(id);
  p = finger.storeModel(id); //storeModel() almacena el modelo de huella digital 
  if (p == FINGERPRINT_OK) {
    Serial.println("Huella guardada");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("No se puede guardar la huella en esa localización");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error al escribir en la memoria flash");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  } 
}