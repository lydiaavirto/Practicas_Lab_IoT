/**
 * @file main_ESP32.cpp
 * @brief Práctica 1 - Apartado 5: Comunicación I2C Esclavo (ESP32-S3).
 * @details Configura la ESP32-S3 en modo esclavo I2C con la dirección 0x08. 
 *          Registra un manejador de eventos por interrupción ('onReceive') que procesa 
 *          los comandos recibidos desde el maestro (Arduino Nano) para conmutar un LED.
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>
#include <Wire.h>

/** @brief Dirección I2C asignada a la ESP32-S3 en el bus */
#define I2C_DEV_ADDR 0x08  

/** @brief Pin GPIO conectado al LED externo controlado por I2C */
#define LED_PIN 10         

/**
 * @brief Rutina de servicio/callback de recepción I2C (ISR).
 * @details Se activa automáticamente en modo interrupción al recibir bytes desde el maestro.
 *          Lee los caracteres entrantes y conmuta el estado lógico del 'LED_PIN':
 *          - '1': Enciende el LED.
 *          - '0': Apaga el LED.
 * @param howMany Número de bytes disponibles en el buffer de recepción I2C.
 */
void receiveEvent(int howMany) {
  while (Wire.available()) {
    char c = Wire.read();  // Lee el carácter recibido por I2C
    
    if (c == '1') {
      digitalWrite(LED_PIN, HIGH);  // Enciende el LED
      Serial.println("Comando recibido: 1 -> LED encendido");
    } 
    else if (c == '0') {
      digitalWrite(LED_PIN, LOW);   // Apaga el LED
      Serial.println("Comando recibido: 0 -> LED apagado");
    }
  }
}

/**
 * @brief Configuración inicial del hardware, puerto serie e interfaz esclava I2C.
 * @details Configura el GPIO del LED como salida, inicializa la librería Wire en modo 
 *          esclavo con dirección 0x08 y registra el callback 'receiveEvent'.
 */
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Inicializa el bus I2C en modo esclavo con la dirección 0x08
  Wire.begin(I2C_DEV_ADDR);
  
  // Registrar la función que atenderá las interrupciones de recepción
  Wire.onReceive(receiveEvent);
  
  Serial.println("ESP32-S3 Esclavo I2C listo...");
}

/**
 * @brief Bucle principal de ejecución.
 * @details Se mantiene libre con un retardo pasivo de 100 ms, ya que el procesamiento 
 *          de tramas I2C se gestiona mediante interrupciones.
 */
void loop() {
  delay(100);
}