/**
 * @file apartado2.cpp
 * @brief Práctica 1 - Apartado 2: Muestreo del ADC mediante interrupciones de Timer Hardware.
 * @details Este programa configura un temporizador por hardware (BBTimer) para generar 
 *          una interrupción periódica cada 10 segundos. La rutina de servicio (ISR / callback) 
 *          activa una bandera (flag) que procesa la lectura del ADC en el bucle principal.
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>
#include "BBTimer.h"

/** @brief Pin de entrada analógica conectado al potenciómetro */
const int pinADC = A0;

/** @brief Instancia del temporizador por hardware utilizando el módulo BB_TIMER3 */
BBTimer miTimer(BB_TIMER3);

/** @brief Volatile flag para sincronización entre la ISR y el bucle principal. */
volatile bool flagTimer = false;

/**
 * @brief Rutina de servicio de la interrupción (Callback) del temporizador.
 * @details Se ejecuta de forma que cada vez que el temporizador hardware alcanza 
 *          el tiempo programado (10 segundos), activa la bandera de sincronización.
 */
void callbackTimer() {
  flagTimer = true;
}

/**
 * @brief Configuración inicial del sistema, puerto serie y temporizador hardware.
 * @details Inicializa la consola serie a 9600 baudios, programa el BBTimer para caducar 
 *          cada 10.000.000 µs (10 s) y arranca el conteo.
 */
void setup() {
  Serial.begin(9600);

  // Configura el timer para saltar cada 10.000.000 us (10 segundos)
  miTimer.setupTimer(10000000, callbackTimer);
  
  // Inicia el temporizador de hardware
  miTimer.timerStart();
}

/**
 * @brief Bucle principal de ejecución.
 * @details Monitoriza la variable `flagTimer`. Al activarse, limpia la bandera, 
 *          lee el ADC, calcula la tensión equivalente en mV y muestra el resultado 
 *          por puerto serie formateado con sprintf().
 */
void loop() {
  // Cuando transcurren los 10 segundos, se activa la bandera desde la ISR
  if (flagTimer) {
    flagTimer = false; // Reiniciamos la bandera

    // 1. Lectura del valor del ADC
    int valorADC = analogRead(pinADC);

    // 2. Cálculo de tensión en milivoltios (0 a 3300 mV)
    long tension_mV = ((long)valorADC * 3300) / 1023;

    // 3. Formateo de la cadena de texto mediante sprintf()
    char buffer[64];
    sprintf(buffer, "[Timer 10s] ADC: %4d | Tension: %ld mV", valorADC, tension_mV);

    // 4. Impresión por consola
    Serial.println(buffer);
  }
}