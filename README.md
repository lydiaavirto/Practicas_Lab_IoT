# Práctica 1: Sistema Embebido Múltiple y Comunicación I2C (Arduino Nano 33 BLE y ESP32-S3)

---

## Descripción General

Este proyecto implementa un sistema embebido distribuido enfocado en la adquisición de datos de sensores, generación y tratamiento de señales (ADC, PWM, Timers) y comunicación bidireccional sobre el bus I2C entre dos plataformas de desarrollo: **Arduino Nano 33 BLE** y **ESP32-S3**.

## ¿Cómo probar los subapartados del proyecto?

Como cada apartado se gestiona mediante filtros de código en PlatformIO:

1. Si vas a probar los **Apartados 1, 2, 3, 4 y 6** (desarrollados para la Arduino Nano 33 BLE):
   * Selecciona el entorno `env:nano33ble` en la barra inferior de PlatformIO.
   * Modifica en el archivo `platformio.ini` la propiedad `build_src_filter` apuntando al apartado que deseas probar (ej. `+<apartado_6/apartado6.cpp>`).
2. Si vas a probar los **Apartados 5 y 7** (Comunicación I2C Maestro-Esclavo):
   * Carga el archivo Maestro (`+<apartado_7/main_Nano.cpp>`) seleccionando el entorno `env:nano33ble` para la Arduino Nano 33 BLE.
   * Carga el archivo Esclavo (`+<apartado_7/main_ESP32.cpp>`) seleccionando el entorno `env:esp32s3` para la ESP32-S3.
3. Abre el **Monitor Serie a 115200 baudios** (o 9600 según el apartado) para la lectura de eventos.

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
