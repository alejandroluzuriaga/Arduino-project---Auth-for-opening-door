### Sistema de control de acceso mediante huella dactilar y Bluetooth

Este proyecto implementa un sistema de control de acceso a una puerta utilizando huella dactilar y comunicación Bluetooth. El sistema permite:

-   Crear huellas dactilares: Se pueden registrar nuevas huellas dactilares en el sistema.
-   Abrir la puerta: La puerta se abre automáticamente cuando se detecta una huella dactilar válida.
-   Comunicación Bluetooth: El sistema se comunica con un dispositivo móvil a través de Bluetooth para recibir comandos para crear huellas dactilares o abrir la puerta.

### Librerías requeridas

-   Librería Servo
-   Librería LiquidCrystal
-   Librería SoftwareSerial
-   Librería Adafruit_Fingerprint

### Instalación

1.  Clonar el repositorio del proyecto.
2.  Instalar las librerías requeridas:
    -   Servo: <https://www.arduino.cc/en/Reference/Servo>
    -   LiquidCrystal: <https://www.arduino.cc/reference/en/libraries/liquidcrystal/>
    -   SoftwareSerial: <https://www.arduino.cc/en/Reference/softwareSerial>
    -   Adafruit_Fingerprint: <https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library>
3.  Conectar el hardware según el esquema de conexiones.
4.  Subir el código a la placa Arduino.

### Funcionamiento

El sistema funciona de la siguiente manera:

1.  Encendido: Al encender el sistema, la pantalla LCD muestra un mensaje de espera.
2.  Conexión Bluetooth: El sistema espera a que un dispositivo móvil se conecte a través de Bluetooth usando la aplicación.
3.  Recepción de comandos: Una vez conectado el dispositivo móvil, el sistema recibe comandos a través de Bluetooth. Los comandos disponibles son:
    -   `1`: Crear una nueva huella dactilar.
    -   `2`: Abrir la puerta.
    -   `3`: Indicar que el dispositivo móvil está conectado.
4.  Creación de huellas dactilares: Si se recibe el comando `1`, el sistema solicita al usuario que coloque su dedo en el sensor de huellas dactilares. Una vez que se ha capturado la huella dactilar, el sistema la almacena en la memoria.
5.  Apertura de la puerta: Si se recibe el comando `2` y se detecta una huella dactilar válida, el sistema activa el servo motor para abrir la puerta.
6.  Comunicación bidireccional con el dispositivo móvil: El sistema espera señales de la aplicación móvil para indicar el estado del sistema y esta gestiona dichas señales para mostrar las pantallas requeridas para dicho estado.

### Consideraciones adicionales

-   Se puede agregar una batería (9V) para permitir que el sistema funcione de forma autónoma.
