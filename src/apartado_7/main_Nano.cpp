/**
 * @file arduinoNano_I2C.cpp
 * @brief Práctica 1 - Apartado 7: PLACA 1 Maestro I2C (Arduino Nano 33 BLE).
 * @details Implementa la captura a demanda de la IMU LSM9DS1 (5 muestras tomadas cada 200 ms durante 1 segundo).
 *          Una vez acumuladas las muestras, transmite el bloque de datos completo de 180 bytes por I2C
 *          a la PLACA 2 (ESP32-S3 con dirección 0x08).
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>
#include "BBTimer.h"
#include <mbed.h>
#include <Arduino_LSM9DS1.h>

/** @brief Dirección I2C del dispositivo esclavo (ESP32-S3) */
const uint8_t DIRECCION_I2C = 0x08;

/** @brief Instancia del temporizador hardware BB_TIMER3 para el control del muestreo a 200 ms */
BBTimer timerMuestreo(BB_TIMER3);

/** @brief Volatile flag que indica el momento de leer la IMU */
volatile bool flagLectura = false;

/** @brief Volatile flag que indica el fin de la ráfaga de 1 segundo (5 muestras) para enviar por I2C */
volatile bool flagEscritura = false;

/** @brief Contador de muestras capturadas en la ráfaga actual (0 a 4) */
volatile uint8_t nMuestra = 0;

/** @brief Estado de la captura a demanda (true = capturando datos) */
bool capturando = false;

/**
 * @struct lecturaIMU
 * @brief Estructura de datos para almacenar una captura completa de la IMU de 9 DoF (36 bytes)[cite: 1].
 */
struct lecturaIMU {
  float ax, ay, az; //  Acelerómetro  
  float gx, gy, gz; //  Giroscopio  
  float mx, my, mz; //  Magnetómetro   
};

/** @brief Array para almacenar 5 muestras completas (5 x 36 bytes = 180 bytes en total) */
lecturaIMU sensorData[5];

/**
 * @brief Rutina de servicio de la interrupción (Callback) del temporizador.
 * @details Se activa cada 200 ms durante la captura. Incrementa el contador de muestras 
 *          y activa la bandera de transmisión al alcanzar 5 muestras (1 segundo).
 */
void callbackTimer() {
  flagLectura = true;  
  nMuestra++;

  if (nMuestra >= 5) {   
    flagEscritura = true;   
    nMuestra = 0;
  }
}

/**
 * @brief Configuración inicial del sistema, puerto serie, I2C Maestro e IMU.
 * @details Inicializa la consola serie a 115200 baudios, arranca la interfaz I2C Maestro (Wire),
 *          comprueba la conexión con la IMU LSM9DS1 y programa el temporizador a 200.000 µs (200 ms).
 */
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Wire.begin(); // Inicializa I2C como Master (SDA=A4, SCL=A5)

  if (!IMU.begin()) {
    Serial.println("¡Error al inicializar el sensor LSM9DS1!");
    while (1);
  }

  // Configura el timer para dispararse cada 200 ms (200.000 µs)
  timerMuestreo.setupTimer(200000, callbackTimer);

  Serial.println("==================================================");
  Serial.println("PLACA 1 Lista.");
  Serial.println("Escribe '1' en el Monitor Serie para capturar 1s de IMU...");
  Serial.println("==================================================");
}

/**
 * @brief Bucle principal de ejecución de la PLACA 1.
 * @details Atiende las órdenes del usuario por la UART para iniciar la captura a demanda,
 *          gestiona las lecturas de los 9 sensores cada 200 ms y envía por I2C la estructura 
 *          binaria completa de 180 bytes a la PLACA 2 tras completar el segundo.
 */
void loop() {
  // 1. Recepción de comando del usuario por UART para iniciar la captura a demanda
  if (Serial.available() > 0 && !capturando) {
    char c = Serial.read(); // Leemos el primer carácter (p. ej. '1')
    Serial.print("\n>>> Comando recibido: "); 
    Serial.println(c);

    // Vaciamos el resto del buffer de la UART para descartar \r y \n
    while (Serial.available() > 0) Serial.read();

    nMuestra = 0;
    capturando = true;
    timerMuestreo.timerStart();
  }

  // 2. Tarea de muestreo de la IMU (Cada 200 ms)
  if (flagLectura) {
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

  // 3. Tarea de transmisión I2C (Al completar 1s / 5 muestras)
  if (flagEscritura) {
    flagEscritura = false;

    timerMuestreo.timerStop();
    capturando = false;

    // Transmisión del bloque binario completo (180 bytes) a la PLACA 2 (0x08)
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