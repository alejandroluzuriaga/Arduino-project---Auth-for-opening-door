//librerías
#include <Servo.h> //librería servo
#include <LiquidCrystal.h> //librería lcd (pantalla)
#include <SoftwareSerial.h> 
#include <Adafruit_Fingerprint.h> //librería de la huella dactilar

//inicialización de todos los componentes
Servo myServo;
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);
SoftwareSerial BTSerial(2,3); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&BTSerial);
uint8_t id;

void setup() {
  Serial.begin(9600);
  setup_servo();
  setup_lcd();
  setup_bluetooth();
  //si quiero añadir una huella a la memoria del módulo ->
  setup_anadir_huella();
  //si quiero ver si una huella es valida o no ->
  setup_huella_valida();
}

void loop() {
  loop_servo();
  loop_bluetooth();
  //si quiero añadir una huella a la memoria del módulo ->
  loop_anadir_huella();
  //si quiero ver si una huella es valida o no ->
  loop_huella_valida();
}

void setup_servo() {
  myServo.attach(9); //servo -> pin 9
}

void loop_servo() {
  myServo.write(0); //grados servo
  delay(1000);
  myServo.write(90); //grados servo
  delay(1000);
  myServo.write(180); //grados servo
  delay(1000);
}

void setup_lcd() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  lcd.cursor();
  lcd.print("Hello World!"); //lo que se escribe en la pantalla
}

void setup_bluetooth() {
  BTSerial.begin(9600);
}

void loop_bluetooth() {
  if(BTSerial.available()) { //comprueba si se ha escrito algo en el bluetooth
    Serial.write(BTSerial.read());
  }

  if(Serial.available()) { //comprueba si se ha escrito algo del Serial Port del ordenador
    BTSerial.write(Serial.read());
  }
}

void setup_anadir_huella() {
  while (!Serial);
  delay(100);
  Serial.println("\n\nEmpezamos con el registro de huellas dactilares");

  finger.begin(57600);
  
  if (finger.verifyPassword()) { //verifyPassword() devuelve true si se detecta el sensor y está listo para usarse, sino devuelve false
    Serial.println("El sensor de huella dactilar se encuentra");
  } 
  else {
    Serial.println("El sensor de huella dactilar NO se encuentra");
    while (1) { 
      delay(1); 
    }
  }
}

void loop_anadir_huella() {
  Serial.println("Listo para registrar la huella");
  Serial.println("Introduce el ID # (desde 1 al 127) donde quieras guardar esta huella...");
  id = lecturaID();
  if (id == 0) { //No se puede registrar ninguna huella en el ID=0
     return;
  }
  Serial.print("Registrando ID #");
  Serial.println(id);
  
  while (!obtenerRegistroDeHuellaDigital());
}

//--------------------------------funciones auxiliares de añadir huella
uint8_t lecturaID(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t obtenerRegistroDeHuellaDigital() {
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

void setup_huella_valida() {
  while (!Serial);
  delay(100);
  Serial.println("\n\n Empezamos a escanear la huella dactilar");

  finger.begin(57600);
  
  if (finger.verifyPassword()) { //verifyPassword() devuelve true si se detecta el sensor y está listo para usarse, sino devuelve false
    Serial.println("El sensor de huella dactilar se encuentra");
  } 
  else {
    Serial.println("El sensor de huella dactilar NO se encuentra");
    while (1) { 
      delay(1); 
    }
  }

  finger.getTemplateCount(); //getTemplateCount() devuelve el número de huellas dactilares almacenadas
  Serial.print("El sensor tiene "); 
  Serial.print(finger.templateCount); 
  Serial.println(" huellas dactilares almacenadas");
  Serial.println("Esperando una huella válida...");
}

void loop_huella_valida() {
  funcionAuxiliar();
  delay(50); 
}

//--------------------------------funciones auxiliares de huella valida o no
uint8_t obtenerIDHuellaDigital() {
  uint8_t p = finger.getImage(); //getImage() intenta capturar una imagen de la huella digital
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Sí se detecta la huella");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No se detecta la huella");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error de escaneo");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }

  //si se detecta correctamente la huella
  p = finger.image2Tz(); //image2Tz() convierte la imagen de la huella digital en otro formato
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
  p = finger.fingerFastSearch(); //fingerFastSearch() busca la huella en la base de datos del sensor.
  if (p == FINGERPRINT_OK) {
    Serial.println("Se ha encontrado la huella en la BD");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("NO se ha encontrado la huella en la BD");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }   
  
  //la huella es correcta
  Serial.print("Encontrada la huella #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" con coincidencia "); 
  Serial.println(finger.confidence); 

  return finger.fingerID; //fingerID devuelve el ID de la huella encontrada
}

int funcionAuxiliar() { //devuelve -1 si falla
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  //no falla
  Serial.print("Encontrada la huella #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" con coincidencia "); 
  Serial.println(finger.confidence); //confidence nos dice el nivel de confianza, cuanto mayor sea el valor, mayor será la coincidencia
  return finger.fingerID; 
}