#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

// 1. UUIDs para el Servicio y las dos Características
const char* SERVICE_UUID               = "19B10000-E8F2-537E-4F6C-D104768A1214";
const char* ACCEL_DATA_CHAR_UUID       = "19B10002-E8F2-537E-4F6C-D104768A1214";
const char* ACCEL_CONTROL_CHAR_UUID    = "19B10003-E8F2-537E-4F6C-D104768A1214";

// 2. Servicio GATT
BLEService accelService(SERVICE_UUID);

// Característica 1: Datos del Acelerómetro (Solo Lectura y Notificación)
BLECharacteristic accelDataCharacteristic(ACCEL_DATA_CHAR_UUID, BLERead | BLENotify, 32);

// Característica 2: Control ON/OFF del Acelerómetro (Solo Escritura)
BLEByteCharacteristic accelControlCharacteristic(ACCEL_CONTROL_CHAR_UUID, BLEWrite);

bool imuIniciada = false;
bool acelerometroActivo = true; // Controla si enviamos notificaciones o no (por defecto: ON)
unsigned long ultimaLectura = 0;

void setup() {
  Serial.begin(115200);

  delay(1500);

  Serial.println("Inicializando sensor LSM9DS1...");
  for (int i = 0; i < 5; i++) {
    if (IMU.begin()) {
      imuIniciada = true;
      Serial.println("-> ¡Sensor LSM9DS1 iniciado con éxito!");
      break;
    }
    Serial.println("-> Reintentando inicialización del LSM9DS1...");
    delay(500);
  }

  // Inicializar módulo BLE
  if (!BLE.begin()) {
    Serial.println("¡Error al inicializar la radio BLE!");
    while (1);
  }

  // Configurar GATT
  BLE.setLocalName("Nano33_Accel_Lydia");
  BLE.setAdvertisedService(accelService);

  // Añadir ambas características al servicio
  accelService.addCharacteristic(accelDataCharacteristic);
  accelService.addCharacteristic(accelControlCharacteristic);
  BLE.addService(accelService);

  // Valores iniciales
  accelDataCharacteristic.writeValue("X:0.00 Y:0.00 Z:0.00");
  accelControlCharacteristic.writeValue(1); // 1 = Activado por defecto

  BLE.advertise();
  Serial.println("Advertising BLE iniciado con caracteristica de control (ON/OFF).");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Conectado a la central: ");
    Serial.println(central.address());

    while (central.connected()) {
      
      // Comprobar si la central ha escrito (Solo Escritura)
      if (accelControlCharacteristic.written()) {
        byte comando = accelControlCharacteristic.value();
        if (comando == 1) {
          acelerometroActivo = true;
          Serial.println("-> Comando recibido: Acelerómetro ACTIVADO (1)");
        } else if (comando == 0) {
          acelerometroActivo = false;
          Serial.println("-> Comando recibido: Acelerómetro DESACTIVADO (0)");
          accelDataCharacteristic.writeValue("Acelerometro OFF");
        }
      }

      // Si el acelerómetro está ACTIVADO, leer y notificar los datos cada 500 ms
      unsigned long ahora = millis();
      if (acelerometroActivo && (ahora - ultimaLectura >= 500)) {
        ultimaLectura = ahora;

        float x, y, z;
        if (imuIniciada && IMU.accelerationAvailable()) {
          IMU.readAcceleration(x, y, z);

          String datosAccel = "X:" + String(x, 2) + " Y:" + String(y, 2) + " Z:" + String(z, 2);
          accelDataCharacteristic.writeValue(datosAccel.c_str());

          Serial.print("Notificación enviada -> ");
          Serial.println(datosAccel);
        }
      }
    }

    Serial.println("Central desconectada. Reanudando Advertising...");
    BLE.advertise();
  }
}