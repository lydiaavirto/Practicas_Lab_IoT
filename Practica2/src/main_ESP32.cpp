#include <Arduino.h>
#include <Wire.h>

const uint8_t DIRECCION_ESCLAVO = 0x08;
const uint8_t PIN_SDA = 8;
const uint8_t PIN_SCL = 9;
const uint8_t PIN_LED = 10;

// Estructura para 1 sola muestra de aceleración
struct lecturaIMU {
  float ax, ay, az;
};

lecturaIMU sensorData;


/**
 * @brief Tarea de FreeRTOS que solicita cada 1s la aceleración actual al Arduino Nano.
 */
void lecturaI2C(void *parameter) {
  lecturaIMU acel_leida;
  for (;;) {
    // Solicitar directamente al esclavo
    uint8_t bytesLeidos = Wire.requestFrom(DIRECCION_ESCLAVO, sizeof(acel_leida));

    if (bytesLeidos == sizeof(acel_leida)) {
      // Volcar la lectura recibida
      Wire.readBytes((uint8_t*)&acel_leida, sizeof(acel_leida));
    }

    sensorData = acel_leida;
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void envioUART_LED( void * parameter){
  TickType_t xLastWakeTime = xTaskGetTickCount(); // instante inicial en que empieza la tarea, se actualiza solo al empezar
  const TickType_t xPeriodo = pdMS_TO_TICKS(1000); // periodo absoluto de la tarea - 1 s- 

  for(;;){

    digitalWrite(PIN_LED, HIGH);

    Serial.println("\n================ RÁFAGA RECIBIDA POR I2C ================");
    Serial.print(" Accel [g]  -> X: "); Serial.print(sensorData.ax, 2);
    Serial.print(" | Y: "); Serial.print(sensorData.ay, 2);
    Serial.print(" | Z: "); Serial.print(sensorData.az, 2);

    vTaskDelay(pdMS_TO_TICKS(200));

    digitalWrite(PIN_LED, LOW);

    vTaskDelayUntil(&xLastWakeTime, xPeriodo); // Hace un delay teniendo en cuenta cuando ha 
    
                                                //empezado la tarea y hasta cuando tiene que durar
    
    
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL, 100000); // Configurado como Maestro I2C

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Crear la tarea para la lectura por I2C
  xTaskCreate(lecturaI2C, "TareaAccel", 4096, NULL, 2, NULL);
  xTaskCreate(envioUART_LED,"TareaLED", 4096, NULL, 1, NULL);
}

void loop() {}