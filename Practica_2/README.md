# Práctica 2: Práctica 2	Diseño de firmware basado en sistema operativo de tiempo real (RTOS) con ESP32

---

## Descripción general
El objetivo principal de esta práctica es comprender y aplicar los conceptos de sistemas operativos en tiempo real (FreeRTOS) sobre un ESP32. Asimismo, se integrará un Arduino Nano 33 BLE para la captura de datos inerciales y la comunicación entre ambos dispositivos mediante el protocolo I2C.

---

## ¿Cómo se usa este código?
1. Si vas a probar el apartado 1 (solo ESP32):
  * Modifica en el archivo `platformio.ini` la propiedad `build_src_filter` con el apartado `+<apartado_1/apartado1.cpp>`).
  * Selecciona el entorno  `esp32-s3-devkitc-1`.
2. Si vas probar el apartado 2 (ambas placas se usan):
  * Modifica en el archivo `platformio.ini` la propiedad `build_src_filter` con el main correspondiente (ej. `+<apartado_2/main_Nano.cpp>`).
  * Selecciona el entorno  `esp32-s3-devkitc-1` o `nano33ble` dependiendo del código a cargar.


## Estructura del proyecto

```text
Practica_2/
├── .pio/
├── .vscode/
├── include/
├── lib/
├── src/
│   ├── apartado_1/
│   │   └── apartado1.cpp      # Tarea 1: Multitarea básica con FreeRTOS (LED Blink + Hola Mundo)
│   └── apartado_2/
│       ├── main_ESP32.cpp     # Tarea 2: ESP32 Maestro I2C + temporización de tareas en FreeRTOS
│       └── main_Nano.cpp      # Tarea 2: Arduino Nano 33 BLE Esclavo I2C + Lectura de IMU (LSM9DS1)
├── platformio.ini             # Configuración de entornos y placas
└── README.md
```

## Apartado 1: Multitarea Básica con FreeRTOS

* **Descripción:** Implementación periódica mediante la creación de dos tareas independientes en FreeRTOS. Mientras una tarea gestiona la conmutación periódica de un LED, la otra se encarga de enviar mensajes por el terminal.
* **Lógica y Funciones:**
  * `xTaskCreate(Tarea1, "Tarea1", 10000, NULL, 1, NULL)`: Crea la tarea encargada de la comunicación asignándole un stack de 10.000 palabras y prioridad 1.
  * `xTaskCreate(Tarea2, "Tarea2", 10000, NULL, 1, NULL)`: Crea la tarea de control del LED con el mismo nivel de prioridad para ejecución concurrente.
  * `Serial.println("Hola mundo")`: Transmite la cadena de texto por el puerto serie cada segundo desde `Tarea1`.
  * `digitalWrite(PIN_LED, estado)`: Conmuta el estado lógico del pin GPIO 10 cada 200 ms desde `Tarea2`.

## Apartado 2: Uso de tareas para la lectura y escritura de un sensor incercial

### 2.1. Nodo Esclavo (Arduino Nano 33 BLE)
* **Descripción:** Muestreo rápido de los tres ejes del acelerómetro integrado y atención de peticiones de lectura del maestro I2C mediante interrupciones de hardware.
* **Lógica y Funciones:**
  * `Wire.begin(DIRECCION_I2C)`: Inicializa el bus I2C configurando la placa Arduino Nano 33 BLE como nodo esclavo en la dirección `0x08`.
  * `Wire.onRequest(atenderPeticion)`: Registra la función callback de interrupción que responde enviando la estructura de datos `muestraActual`.
  * `IMU.readAcceleration(x, y, z)`: Captura en tiempo real los valores de aceleración ($a_x, a_y, a_z$) del sensor inercial.
  * `noInterrupts()` / `interrupts()`: Delimita la sección crítica para garantizar una copia atómica e inmune a interrupciones hacia la variable `volatile`.

### 2.2. Nodo Maestro (ESP32)
* **Descripción:** Lectura periódica vía I2C de los datos inerciales procedentes del Nano 33 BLE y escritura de las muestras vía terminal. Asimismo, se incluye una notificación visual en un LED con temporización.
* **Lógica y Funciones:**
  * `Wire.begin(PIN_SDA, PIN_SCL, 100000)`: Configura el ESP32 como Maestro I2C utilizando los pines GPIO 8 (SDA) y GPIO 9 (SCL) a una frecuencia de 100 kHz.
  * `Wire.requestFrom(DIRECCION_ESCLAVO, sizeof(acel_leida))`: Solicita al esclavo `0x08` la transmisión directa de los 12 bytes correspondientes a la lectura de aceleración.
  * `Wire.readBytes(...)`: Lee el buffer I2C y vuelca la ráfaga binaria directamente en la estructura de datos local.
  * `vTaskDelayUntil(&xLastWakeTime, xPeriodo)`: Bloquea la tarea considerando el instante inicial del ciclo, asegurando un periodo exacto de 1 segundo sin acumulación de error de reloj.

