/**
 * @file apartado3.cpp
 * @brief Práctica 1 - Apartado 3: Generación de señal PWM a 5 kHz proporcional a la lectura del ADC.
 * @details Utiliza la API de Mbed OS (PwmOut) para generar una salida PWM de 5 kHz 
 *          en el pin D2. El ciclo de trabajo (Duty Cycle) de la señal varía de forma 
 *          proporcional entre el 0% y el 100% según el valor leído en la entrada analógica A0.
 * @author Lydia Virto Zardoya
 * @date 25/09/2026
 */

#include <Arduino.h>
#include <mbed.h>

/** @brief Pin de entrada analógica conectado al potenciómetro */
const int pinADC = A0;

/** 
 * @brief Salida PWM en el pin D2 utilizando Mbed OS.
 * @details Permite la reconfiguración directa de la frecuencia y el ciclo de trabajo.
 */
mbed::PwmOut pwmSalida(digitalPinToPinName(D2));

/**
 * @brief Configuración inicial del sistema, puerto serie y frecuencia del PWM.
 * @details Inicializa la consola a 9600 baudios y establece el periodo del PWM 
 *          a 200 µs (frecuencia de 5 kHz).
 */
void setup() {
  Serial.begin(9600);

  // 1. Configurar el período para 5 kHz
  // Frecuencia = 5000 Hz -> Período = 1 / 5000 s = 200 microsegundos
  pwmSalida.period_us(200);

  // Inicializar la salida PWM al 0% de Duty Cycle
  pwmSalida.write(0.0f);
}

/**
 * @brief Bucle principal de ejecución.
 * @details Lee la entrada analógica A0, calcula el ciclo de trabajo (0.0 a 1.0) y actualiza 
 *          la salida PWM. Muestra la tensión y el porcentaje de Duty Cycle por puerto serie cada 50 ms.
 */
void loop() {
  // 1. Lectura del ADC (0 a 1023)
  int valorADC = analogRead(pinADC);

  // 2. Mapeo del valor analógico a un rango flotante para el ciclo de trabajo (0.0 a 1.0)
  float dutyCycle = (float)valorADC / 1023.0f;

  // 3. Aplicar el Duty Cycle al módulo PWM de 5 kHz
  pwmSalida.write(dutyCycle);

  // 4. Mostrar información de diagnóstico en el Serial Monitor
  long tension_mV = ((long)valorADC * 3300) / 1023;
  char buffer[64];
  sprintf(buffer, "ADC A0: %4d | Tension: %ld mV | PWM Duty: %d%%", 
          valorADC, tension_mV, (int)(dutyCycle * 100));
  Serial.println(buffer);

  delay(50); // Muestreo fluido para observación e interacción en el potenciómetro
}