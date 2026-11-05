#include <Arduino.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth no está habilitado. Por favor, habilítalo en la configuración.
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  
  // Nombre del dispositivo que aparecerá en el escaneo Bluetooth del móvil
  SerialBT.begin("ESP32_Chat_Lab"); 
  
  Serial.println("--- Bluetooth SPP Iniciado ---");
  Serial.println("Busca 'ESP32_Chat_Lab' en el Bluetooth de tu móvil y conéctate.");
}

void loop() {
  // 1. Lee lo que escribes en el terminal y lo envía al móvil por Bluetooth
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  // 2. Lee los mensajes que llegan desde la App del móvil y los imprime en el terminal
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
}