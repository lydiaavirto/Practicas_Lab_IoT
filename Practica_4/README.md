# Práctica 4: Comunicaciones Bluetooth Low Energy (BLE) y Bluetooth Classic

---

## Descripción General

Implementación y validación de protocolos de comunicación **Bluetooth Low Energy (BLE)** y **Bluetooth** utilizando la placa **Arduino Nano 33 BLE** (apartados 1 al 4) y **ESP32** (apartado 5). La práctica abarca la configuración de servidor y cliente GATT, creación de servicios y características personalizadas, emisión de anuncios (Advertising), envío de notificaciones y escaneo de dispositivos BLE.

## ¿Cómo se usa este código?
Cada apartado se gestiona de la siguiente forma:

1. **Configuración del entorno:**
   * Selecciona el entorno `env:nano33ble` en la barra inferior de PlatformIO.
   * Modifica en el archivo `platformio.ini` la propiedad `build_src_filter` indicando la ruta del apartado que deseas probar (ej. `+<apartado_1/apartado1.cpp>`).
2. Si vas a probar el **Apartado 5**:
   * Carga el archivo (`+<apartado_5/apartado5.cpp>`) seleccionando el entorno `` para el ESP32.
3. **Carga y Monitoreo:**
   * Compila y sube el firmware a la placa.
   * Abre la consola serie configurada a **115200 baudios**.

## Estructura del Proyecto

```text
.
├── include/
├── lib/
├── src/
│   ├── apartado_1/       # Configuración de servidor y Advertising (Arduino Nano 33 BLE)
│   ├── apartado_2/       # Servicios y Características GATT (Arduino Nano 33 BLE)
│   ├── apartados_3_4/    # Notificaciones BLE y gestión de eventos (Arduino Nano 33 BLE)
│   └── apartado_5/       # Cliente BLE y escaneo de dispositivos (ESP32)
├── test/
├── .gitignore
├── platformio.ini
└── README.md
```
## Apartado 1: Inicialización de BLE y Advertising con UUID Personalizado
* **Descripción:** Configuración del módulo BLE de la placa Arduino Nano 33 BLE en modo servidor para iniciar la emisión de paquetes de anuncios (Advertising) visibles para otros dispositivos, incluyendo un nombre local y el UUID de un servicio GATT.
* **Lógica y Funciones:**
  * `BLE.begin()`: Inicializa y enciende el módulo de hardware Bluetooth Low Energy.
  * `BLEService servicio(uuid)`: Instancia un servicio GATT utilizando un UUID único de 128 bits.
  * `BLE.setLocalName("NanoBLE_Lydia")`: Asigna el nombre público que transmitirá la placa al ser escaneada.
  * `BLE.setAdvertisedService(servicio)`: Incluye el UUID del servicio dentro del paquete de anunciamiento.
  * `BLE.addService(servicio)`: Registra el servicio creado en la estructura del servidor GATT de la placa.
  * `BLE.advertise()`: Activa la emisión por radiofrecuencia de los paquetes de anuncio.
 
## Apartado 2: Servicios y Características GATT (Control de LED RGB)
* **Descripción:** Configuración de un servicio y una característica GATT con permisos de lectura y escritura (`BLERead | BLEWrite`) en la placa Arduino Nano 33 BLE, permitiendo recibir valores desde una aplicación para controlar el estado y color de un LED RGB.
* **Lógica y Funciones:**
  * `BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite)`: Instancia la característica de 1 byte con UUID `2A57` para transmisión de datos de lectura/escritura.
  * `ledService.addCharacteristic(switchCharacteristic)`: Añade la característica al servicio GATT `180A`.
  * `switchCharacteristic.writeValue(0)`: Establece el valor numérico inicial de la característica.
  * `BLE.central()`: Detecta y captura la conexión de un dispositivo central.
  * `central.connected()`: Mantiene el procesamiento continuo de eventos mientras el dispositivo central permanezca vinculado.
  * `switchCharacteristic.written()`: Comprueba si el dispositivo central ha escrito un nuevo valor en la característica.
  * `switchCharacteristic.value()`: Lee el dato numérico recibido (1: Rojo, 2: Verde, 3: Azul, otro: Apagado) para conmutar los pines digitales del LED RGB.
  * `digitalWrite(LED_BUILTIN, HIGH/LOW)`: Enciende el LED integrado al establecerse la conexión BLE y lo apaga tras la desconexión como indicador de estado.

## Apartados 3 y 4: Notificaciones BLE y Control del Acelerómetro
* **Descripción:** Lectura de las componentes de aceleración de la IMU integrada en la placa Arduino Nano 33 BLE y su transmisión periódica (cada 500 ms) mediante notificaciones BLE hacia un dispositivo central, integrando además una característica de control remoto por escritura para activar o desactivar el envío de datos.
* **Lógica y Funciones:**
  * `BLECharacteristic accelDataCharacteristic(..., BLERead | BLENotify, 32)`: Instancia la característica para la transmisión de datos con permisos de lectura y notificaciones de hasta 32 bytes.
  * `BLEByteCharacteristic accelControlCharacteristic(..., BLEWrite)`: Instancia la característica de control de 1 byte con permiso exclusivo de escritura para conmutar el estado del servicio.
  * `accelControlCharacteristic.written()`: Comprueba si el dispositivo central ha enviado un nuevo comando de control.
  * `accelControlCharacteristic.value()`: Extrae el comando numérico recibido (1: Activar lectura, 0: Desactivar transmisión).
  * `IMU.readAcceleration(x, y, z)`: Lee la aceleración de la IMU LSM9DS1 en los ejes X, Y y Z.
  * `accelDataCharacteristic.writeValue(datosAccel.c_str())`: Actualiza el valor de la característica y dispara de forma automática la notificación hacia la central suscrita.
  * `millis()`: Controla la lectura del sensor y la emisión de notificaciones a intervalos de 500 ms de forma no bloqueante.

## Apartado 5: Comunicación Bluetooth Classic 
* **Descripción:** Configuración del ESP32 en modo Bluetooth Classic mediante el perfil de puerto serie (SPP), permitiendo establecer una pasarela de comunicación bidireccional entre el monitor serie de la PC y una aplicación móvil.
* **Lógica y Funciones:**
  * `BluetoothSerial SerialBT`: Instancia el objeto para la gestión de la comunicación por Bluetooth Classic.
  * `SerialBT.begin("ESP32_Chat_Lab")`: Inicializa la pila Bluetooth y establece el nombre visible del dispositivo para el escaneo y emparejamiento.
  * `Serial.available()`: Comprueba si existen caracteres introducidos desde el PC pendientes en el búfer serie.
  * `Serial.read()`: Extrae el siguiente carácter disponible del puerto serie de la PC.
  * `SerialBT.write(...)`: Envíos por radiofrecuencia hacia el dispositivo móvil de los datos leídos desde el PC.
  * `SerialBT.available()`: Comprueba si la aplicación móvil ha enviado datos que estén pendientes de lectura en el búfer Bluetooth.
  * `SerialBT.read()`: Extrae el siguiente byte recibido a través de la conexión Bluetooth.
  * `Serial.write(...)`: Imprime en el monitor serie de la PC el byte recibido desde el teléfono móvil.



