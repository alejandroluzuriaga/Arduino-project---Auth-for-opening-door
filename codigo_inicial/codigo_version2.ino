// Librerías
#include <Servo.h> // Librería servo
#include <LiquidCrystal.h> // Librería lcd (pantalla)
#include <SoftwareSerial.h> 
#include <Adafruit_Fingerprint.h> // Librería de la huella dactilar

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
  lcd.print("App conectada!"); // Lo que se escribe en la pantalla
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
  myServo.attach(9); // Servo -> pin 9
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
  loop_tratarSenalApp(bluetoothData);
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
void loop_tratarSenalApp(String bluetoothData) {
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
  // Escaneo 1
  int p = -1;
  int resultado = escaneo(p);
  if (resultado != 0) {
    return;
  }

  escribirEnLCD("Quita el dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
  }
  escribirEnLCD("ID "); 
  escribirEnLCD(id);

  // Escaneo 2
  p = -1;
  resultado = escaneo(p);
  if (resultado != 0) {
    return;
  }

  escribirEnLCD("Creando modelo para #");  
  escribirEnLCD(id);

  p = finger.createModel(); //createModel() crea el modelo de la huella
  resultado = crear_modelo_huella(p);
  if (resultado != 0) {
    return;
  }

  escribirEnLCD("ID "); 
  escribirEnLCD(id);
  p = finger.storeModel(id); //storeModel() almacena el modelo de huella digital 
  resultado = almacenar_huella(p);
  if (resultado != 0) {
    return;
  }
}

int escaneo(int p) {
  escribirEnLCD("Introduce la huella"); 
  detectar_huella(p);

  p = finger.image2Tz(1); //image2Tz() convierte la imagen de la huella digital en otro formato
  return escanear_huella(p);
}

void detectar_huella(int p) {
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
}

int escanear_huella(int p) {
  switch (p) {
    case FINGERPRINT_OK:
      escribirEnLCD("Escaneo correcto");
      return 0;
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

int crear_modelo_huella(int p) {
  if (p == FINGERPRINT_OK) {
    escribirEnLCD("Modelo creado correctamente");
    return 0;
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
}

int almacenar_huella(int p) {
  if (p == FINGERPRINT_OK) {
    escribirEnLCD("Huella guardada");
    return 0;
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

//NO SE SI EN LOOP_AUTENTICARSE SE NECESITA LA FUNCIÓN "obtener_id_huella". NO SE SI ESTAS DOS FUNCIONES DE ABAJO SON NECESARIAS O NO.
/*uint8_t obtener_id_huella() {
  uint8_t p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
  detectar_huella(p);

  p = finger.image2Tz(); //image2Tz() convierte la imagen de la huella digital en otro formato
  escanear_huella(p);
  
  p = finger.fingerFastSearch(); //fingerFastSearch() busca la huella en la base de datos del sensor.
  int resultado = busqueda_huella(p); 
  if (resultado != 0) {
    return;
  }
  
  escribirEnLCD("Encontrada la huella #"); 
  escribirEnLCD(finger.fingerID); 
  escribirEnLCD(" con coincidencia "); 
  escribirEnLCD(finger.confidence); 

  return finger.fingerID; //fingerID devuelve el ID de la huella encontrada
}

int busqueda_huella(int p) {
  if (p == FINGERPRINT_OK) {
    escribirEnLCD("Se ha encontrado la huella en la BD");
    return 0;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    escribirEnLCD("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    escribirEnLCD("NO se ha encontrado la huella en la BD");
    return p;
  } else {
    escribirEnLCD("Error desconocido");
    return p;
  }   
}*/