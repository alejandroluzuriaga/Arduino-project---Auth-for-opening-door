//EJEMPLO ESCANEAR HUELLA DACTILAR (VÁLIDA O NO)
#include <Adafruit_Fingerprint.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(9600);
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

void loop() {
  funcionAuxiliar();
  delay(50);           
}

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

  //Si se detecta correctamente la huella
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
  
  //Si se escanea correctamente la huella
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
  
  //La huella es correcta
  Serial.print("Encontrada la huella #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" con coincidencia "); 
  Serial.println(finger.confidence); 

  return finger.fingerID; //fingerID devuelve el ID de la huella encontrada
}

int funcionAuxiliar() { //Devuelve -1 si falla
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  //No falla
  Serial.print("Encontrada la huella #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" con coincidencia "); 
  Serial.println(finger.confidence); //confidence nos dice el nivel de confianza, cuanto mayor sea el valor, mayor será la coincidencia
  return finger.fingerID; 
}