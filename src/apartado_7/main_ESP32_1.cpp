/**
 * @file esp32_I2C.cpp
 * @brief Práctica 1 - Apartado 7: PLACA 2 Esclavo I2C (ESP32-S3).
 * @details Configura la ESP32-S3 como esclavo I2C en la dirección 0x08.
 *          Recibe de forma asíncrona por interrupción ('onReceive') fragmentos de datos 
 *          I2C enviados por el Maestro hasta acumular el bloque completo de 180 bytes 
 *          (5 muestras de la IMU x 36 bytes). Al completarse la ráfaga, despliega los datos 
 *          organizados por la consola serie y activa un LED indicador durante 1 segundo.
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>
#include <Wire.h>

/** @brief Dirección I2C asignada a la ESP32-S3 en el bus */
const uint8_t DIRECCION_I2C = 0x08;

/** @brief Pin GPIO asignado a la línea SDA del bus I2C */
const uint8_t PIN_SDA = 8;

/** @brief Pin GPIO asignado a la línea SCL del bus I2C */
const uint8_t PIN_SCL = 9;

/** @brief Pin GPIO conectado al LED indicador de recepción de datos */
const uint8_t PIN_LED = 10;

/**
 * @struct lecturaIMU
 * @brief Estructura de datos idéntica a la del Maestro para desempaquetar la trama de la IMU (36 bytes).
 */
struct lecturaIMU {
  float ax, ay, az; // Acelerómetro
  float gx, gy, gz; // Giroscopio 
  float mx, my, mz; // Magnetómetro
};

/** @brief Array de recepción para alojar las 5 muestras del bloque I2C (5 x 36 bytes = 180 bytes) */
lecturaIMU sensorData[5];

/** @brief Búfer interno para acumular los fragmentos I2C entrantes */
uint8_t bufferRecepcion[sizeof(sensorData)];

/** @brief Contador de bytes acumulados en la ráfaga actual */
size_t bytesAcumulados = 0;

/** @brief Volatile flag que indica la recepción correcta de la ráfaga completa por I2C */
volatile bool flagRecibido = false;

/**
 * @brief Rutina de servicio de la interrupción (ISR) de recepción I2C.
 * @details Se activa automáticamente al recibir cada bloque de bytes por I2C.
 *          Lee los datos entrantes y los va acumulando secuencialmente en 'bufferRecepcion'.
 *          Cuando acumula los 180 bytes esperados, vuelca los datos a 'sensorData'
 *          y activa la bandera 'flagRecibido'.
 * @param bytesRecibidos Número de bytes transferidos en la transacción actual.
 */
void recibirDatosI2C(int bytesRecibidos) {
  while (Wire.available()) {
    if (bytesAcumulados < sizeof(bufferRecepcion)) {
      bufferRecepcion[bytesAcumulados++] = (uint8_t)Wire.read();
    } else {
      Wire.read(); // Descartar bytes sobrantes para prevenir desbordamientos
    }
  }

  // Verificar si ya hemos completado la ráfaga de 180 bytes (5 muestras)
  if (bytesAcumulados >= sizeof(sensorData)) {
    memcpy((void*)sensorData, bufferRecepcion, sizeof(sensorData));
    bytesAcumulados = 0; // Reiniciar el contador para la próxima ráfaga
    flagRecibido = true;
  }
}

/**
 * @brief Configuración inicial del hardware, puerto serie, pines de comunicación e interfaz esclava I2C.
 * @details Inicializa la consola serie a 115200 baudios, configura el pin del LED como salida,
 *          inicia la librería Wire en modo esclavo (SDA=GPIO8, SCL=GPIO9, 100 kHz) y registra la ISR 'recibirDatosI2C'.
 */
void setup() {
  Serial.begin(115200);

  // Configuración del LED indicador
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Inicializar bus I2C en modo Esclavo (dirección 0x08, SDA=8, SCL=9, 100 kHz)
  Wire.begin(DIRECCION_I2C, PIN_SDA, PIN_SCL, 100000);
  Wire.onReceive(recibirDatosI2C);

  Serial.println("==================================================");
  Serial.println("PLACA 2 (ESP32-S3 Esclavo) Lista.");
  Serial.println("Esperando ráfaga de datos I2C desde el Maestro...");
  Serial.println("==================================================");
}

/**
 * @brief Bucle principal de ejecución de la PLACA 2.
 * @details Al activarse la bandera 'flagRecibido':
 *          1. Enciende el LED indicador.
 *          2. Formatea e imprime por consola las 5 muestras de la IMU (Acelerómetro, Giroscopio y Magnetómetro).
 *          3. Mantiene el LED encendido exactamente durante 1000 ms antes de apagarlo.
 */
void loop() {
  if (flagRecibido) {
    flagRecibido = false;

    // 1. Encender el LED indicador
    digitalWrite(PIN_LED, HIGH);
    unsigned long tiempoInicioLED = millis();

    // 2. Desplegar los datos de las 5 muestras formateadas por el Monitor Serie
    Serial.println("\n================ RÁFAGA RECIBIDA POR I2C ================");
    for (uint8_t i = 0; i < 5; i++) {
      Serial.print("Muestra #"); Serial.print(i + 1); Serial.print("]:");

      // Acelerómetro
      Serial.print(" Accel [g]  -> X: "); Serial.print(sensorData[i].ax, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].ay, 2);
      Serial.print(" | Z: "); Serial.print(sensorData[i].az, 2);

      // Giróscopo
      Serial.print(" Gyro  [dps] -> X: "); Serial.print(sensorData[i].gx, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].gy, 2);
      Serial.print(" | Z: "); Serial.print(sensorData[i].gz, 2);

      // Magnetómetro
      Serial.print(" Mag   [uT]  -> X: "); Serial.print(sensorData[i].mx, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].my, 2);
      Serial.print(" | Z: "); Serial.println(sensorData[i].mz, 2);
    }
    Serial.println("========================================================\n");

    // Mantener el LED encendido exactamente 1 segundo (1000 ms) sin bloquear interrupciones
    while (millis() - tiempoInicioLED < 1000);
    digitalWrite(PIN_LED, LOW); // Apagar LED
  }
}