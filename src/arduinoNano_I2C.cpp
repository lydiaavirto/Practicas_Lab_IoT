#include <Arduino.h>
#include "BBTimer.h"
#include <mbed.h>
#include <Arduino_LSM9DS1.h>

const uint8_t DIRECCION_I2C = 0x08;

// VARIABLES DE LOS TIMERS //
// Se coge solo un timer por ahorrarnos recursos y problemas
BBTimer timerMuestreo(BB_TIMER3);

volatile bool flagLectura = false;
volatile bool flagEscritura = false;
volatile uint8_t nMuestra = 0;

bool capturando = false;

// VARIABLES DEL IMU //
struct lecturaIMU{
  float ax, ay, az; // variables del acelerometro
  float gx, gy, gz; // Variables del giroscopo  
  float mx, my, mz; // Variables del magnetometro
};
// Array para almacenar las 10 muestras de cada segundo
lecturaIMU sensorData[5];

void callbackTimer() {
  flagLectura = true;  // Sensores del Arduino
  nMuestra++;

  if (nMuestra>=5){   // Cada 10 de lectura hay una escritura
    flagEscritura = true;   // Escritura de los sensores 
    nMuestra = 0;
    }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Wire.begin(); // Inicializa I2C como Master (SDA=A4, SCL=A5)

  if (!IMU.begin()) {
    Serial.println("¡Error al inicializar el sensor LSM9DS1!");
    while (1);
  }

  // Configura el timer para dispararse cada 100 ms e inicializarlo
  timerMuestreo.setupTimer(200000, callbackTimer);

  Serial.println("==================================================");
  Serial.println("PLACA 1 Lista.");
  Serial.println("Escribe '1' en el Monitor Serie para capturar 1s de IMU...");
  Serial.println("==================================================");

}

void loop() {

  if (Serial.available() > 0 && !capturando) {
    char c = Serial.read(); // Leemos el primer carácter (p. ej. '1')
    Serial.print("\n>>> Comando recibido: "); 
    Serial.println(c);

    // Vaciamos el resto del buffer para descartar los \r y \n
    while (Serial.available() > 0) Serial.read();

    nMuestra = 0;
    capturando = true;
    timerMuestreo.timerStart();
}

  if(flagLectura){
    flagLectura = false;
    uint8_t posActual = (nMuestra == 0) ? 4 : (nMuestra - 1);

    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(sensorData[posActual].ax, sensorData[posActual].ay, sensorData[posActual].az);
    }
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(sensorData[posActual].gx, sensorData[posActual].gy, sensorData[posActual].gz);
    }
    if (IMU.magneticFieldAvailable()) {
      IMU.readMagneticField(sensorData[posActual].mx, sensorData[posActual].my, sensorData[posActual].mz);
    }
  }

  if(flagEscritura){
    flagEscritura = false;

    timerMuestreo.timerStop();
    capturando = false;
    Wire.beginTransmission(DIRECCION_I2C);
    Wire.write((uint8_t*)sensorData, sizeof(sensorData));
    uint8_t estado = Wire.endTransmission();

    if (estado == 0) {
      Serial.println(">>> Envío I2C exitoso.");
    } else {
      Serial.print(">>> Error I2C: "); Serial.println(estado);
    }
  }
}