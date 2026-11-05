# Práctica 5: Comunicación, Gestión y Representación de Datos de Sensores con Python

---

## Descripción General

Implementación de un sistema completo de adquisición, almacenamiento y visualización en tiempo real de datos inerciales procedentes de una placa **Arduino Nano 33 BLE**. El microcontrolador muestrea la aceleración de la IMU integrada cada 50 ms y transmite las lecturas por el puerto serie (UART) a 115200 baudios. En el PC, scripts desarrollados en **Python** procesan la comunicación mediante `pyserial` y almacenan los datos en ficheros estructurados con delimitadores `;` para su posterior procesamiento en hojas de cálculo (Excel).

## ¿Cómo se usa este código?
1. **Firmware Arduino:**
   * Configura `build_src_filter = +<src/main_Nano.cpp>` en `platformio.ini`.
   * Carga el programa en el Arduino Nano 33 BLE para iniciar el envío serie a **115200 baudios**.
2. **Scripts Python:**
   * Ejecuta el script correspondiente de `src/apartado2/` o `src/apartado3/` indicando el puerto COM.

## Estructura del Proyecto

```text
.
├── include/
├── lib/
├── src/
│   ├── apartado2/        # Scripts de Python para recepción por puerto serie
│   ├── apartado3/        # Script de Python para visualización de datos en Excel usando un archivo .txt
│   └── main_Nano.cpp     # Mmuestreo IMU cada 50 ms y envío UART
├── test/
├── .gitignore
├── platformio.ini
└── README.md
```

## Apartado 1: Adquisición Inercial (9 DOF) y Transmisión en Ráfaga por UART (`main_Nano.cpp`)
* **Descripción:** Adquisición de datos inerciales de 9 grados de libertad (acelerómetro, giroscopio y magnetómetro) con el sensor LSM9DS1 de la placa Arduino Nano 33 BLE. El muestreo se realiza con precisión temporal mediante un temporizador hardware a intervalos de 100 ms, acumulando las lecturas en un búfer de 10 muestras para transmitirlas en ráfaga por la UART cada segundo.
* **Lógica y Funciones:**
  * `BBTimer timerMuestreo(BB_TIMER3)`: Instancia del temporizador hardware utilizado para el control estricto del tiempo de muestreo.
  * `struct lecturaIMU`: Estructura de datos para almacenar los 9 valores inerciales ($a_x, a_y, a_z, g_x, g_y, g_z, m_x, m_y, m_z$).
  * `timerMuestreo.setupTimer(100000, callbackTimer)`: Configura e inicia la interrupción periódica del timer a 100 ms (100.000 µs).
  * `callbackTimer()`: Rutina de interrupción (ISR) que activa las banderas de control (`flagLectura` y `flagEscritura`) y contabiliza los lotes de 10 muestras.
  * `IMU.readAcceleration()`, `IMU.readGyroscope()`, `IMU.readMagneticField()`: Lectura síncrona de los ejes triaxiales del sensor inercial.
  * `Serial.print(...)`: Emisión estructurada por puerto serie a 9600 baudios de la ráfaga de 10 capturas al completar cada segundo.
 
## Apartado 2: Lectura en Tiempo Real por Puerto Serie
* **Descripción:** Script de Python desarrollado con la librería `pyserial` para establecer comunicación serie síncrona a 115200 baudios con la placa Arduino Nano 33 BLE, capturando e imprimiendo las lecturas recibidas por consola en tiempo real.
* **Lógica y Funciones:**
  * `serial.Serial(puerto, baudios, timeout=1)`: Abre e inicializa la conexión con el puerto serie indicado (`COM3`) a 115200 baudios con tiempo de espera de lectura.
  * `conexion.in_waiting`: Comprueba si existen bytes de datos disponibles en el búfer de entrada antes de proceder a la lectura.
  * `conexion.readline()`: Captura la cadena de datos transmitida por la placa hasta encontrar el salto de línea.
  * `.decode('utf-8', errors='ignore').strip()`: Decodifica el flujo de bytes a texto UTF-8 ignorando fallos de conversión y elimina espacios o caracteres especiales.
  * `try / except KeyboardInterrupt / finally`: Controla excepciones e interrupciones del teclado (`Ctrl+C`), garantizando la liberación y cierre del puerto con `conexion.close()`.

## Apartado 3: Almacenamiento de Datos Serie en Fichero CSV/TXT
* **Descripción:** Script de Python para la captura de lecturas inerciales transmitidas por el puerto serie y su almacenamiento en tiempo real en un archivo de texto (`datos_acelerometro.txt`), estructurado con punto y coma (`;`) como delimitador para su posterior análisis e importación en Excel.
* **Lógica y Funciones:**
  * `open(NOMBRE_ARCHIVO, "w", encoding="utf-8")`: Abre o crea el archivo destino en modo escritura (`w`).
  * `archivo.write("X;Y;Z\n")`: Escribe la primera línea del archivo a modo de cabecera para definir las columnas de datos.
  * `linea.replace(",", ";").replace(" ", ";")`: Formatea la cadena recibida sustituyendo espacios o comas por punto y coma (`;`) para garantizar la estructura.
  * `archivo.write(linea_csv + "\n")`: Registra cada muestra en el fichero adjuntando un salto de línea.
  * `archivo.flush()`: Fuerza el vaciado del búfer de memoria volcando los datos inmediatamente en el disco sin esperar al cierre del archivo.
