# Práctica 1: Arquitectura Arduino Nano 33 sense BLE y Comunicación I2C con ESP32-S3

---

## Descripción General

El propósito global de esta práctica es diseñar, implementar y validar un sistema embebido distribuido completo mediante el desarrollo de 7 subapartados. A lo largo del proyecto se abarca desde los fundamentos del hardware hasta la integración avanzada de periféricos. La práctica termina con la lectura periódica de una unidad de medición inercial (IMU) y la transmisión de datos en tiempo real mediante un bus de comunicación I2C interconectando dos arquitecturas distintas: **Arduino Nano 33 BLE Sense** y **ESP32-S3**.

## ¿Cómo se usa este código?

Cada apartado se gestiona de la siguiente forma:

1. Si vas a probar los **Apartados 1, 2, 3, 4 y 6** (para la Arduino Nano 33 BLE):
   * Selecciona el entorno `env:nano33ble` en la barra inferior de PlatformIO.
   * Modifica en el archivo `platformio.ini` la propiedad `build_src_filter` con el apartado que deseas probar (ej. `+<apartado_6/apartado6.cpp>`).
2. Si vas a probar los **Apartados 5 y 7** (Comunicación I2C Maestro-Esclavo):
   * Carga el archivo Maestro (`+<apartado_7/main_Nano.cpp>`) seleccionando el entorno `env:nano33ble` para la Arduino Nano 33 BLE.
   * Carga el archivo Esclavo (`+<apartado_7/main_ESP32.cpp>`) seleccionando el entorno `env:esp32s3` para la ESP32-S3.
3. Abre la consola serie configurada a **115200 baudios**.

## Estructura del Proyecto

```text
.
├── include/
│   └── BBTimer.h
├── src/
│   ├── apartado_1/       # Lectura de entradas analógicas (ADC)
│   ├── apartado_2/       # Configuración y uso de Timers hardware (BBTimer)
│   ├── apartado_3/       # Generación de señales PWM
│   ├── apartado_4/       # Protocolo UART y comandos por consola
│   ├── apartado_5/       # Comunicación I2C básica (Maestro/Esclavo)
│   ├── apartado_6/       # Muestreo de IMU (LSM9DS1) con BBTimer a 100ms
│   └── apartado_7/       # Integración I2C + IMU en ráfaga a demanda (180 bytes)
│       ├── main_Nano.cpp
│       └── main_ESP32.cpp
├── doc/
│   └── html/             # Documentación HTML generada por Doxygen
├── Doxyfile              # Archivo de configuración Doxygen
└── platformio.ini        # Configuración de entornos y build_src_filter
```

## Apartado 1: Adquisición Analógica (ADC)
* **Descripción:** Lectura del canal analógico A0 periódicamente. Se escala la señal a tensión real en milivoltios ($0\text{-}3300\text{ mV}$) y se transmite la lectura por el terminal.
* **Lógica y Funciones:**
  * `BBTimer miTimer(BB_TIMER3)`: Instancia del temporizador por hardware utilizando el módulo BB_TIMER3.
  * `sprintf(buffer, ...)`: Da formato a la cadena de salida para mostrar el valor del ADC y la tensión equivalente en milivoltios.
  * `Serial.println(buffer)`: Escribe el valor leído por el terminal.
  * `delay(1000)`: Pausa de $1\text{ s}$ antes de la siguiente lectura.
 
## Apartado 2: Adquisición Analógica (ADC)
* **Descripción:** Configuración de un temporizador por hardware para generar una interrupción periódica. La rutina de servicio (ISR / callback) activa una bandera (flag) que procesa la lectura del ADC en el bucle principal.
* **Lógica y Funciones:**
  * `analogRead(pinADC)`: Captura el nivel de tensión en el pin analógico.
  * `volatile bool flagTimer`: 	Volatile flag para sincronización entre la ISR y el bucle principal.
  * `	callbackTimer()`: Rutina de servicio de la interrupción (Callback) del temporizador. Se ejecuta de forma que cada vez que el temporizador hardware alcanza el tiempo programado, activa la bandera de sincronización.
  * `miTimer.setupTimer(10000000, callbackTimer)`: Configura el timer para saltar cada 10.000.000 us (10 segundos).
  * `miTimer.timerStart()`: Inicia el temporizador de hardware.
  * `if (flagTimer)`: Cuando transcurren los 10 segundos, se activa la bandera desde la ISR y ejecuta la tarea.

