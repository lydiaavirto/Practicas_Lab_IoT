#include <Arduino.h>
#include <Wire.h>
#include <Arduino_LSM9DS1.h>

const uint8_t DIRECCION_I2C = 0x08;

struct LecturaAccel {
  float ax, ay, az;
};

// Se mantiene volatile para el uso seguro entre loop() e ISR
volatile LecturaAccel muestraActual;

/**
 * @brief Callback que responde enviando los 12 bytes en cuanto la ESP32 los solicita.
 */
void atenderPeticion() {
  Wire.write((uint8_t*)&muestraActual, sizeof(muestraActual));
}

void setup() {
  Serial.begin(115200);

  if (!IMU.begin()) {
    while (1); // Detener ejecución si no se encuentra la IMU
  }

  // Inicializar como Esclavo I2C en la dirección 0x08
  Wire.begin(DIRECCION_I2C);
  Wire.onRequest(atenderPeticion);
}

void loop() {
  // Mantener actualizada la variable local con la lectura más reciente de la IMU
  if (IMU.accelerationAvailable()) {
    float x, y, z; // Variables temporales normales (no volatile)
    IMU.readAcceleration(x, y, z);

    // Asignación limpia a la estructura volatile
    noInterrupts(); // Desactivar interrupciones durante la copia rápida
    muestraActual.ax = x;
    muestraActual.ay = y;
    muestraActual.az = z;
    interrupts();   // Reactivar interrupciones
  }
  delay(50); // Muestreo continuo rápido
}