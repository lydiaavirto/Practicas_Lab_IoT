#include <Arduino.h>
#include <Wire.h>

<<<<<<< HEAD
const uint8_t DIRECCION_I2C = 0x08;
const uint8_t PIN_SDA = 8;
const uint8_t PIN_SCL = 9;
const uint8_t PIN_LED = 10;

// ESTRUCTURA IDENTICA A LA DEL MAESTRO (36 bytes)
struct lecturaIMU {
  float ax, ay, az; // Acelerómetro
  float gx, gy, gz; // Giróscopo
  float mx, my, mz; // Magnetómetro
};

// Array para recibir las 5 muestras del bloque
lecturaIMU sensorData[5];

volatile bool flagRecibido = false;

// Evento de interrupción I2C (se dispara cuando el Maestro transmite)
void recibirDatosI2C(int bytesRecibidos) {
  // Verificar que recibimos exactamente los 180 bytes (5 muestras * 36 bytes)
  if (bytesRecibidos == sizeof(sensorData)) {
    Wire.readBytes((uint8_t*)sensorData, bytesRecibidos);
    flagRecibido = true;
  } else {
    // Si la trama no encaja con la estructura, limpiar el buffer
    while (Wire.available()) {
      Wire.read();
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Configuración del LED indicador
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Inicializar bus I2C en modo Esclavo (dirección 0x08, SDA=8, SCL=9)
  Wire.begin(DIRECCION_I2C, PIN_SDA, PIN_SCL, 100000);
  Wire.onReceive(recibirDatosI2C);

  Serial.println("==================================================");
  Serial.println("PLACA 2 (ESP32-S3 Esclavo) Lista.");
  Serial.println("Esperando ráfaga de datos I2C desde el Maestro...");
  Serial.println("==================================================");
}

void loop() {
  if (flagRecibido) {
    flagRecibido = false;

    // 1. Encender el LED
    digitalWrite(PIN_LED, HIGH);
    unsigned long tiempoInicioLED = millis();

    // 2. Desplegar los datos de las 5 muestras por Serial
    Serial.println("\n================ RÁFAGA RECIBIDA POR I2C ================");
    for (uint8_t i = 0; i < 5; i++) {
      Serial.print("Muestra #"); Serial.print(i + 1);
      Serial.print(" ["); Serial.print(i * 200); Serial.println(" ms]:");

      // Acelerómetro
      Serial.print("  Accel [g]  -> X: "); Serial.print(sensorData[i].ax, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].ay, 2);
      Serial.print(" | Z: "); Serial.println(sensorData[i].az, 2);

      // Giróscopo
      Serial.print("  Gyro  [dps] -> X: "); Serial.print(sensorData[i].gx, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].gy, 2);
      Serial.print(" | Z: "); Serial.println(sensorData[i].gz, 2);

      // Magnetómetro
      Serial.print("  Mag   [uT]  -> X: "); Serial.print(sensorData[i].mx, 2);
      Serial.print(" | Y: "); Serial.print(sensorData[i].my, 2);
      Serial.print(" | Z: "); Serial.println(sensorData[i].mz, 2);
    }
    Serial.println("========================================================\n");

    // Mantener el LED encendido exactamente 1 segundo (1000 ms) sin bloquear interrupciones
    while (millis() - tiempoInicioLED < 1000);
    digitalWrite(PIN_LED, LOW); // Apagar LED
  }
=======
#define DIRECCION_I2C 0x08  // Dirección I2C del ESP32-S3
#define LED_PIN 10         // Pin donde está conectado el LED

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Inicializa el I2C en modo esclavo con la dirección 0x08
  Wire.begin(DIRECCION_I2C);
  
  Serial.println("ESP32-S3 Esclavo I2C listo (modo loop)...");
}

void loop() {
  // Comprobamos si el Maestro ha enviado algún byte al bus
  if (Wire.available()) {
    char c = Wire.read();  // Lee el carácter recibido
    
    if (c == '1') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Comando recibido: 1 -> LED encendido");
    } 
    else if (c == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Comando recibido: 0 -> LED apagado");
    }
  }

  // Pequeña pausa para no saturar el procesador
  delay(10);
>>>>>>> 0931406e51cbe9fcc64e29d092f6088f0fdea00c
}