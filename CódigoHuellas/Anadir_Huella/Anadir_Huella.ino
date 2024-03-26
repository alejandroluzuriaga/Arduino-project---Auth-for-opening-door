//EJEMPLO AÑADIR HUELLA DACTILAR A LA MEMORIA DEL MÓDULO
#include <Adafruit_Fingerprint.h> 
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

void setup() {
  Serial.begin(9600);
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

uint8_t lecturaID(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop() {       
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

  //Si se detecta correctamente la huella
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

  //Si se detecta correctamente la huella
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
  
  //Si se escanea correctamente la huella
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