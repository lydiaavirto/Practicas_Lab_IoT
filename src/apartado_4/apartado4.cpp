/**
 * @file apartado4.cpp
 * @brief Práctica 1 - Apartado 4: Protocolo de comunicación UART para el control 
 *        de periféricos (ADC y PWM).
 * @details Implementa un protocolo de comandos por puerto serie para controlar la lectura 
 *          del ADC y el ciclo de trabajo del PWM. Admite los siguientes comandos:
 *          - 'ADC': Muestra una lectura puntual del ADC.
 *          - 'ADC(x)': Configura el envío periódico del ADC cada 'x' segundos mediante Timer. 
 *             Si x=0, detiene el envío.
 *          - 'PWM(x)': Cambia el Duty Cycle del PWM entre 0 y 9 (0% a 100%).
 * @author Lydia Virto Zardoya
 * @date 24/09/2026
 */

#include <Arduino.h>
#include "BBTimer.h"
#include <mbed.h>

/** @brief Pin de entrada analógica conectado al potenciómetro */
const int pinADC = A0;

/** 
 * @brief Salida PWM en el pin D2 a 5 kHz mediante Mbed OS.
 */
mbed::PwmOut pwmSalida(digitalPinToPinName(D2));

/** @brief Buffer de entrada para almacenar el comando recibido por la UART */
String data = "";

/** @brief Instancia del temporizador por hardware BB_TIMER3 */
BBTimer miTimer(BB_TIMER3);

/** @brief Volatile flag para sincronización de la interrupción del temporizador */
volatile bool flagTimer = false;

/**
 * @brief Rutina de servicio (callback) ejecutada por el temporizador.
 * @details Activa la bandera 'flagTimer' para desencadenar el envío del ADC en el bucle principal.
 */
void callbackTimer() {
  flagTimer = true;
}

/**
 * @brief Realiza la lectura del ADC y la envía por el puerto serie.
 * @details Lee el ADC con resolución de 12 bits (0 a 4095), calcula la tensión en mV
 *          y envía el resultado formateado mediante sprintf().
 */
void enviarADC() {
  int valorADC = analogRead(pinADC);
  long tension_mV = ((long)valorADC * 3300) / 4095;
  char buffer[64];
  sprintf(buffer, "ADC A0: %4d | Tension aproximada: %ld mV", valorADC, tension_mV);
  Serial.println(buffer);
}

/**
 * @brief Configuración inicial del sistema, resolución del ADC, PWM y UART.
 */
void setup() {
  Serial.begin(9600);
  delay(1000);

  // Configurar período del PWM a 5 kHz (200 µs)
  pwmSalida.period_us(200);
  pwmSalida.write(0.0f);

  // Cambiar resolución del ADC a 12 bits (0 - 4095)
  analogReadResolution(12);
}

/**
 * @brief Bucle principal de ejecución.
 * @details Analiza los caracteres recibidos por la UART línea por línea y procesa
 *          los comandos "ADC", "ADC(x)" y "PWM(x)". También procesa los eventos
 *          temporizados lanzados por la interrupción del temporizador.
 */
void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '\r') {
      return; 
    }

    if (c != '\n') {
      data.concat(c);
      Serial.print(c); // Eco en el terminal
    } else {
      Serial.println(); // Salto de línea tras pulsar enter
      
      // CASO 1: LECTURA PUNTUAL DEL ADC
      if (data == "ADC") {
        enviarADC();
      }
      // CASO 2: MUESTREO PERIÓDICO DEL ADC MEDIANTE TIMER
      else if (data.startsWith("ADC(") && data.endsWith(")")) {
        String xTimer = data.substring(4, data.length() - 1);
        int tiempo = xTimer.toInt();

        miTimer.timerStop();
        if (tiempo > 0) {
          unsigned long periodo = tiempo * 1000000UL;
          miTimer.setupTimer(periodo, callbackTimer);
          miTimer.timerStart();
        } else {
          Serial.println("Temporizador detenido");
        }
      }
      // CASO 3: CONTROL DEL DUTY CYCLE DEL PWM (0-9)
      else if (data.startsWith("PWM(") && data.endsWith(")")) {
        String xPWM = data.substring(4, data.length() - 1);
        int x = xPWM.toInt();
        
        if (x >= 0 && x <= 9) {
          float duty = (float)x / 9.0f; // Mapea de 0.0 a 1.0 el duty cycle
          pwmSalida.write(duty);
        } else {
          Serial.println("Valor no valido (usar 0-9)");
        }
      } 
      else {
        Serial.print("Comando desconocido: ");
        Serial.println(data);
      }
      
      // Limpiar buffer de entrada para el siguiente comando
      data = "";
    }
  }

  // Verificación de la bandera del temporizador hardware
  if (flagTimer) {
    flagTimer = false;
    enviarADC();
  }
}