## Apartado 3: Modulación por Ancho de Pulsos (PWM)
* **Descripción:** Generación de una señal PWM mediante la API de Mbed OS (`mbed::PwmOut`). El ciclo de trabajo (*Duty Cycle*) se ajusta dinámicamente entre el 0% y el 100% de forma proporcional a la tensión leída en el pin analógico A0, mostrando lecturas de diagnóstico cada 50 ms por el puerto serie.
* **Lógica y Funciones:**
  * `mbed::PwmOut pwmSalida(digitalPinToPinName(D2))`: Permite la reconfiguración directa de la frecuencia y el ciclo de trabajo.
  * `pwmSalida.period_us(200)`: Configura el periodo de la señal PWM a 200 us (frecuencia de 5 kHz).
  * `pwmSalida.write(dutyCycle)`: Ajusta el ciclo de trabajo con un valor flotante en el rango [0.0, 1.0].

 ## Apartado 4: Interfaz de Comunicación y Comandos por UART
* **Descripción:** Implementación de un protocolo de comunicación serie mediante un analizador de cadenas. Permite solicitar lecturas puntuales del ADC a 12 bits (`ADC`), programar muestreos periódicos del ADC mediante temporizador hardware (`ADC(x)`) y configurar el *Duty Cycle* del PWM de 5 kHz en un rango de 0 a 9 (`PWM(x)`).
* **Lógica y Funciones:**
  * `enviarADC()`: Función auxiliar que realiza la lectura del pin A0 en 12 bits, calcula la tensión en milivoltios y emite el resultado formateado mediante `sprintf()`.
  * `analogReadResolution(12)`: Cambia la resolución del ADC a 12 bits (0 - 4095).
  * `miTimer.timerStop()`: Para el temporizador de hardware.
  * `pwmSalida.write(duty)`: Ajusta el ciclo de trabajo del PWM mediante la librería Mbed OS, escalando el parámetro `x` (0-9) a un rango flotante (0.0 a 1.0).
* **Procesamiento de Comandos:** Lee el puerto serie carácter a carácter almacenándolos en la variable `data` mediante `concat()`. Al detectar el salto de línea (`\n`), analiza la cadena mediante métodos como `startsWith()` y `substring()` para ejecutar la acción correspondiente y reiniciar el búfer.

## Apartado 5: Enlace I2C Maestro-Esclavo

### 5.1. Nodo Maestro (Arduino Nano 33 BLE)
* **Descripción:** Implementación de la interfaz I2C en modo Maestro utilizando los pines dedicados SDA (A4) y SCL (A5). Genera de forma periódica tramas de control dirigidas al nodo esclavo enviando de forma alternada los caracteres `'1'` y `'0'` para ordenar la conmutación de un LED.
* **Funciones y Lógica Clave:**
  * `Wire.begin()`: Inicializa la librería `Wire` en modo Maestro.
  * `Wire.beginTransmission(0x08)`: Inicia la comunicación serie orientada al dispositivo esclavo con dirección `0x08`.
  * `Wire.write('1')` / `Wire.write('0')`: Carga en el búfer de salida el carácter correspondiente al estado del LED.
  * `Wire.endTransmission()`: Envía los datos cargados en el bus y emite la condición de STOP para liberar el canal I2C.

### 5.2. Nodo Esclavo (ESP32-S3)
* **Descripción:** Configuración de la ESP32-S3 como nodo Esclavo en el bus I2C asignándole la dirección `0x08`. Atiende las peticiones del Maestro de forma asíncrona mediante interrupciones, leyendo los comandos recibidos y conmutando el estado del LED conectado al GPIO 10.
* **Funciones y Lógica Clave:**
  * `Wire.begin(0x08)`: Configura el periférico I2C en modo Esclavo escuchando bajo la dirección `0x08`.
  * `Wire.onReceive(receiveEvent)`: Asocia la función de *callback* que se dispara de forma automática mediante una interrupción (ISR) al recibir datos.
  * `receiveEvent(int howMany)`: Rutina ISR que vacía el búfer de recepción con `Wire.read()`, evalúa el carácter enviado (`'1'` o `'0'`) y modifica el nivel lógico del pin mediante `digitalWrite(10, HIGH/LOW)`.
  * 
