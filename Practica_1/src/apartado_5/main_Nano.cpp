/**
 * @file main_Nano.cpp
 * @brief Práctica 1 - Apartado 5: Comunicación I2C Maestro (Arduino Nano 33 BLE).
 * @details Implementa la interfaz I2C Maestro para enviar comandos de control a un 
 *          esclavo (ESP32-S3) con dirección 0x08. Envía alternativamente los caracteres 
 *          '1' y '0' cada 2 segundos para encender y apagar un LED externo conectado 
 *          en la placa esclava.
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>
#include <Wire.h>

/** @brief Dirección I2C del dispositivo esclavo (ESP32-S3) */
const uint8_t DIRECCION_I2C = 0x08;

/**
 * @brief Configuración inicial de la comunicación serie e interfaz I2C Maestro.
 * @details Inicializa la consola a 115200 baudios y configura las líneas SDA (A4) 
 *          y SCL (A5) en modo Maestro mediante la librería Wire.
 */
void setup() {
  Serial.begin(115200);
  while (!Serial); // Espera la apertura del monitor serie
  Wire.begin();    // Inicializa el bus I2C en modo Maestro (SDA=A4, SCL=A5)
}

/**
 * @brief Bucle principal de ejecución del Maestro.
 * @details Genera tramas de escritura I2C dirigidas a la dirección 0x08:
 *          - Envía el carácter '1' para ordenar el encendido del LED.
 *          - Espera 2000 ms.
 *          - Envía el carácter '0' para ordenar el apagado del LED.
 *          - Espera 2000 ms.
 */
void loop() {
  // 1. Enviar comando para encender el LED ('1')
  Wire.beginTransmission(DIRECCION_I2C); // Inicia la trama hacia el esclavo 0x08
  Wire.write('1');                       // Manda el byte de encendido
  Wire.endTransmission();                 // Finaliza la transmisión con la condición de STOP

  delay(2000);

  // 2. Enviar comando para apagar el LED ('0')
  Wire.beginTransmission(DIRECCION_I2C); // Inicia la trama hacia el esclavo 0x08
  Wire.write('0');                       // Manda el byte de apagado
  Wire.endTransmission();                 // Finaliza la transmisión con la condición de STOP

  delay(2000);
}