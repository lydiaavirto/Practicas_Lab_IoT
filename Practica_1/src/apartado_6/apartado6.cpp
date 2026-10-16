/**
 * @file apartado6.cpp
 * @brief Práctica 1 - Apartado 6: Muestreo periódico de la IMU LSM9DS1 y transmisión por UART.
 * @details Este programa utiliza un temporizador hardware (BBTimer) configurado a 100 ms para 
 *          muestrear los 9 grados de libertad (Acelerómetro, Giroscopio y Magnetómetro) de la IMU.
 *          Almacena 10 muestras en un array de estructuras y envía la ráfaga completa por el puerto 
 *          serie cada 1 segundo (al acumular 10 muestras).
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>
#include "BBTimer.h"
#include <mbed.h>
#include <Arduino_LSM9DS1.h>

/** @brief Instancia del temporizador hardware BB_TIMER3 para el control temporal de 100 ms */
BBTimer timerMuestreo(BB_TIMER3);

/** @brief Volatile flag para indicar que se debe realizar la lectura de los sensores de la IMU */
volatile bool flagLectura = false;

/** @brief Volatile flag para indicar que transcurrió 1 s (10 muestras acumuladas) y se debe transmitir por UART */
volatile bool flagEscritura = false;

/** @brief Contador de muestras acumuladas en el ciclo actual (0 a 9) */
volatile uint8_t nMuestra = 0;

/**
 * @struct lecturaIMU
 * @brief Estructura de datos para almacenar una captura completa de la IMU.
 */
struct lecturaIMU {
  float ax, ay, az; // Componentes del acelerómetro 
  float gx, gy, gz; // Componentes del giroscopio 
  float mx, my, mz; // Componentes del magnetómetro  
};

/** @brief Array para almacenar 10 muestras consecutivas tomadas cada 100 ms */
lecturaIMU sensorData[10];

/**
 * @brief Rutina de servicio de la interrupción (ISR / Callback) del temporizador.
 * @details Se activa cada 100 ms. Marca el flag de lectura y gestiona el contador 
 *          de muestras para activar el flag de transmisión UART cada 10 eventos (1 segundo).
 */
void callbackTimer() {
  flagLectura = true;  
  nMuestra++;

  if (nMuestra >= 10) {   
    flagEscritura = true;   
    nMuestra = 0;
  }
}

/**
 * @brief Configuración inicial del sistema, puerto serie, IMU LSM9DS1 y BBTimer.
 * @details Inicializa la consola a 9600 baudios, comprueba la inicialización de la IMU 
 *          integrada y programa el temporizador hardware para activarse cada 100.000 µs (100 ms).
 */
void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inicializa el sensor IMU integrado (LSM9DS1)
  if (!IMU.begin()) {
    Serial.println("¡Error al inicializar el sensor LSM9DS1!");
    while (1);
  }

  // Configura el timer para dispararse cada 100 ms (100.000 µs)
  timerMuestreo.setupTimer(100000, callbackTimer);
  timerMuestreo.timerStart();

  Serial.println(">>> Sistema iniciado: BBTimer a 100ms, Envío UART a 1s <<<");
}

/**
 * @brief Bucle principal de ejecución.
 * @details Atiende los eventos síncronos generados por las banderas de la ISR:
 *          1. **Lectura (cada 100 ms):** Lee acelerómetro, giroscopio y magnetómetro y los guarda en el array.
 *          2. **Transmisión (cada 1 s):** Imprime en la consola serie el contenido de las 10 muestras con formato claro.
 */
void loop() {
  // 1. TAREA DE MUESTREO (Cada 100 ms)
  if (flagLectura) {
    flagLectura = false;

    // Calcular la posición donde guardar la muestra recién tomada
    uint8_t posActual = (nMuestra == 0) ? 9 : (nMuestra - 1);

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

  // 2. TAREA DE ENVÍO POR UART (Cada 1 segundo / Tras completar 10 muestras)
  if (flagEscritura) {
    flagEscritura = false;

    Serial.println("================ RÁFAGA DE 10 MUESTRAS (1s) ================");
    for (uint8_t i = 0; i < 10; i++) {
      Serial.print("Muestra "); Serial.print(i + 1); Serial.print(":");

      // Imprimir Acelerómetro
      Serial.print(" A -> X: "); Serial.print(sensorData[i].ax, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].ay, 2);
      Serial.print(" | Z: "); Serial.print(sensorData[i].az, 2);

      // Imprimir Giróscopo
      Serial.print(" G -> X: "); Serial.print(sensorData[i].gx, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].gy, 2);
      Serial.print(" | Z: "); Serial.print(sensorData[i].gz, 2);

      // Imprimir Magnetómetro
      Serial.print(" M -> X: "); Serial.print(sensorData[i].mx, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].my, 2);
      Serial.print(" | Z: "); Serial.println(sensorData[i].mz, 2);
    }
    Serial.println("============================================================\n");
  }
}