## Apartado 6: Muestreo Periódico de la IMU (LSM9DS1)
* **Descripción:** Implementación del muestreo periódico de los 9 grados de libertad (acelerómetro, giroscopio y magnetómetro) de la IMU LSM9DS1 integrada en la Arduino Nano 33 BLE. 
* **Funciones y Lógica Clave:**
  * `struct lecturaIMU`: Estructura personalizada que empaqueta las componentes flotantes de los 3 ejes de aceleración (`ax, ay, az`), velocidad angular (`gx, gy, gz`) y campo magnético (`mx, my, mz`).
  * `timerMuestreo.setupTimer(100000, callbackTimer)`: Programa el temporizador para interrumpir cada 100 000 us (100 ms).
  * `callbackTimer()`: Rutina ISR que conmuta `flagLectura`, incrementa el contador `nMuestra` y activa `flagEscritura` al alcanzar las 10 lecturas (1 s).
  * `IMU.readAcceleration()` / `IMU.readGyroscope()` / `IMU.readMagneticField()`: Métodos de lectura para capturar los datos inerciales del sensor cuando la bandera de la ISR está activa.
* **Procesamiento**: Mediante un temporizador hardware (`BBTimer`) ajustado a 100 ms se adquieren las lecturas y se van almacenando en un array de estructuras de 10 posiciones. Al completarse 1 segundo (10 muestras), el bucle principal imprime la ráfaga completa de datos de forma estructurada por la consola serie.

## Apartado 7: Transmisión de Ráfagas IMU por I2C
#### 7.1. Nodo Maestro (Arduino Nano 33 BLE)
* **Descripción:** Adquisición a demanda de datos inerciales de la IMU LSM9DS1 desencadenada por puerto serie (al recibir un carácter por UART). Utiliza `BBTimer` a 200 ms para capturar 5 muestras completas (1 segundo de datos). Tras acumular la ráfaga, detiene el temporizador y transmite el bloque binario continuosobre el bus I2C al nodo esclavo (`0x08`).
* **Funciones y Lógica Clave:**
  * `struct lecturaIMU`: Estructura con las 9 variables flotantes de la IMU que suman un tamaño de 36 bytes por muestra.
  * `timerMuestreo.setupTimer(200000, callbackTimer)`: Programa la interrupción del temporizador a 200 ms (200 000 us).
  * `callbackTimer()`: Rutina ISR que conmuta `flagLectura`, gestiona el índice de muestras y activa `flagEscritura` al acumular las 5 capturas.
  * `Wire.beginTransmission(0x08)` / `Wire.write((uint8_t*)sensorData, sizeof(sensorData))` / `Wire.endTransmission()`: Convierte el array de estructuras a un puntero de bytes y emite el bloque binario completo de 180 bytes hacia el esclavo en una sola transmisión.
### 7.2. Nodo Esclavo (ESP32-S3)
* **Descripción:** Configuración de la ESP32-S3 como Esclavo en el bus I2C (dirección `0x08`, pines SDA=GPIO8, SCL=GPIO9 a 100 kHz).
* **Funciones y Lógica Clave:**
  * `Wire.begin(0x08, 8, 9, 100000)`: Inicializa la interfaz I2C en modo Esclavo definiendo dirección, pines de comunicación y frecuencia de reloj (100 kHz).
  * `Wire.onReceive(recibirDatosI2C)`: Registra la rutina ISR para capturar las transmisiones entrantes del Maestro.
  * `recibirDatosI2C(bytesRecibidos)`: Función callback de interrupción que verifica la integridad del bloque (`bytesRecibidos == sizeof(sensorData)`) y vuelca directamente los datos en el array mediante `Wire.readBytes((uint8_t*)sensorData, bytesRecibidos)`.
  * `millis()`: Hace un conteo de tiempo para el encendido del LED.
* **Procesamiento**: Recibe de forma asíncrona mediante interrupciones de ráfagas de 180 bytes (5 muestras de la IMU × 36 bytes), valida el tamaño del bloque recibido, vuelca los datos en un array de estructuras y los despliega por el terminal serie a 115200 baudios, activando un LED indicador en el GPIO 10 durante exactamente 1 segundo sin bloquear el sistema.
