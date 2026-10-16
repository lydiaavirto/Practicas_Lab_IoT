/**
 * @file apartado1.cpp
 * @brief Práctica 1 - Apartado 1: Muestreo periódico de canal analógico (ADC).
 * @details Este programa realiza la lectura del valor analógico en el pin A0
 *          con una periodicidad de 1 segundo utilizando delay(). Calcula la tensión 
 *          equivalente en milivoltios (0-3300 mV) y da formato a la salida 
 *          vía consola mediante sprintf().
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>

/** @brief Pin de entrada analógica conectado al potenciómetro */
const int pinADC = A0;

/**
 * @brief Configuración inicial del sistema y del puerto serie.
 * @details Inicializa la comunicación UART a 9600 baudios para el envío de datos a la consola.
 */
void setup() {
  Serial.begin(9600);
}

/**
 * @brief Bucle principal de ejecución.
 * @details Realiza la lectura del ADC, convierte la medida a tensión (mV), 
 *          formatea la cadena de texto con sprintf() y la envía por el puerto serie cada 1000 ms.
 */
void loop() {
  // 1. Lectura del valor del ADC (0 a 1023 por defecto - 10 bits)
  int valorADC = analogRead(pinADC);

  // 2. Cálculo de la tensión en Milivoltios (0 a 3300 mV)
  // Regla de tres: 1023 -> 3300 mV; valorADC -> tension_mV
  long tension_mV = ((long)valorADC * 3300) / 1023;

  // 3. Formateo de la cadena de texto mediante sprintf()
  char buffer[64];
  sprintf(buffer, "ADC A0: %4d | Tension aproximada: %ld mV", valorADC, tension_mV);

  // 4. Muestra del resultado formateado por el puerto serie
  Serial.println(buffer);

  // 5. Pausa de 1 segundo antes de la siguiente lectura
  delay(1000); 
}