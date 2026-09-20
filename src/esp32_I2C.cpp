#include <Arduino.h>
#include <Wire.h>

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
}