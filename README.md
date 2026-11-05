# Prácticas de Laboratorio IoT

Este repositorio contiene el código fuente y scripts desarrollados para las prácticas de la asignatura Laboratorio de Comunicaiones IoT, abarcando desde el control de periféricos a bajo nivel y sistemas operativos en tiempo real (RTOS), hasta comunicaciones inalámbricas (WiFi, BLE, Bluetooth Classic) y procesamiento/visualización de datos con Python.

---

## Estructura del Repositorio

```text
.
├── Practica_1/    # Periféricos, ADC, PWM, Timers e I2C (Arduino Nano 33 BLE)
├── Practica_2/    # Programación multitarea con FreeRTOS (ESP32)
├── Practica_3/    # Comunicaciones WiFi, Sockets TCP, NTP, Web Server, FTP y MQTT (ESP32)
├── Practica_4/    # Servicios GATT BLE (Arduino Nano 33 BLE) y Bluetooth Classic SPP (ESP32)
├── Practica_5/    # Adquisición serie, exportación a CSV y visualización en tiempo real (Python)
└── README.md

```
## Resumen de Prácticas

### Práctica 1: Arquitectura Arduino Nano 33 sense BLE y Comunicación I2C con ESP32-S3
* **Objetivo:** Control e interconexión de periféricos a bajo nivel en el microcontrolador.
* **Contenido:** Lectura analógica (ADC), generación de señales PWM, uso de temporizadores hardware (`BBTimer`), gestión de comandos por UART, comunicación I2C entre placas y muestreo del sensor inercial IMU LSM9DS1.

### Práctica 2: Diseño de firmware basado en sistema operativo de tiempo real (RTOS) con ESP32
* **Objetivo:** Diseño de firmware concurrente en ESP32 utilizando el sistema operativo FreeRTOS.
* **Contenido:** Creación y gestión de tareas independientes (control de LEDs, lectura periódica de sensores inerciales y temporización de transmisiones serie).

### Práctica 3: Comunicaciones Wi-Fi con ESP32
* **Objetivo:** Implementación de comunicaciones a bajo nivel y protocolos de capa de aplicación.
* **Contenido:** Conexión a redes WiFi, sincronización horaria mediante NTP, comunicación por sockets TCP, servidor web, generación y subida de ficheros JSON (estándar SenML) y publicación/suscripción MQTT en Adafruit IO.

### Práctica 4: Comunicaciones Bluetooth Low Energy (BLE) y Bluetooth Classic
* **Objetivo:** Configuración y gestión de perfiles de comunicación inalámbrica Bluetooth.
* **Contenido:** Servidor GATT en Arduino Nano 33 BLE con UUID personalizado, control de LED RGB mediante características de lectura/escritura, transmisión de datos del acelerómetro por notificaciones y pasarela de chat por Bluetooth Classic en ESP32.

### Práctica 5: Adquisición y Visualización de Datos con Python
* **Objetivo:** Procesamiento, almacenamiento y representación gráfica en tiempo real de datos procedentes de sensores.
* **Contenido:** Captura del puerto serie desde el PC con `pyserial` y formateo de datos a ficheros estructurados CSV/TXT (delimitados por `;`) para análisis en Excel.

---

## Tecnologías y Herramientas

* **Placas de Desarrollo:** Arduino Nano 33 BLE, ESP32.
* **Entorno de Desarrollo:** Visual Studio Code + PlatformIO.
* **Lenguajes de Programación:** C++ (Embedded), Python 3.x.
* **Librerías Principales:** `ArduinoBLE`, `Arduino_LSM9DS1`, `WiFi`, `EspMQTTClient`, `pyserial`.
