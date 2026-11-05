# Práctica 3: Comunicaciones Wi-Fi con ESP32

---

## Descripción General

El objetivo de esta práctica es implementar conexiones inalámbricas vía Wi-Fi con **ESP32-S3**. La práctica abarca el uso de sockets TCP, sincronización de hora por NTP, creación de un servidor web, envío de ficheros SenML (JSON) vía FTP e integración en la nube con Adafruit IO (HTTP POST y MQTT).

##

## ¿Cómo se usa este código?

Cada apartado se gestiona de la siguiente forma:

1. **Configuración del entorno:**
   * Selecciona el entorno `env:esp32` (o `env:esp32dev`) en la barra inferior de PlatformIO.
   * Modifica en el archivo `platformio.ini` la propiedad `build_src_filter` indicando la ruta del apartado que deseas probar (ej. `+<apartado_1/apartado1.cpp>`).
2. Si vas a probar el **Apartado 6**:
   * Carga el archivo Maestro (`+<apartado_6/main_Nano.cpp>`) seleccionando el entorno `env:nano33ble` para la Arduino Nano 33 BLE.
   * Carga el archivo Esclavo (`+<apartado_6/main_ESP32.cpp>`) seleccionando el entorno `env:esp32s3` para la ESP32-S3.
3. **Carga y Monitoreo:**
   * Compila y sube el firmware a la placa ESP32.
   * Abre la consola serie configurada a **115200 baudios**.

## Estructura del Proyecto

```text
.
├── data/
├── include/
├── lib/
├── src/
│   ├── apartado_1/       # Conexión a red Wi-Fi STA, obtención de IP y prueba de ping 
│   ├── apartado_2/       # Sincronización de reloj y hora local mediante servidor NTP
│   ├── apartados_4_5/    # Comunicación cliente Socket TCP con control mediante comandos (start/stop)
│   ├── apartado_6/       # Envíos periódicos de datos del acelerómetro sobre Socket TCP
│   ├── apartado_7/       # Servidor Web HTTP (visualización y reseteo de hora)
│   ├── apartado_8/       # Generación de ficheros JSON (estándar SenML) y transferencia a servidor FTP
│   └── apartado_10/      # Integración en la nube con Adafruit IO mediante HTTP POST y MQTT
├── test/
├── .gitignore
├── platformio.ini
└── README.md
```

---

## Apartado 1: Conexión Wi-Fi y Ping
* **Descripción:** Conexión del ESP32 a una red Wi-Fi generada por el móvil/ordenador en modo estación (STA), obtención de la dirección IP asignada y verificación del acceso a Internet mediante un ping a `www.google.com`.
* **Lógica y Funciones:**
  * `WiFi.mode(WIFI_STA)`: Configura el ESP32 como cliente Wi-Fi.
  * `WiFi.disconnect()`: Borra conexiones anteriores para forzar un inicio limpio.
  * `WiFi.begin(ssid, password)`: Arranca la conexión a la red Wi-Fi mediante el nombre y su contraseña.
  * `WiFi.status()`: Espera en un bucle a que la conexión sea correcta.
  * `WiFi.localIP()`: Obtiene la IP local asignada por la red.
  * `Ping.ping(remote_host)`: Envía paquetes ICMP (ping) a Google para comprobar la salida a Internet.

## Apartado 2: Sincronización NTP y Hora Local
* **Descripción:** Conexión a Wi-Fi y configuración del reloj interno del ESP32 mediante servidores NTP (`pool.ntp.org`), aplicando la zona horaria de España (CET/CEST) para obtener e imprimir la fecha y hora formateadas por el terminal serie.
* **Lógica y Funciones:**
  * `configTzTime(time_zone, ntpServer1, ntpServer2)`: Configura los servidores NTP y la cadena de zona horaria para la gestión automática de la hora.
  * `getLocalTime(&timeinfo, 10000)`: Consulta la hora del servidor NTP esperando un máximo de 10 segundos.
  * `strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo)`: Convierte la estructura de tiempo a la cadena legible `DD/MM/AAAA HH:MM:SS`.
  * `mostrarHoraLocal()`: Función auxiliar que valida la recepción de la fecha/hora y la imprime por consola.
 
## Apartados 4 y 5: Cliente Socket TCP y Control mediante comandos (Start/Stop)
* **Descripción:** Conexión del ESP32 a un servidor TCP externo mediante un cliente socket, permitiendo recibir comandos de control (`start` y `stop`) a través de la red para iniciar o detener la transmisión periódica (cada 1 segundo) de la hora local.
* **Lógica y Funciones:**
  * `WiFiClient cliente`: Crea el cliente socket para la gestión de la comunicación TCP.
  * `cliente.connect(direccionIP, puerto)`: Inicia o reconecta la comunicación TCP con la IP y puerto del servidor.
  * `cliente.available()`: Comprueba si no se ha desconectado el Socket y la conexión sigue disponible.
  * `cliente.read()`: Extrae y devuelve el siguiente carácter disponible en el búfer del Socket.
  * `millis()`: Gestiona el envío temporizado de la hora cada 1000 ms sin bloquear la ejecución del programa.
  * `cliente.println(buffer)`: Envía la cadena de fecha y hora (`DD/MM/AAAA HH:MM:SS`) a través del socket TCP.
* **Control por Comandos:** Al recibir `start` activa la bandera `mandandoHora` para transmitir datos, y al recibir `stop` la desactiva para pausar el envío.

## Apartado 6: Lectura de IMU por I2C y Envío por TCP con FreeRTOS
### 6.1. Nodo Maestro (ESP32)
* **Descripción:** Petición periódica de lecturas de aceleración al nodo esclavo por el bus I2C mediante tareas en FreeRTOS y envío de los datos cada segundo a través de un socket TCP con indicación por LED.
* **Lógica y Funciones:**
  * `struct lecturaIMU`: Estructura con las componentes flotantes de aceleración (`ax`, `ay`, `az`).
  * `Wire.requestFrom(DIRECCION_ESCLAVO, sizeof)`: Solicita los datos al dispositivo esclavo por I2C.
  * `Wire.readBytes(...)`: Guarda el bloque de bytes recibido en la estructura.
  * `xTaskCreate()`: Crea las tareas de FreeRTOS (`lecturaI2C` a prioridad 2 y `envioUART_LED` a prioridad 1).
  * `vTaskDelay()`: Pausa la lectura I2C 100 ms entre peticiones.
  * `vTaskDelayUntil()`: Asegura el envío por TCP con una periodicidad exacta de 1 segundo.
  * `cliente.printf(...)`: Transmite los valores del acelerómetro por el socket TCP.
  * `digitalWrite(PIN_LED, HIGH/LOW)`: Enciende un LED durante 200 ms en cada envío por red.

### 6.2. Nodo Esclavo (Arduino Nano 33 BLE)
* **Descripción:** Configuración de el Arduino Nano 33 BLE como nodo esclavo I2C (dirección `0x08`), muestreando continuamente la IMU integrada y respondiendo a las peticiones del maestro enviando la aceleración actual.
* **Lógica y Funciones:**
  * `Wire.begin(DIRECCION_I2C)`: Configura el microcontrolador como esclavo I2C bajo la dirección `0x08`.
  * `Wire.onRequest(atenderPeticion)`: Registra la función de interrupción (ISR) que responde cuando la ESP32 solicita datos.
  * `atenderPeticion()`: Rutina ISR que transmite la estructura con los 12 bytes de aceleración mediante `Wire.write()`.
  * `IMU.readAcceleration(x, y, z)`: Captura las componentes de la IMU en el bucle principal cada 50 ms.
  * `noInterrupts()` / `interrupts()`: Bloque de sección crítica que asegura la copia de datos a la variable `volatile` de forma que evita conflictos con la ISR.
  
## Apartado 7: Servidor Web Embedido con SPIFFS
* **Descripción:** Montaje de un servidor web asíncrono en el ESP32 para servir una interfaz HTML almacenada en la memoria Flash (SPIFFS), permitiendo consultar la hora actual en tiempo real y reiniciar el reloj a las 00:00:00 mediante peticiones HTTP.
* **Lógica y Funciones:**
  * `SPIFFS.begin(true)`: Inicializa el sistema de archivos en la memoria Flash (`index.html`).
  * `AsyncWebServer web(80)`: Instancia el servidor web en el puerto 80.
  * `obtenerHoraFormateada()`: Lee el tiempo del sistema y lo devuelve en formato de cadena `HH:MM:SS`.
  * `resetearReloj()`: Fuerza la hora interna del microcontrolador a las 00:00:00 mediante `settimeofday()`.
  * `web.on("/", HTTP_GET, ...)`: Sirve el archivo `/index.html` guardado en la memoria SPIFFS al acceder a la raíz.
  * `web.on("/hora", HTTP_GET, ...)`: Endpoint HTTP que responde con la hora formateada para la actualización de la página web.
  * `web.on("/reset", HTTP_POST, ...)`: Endpoint HTTP que recibe la orden de reinicio del reloj a las 00:00:00.
  * `web.begin()`: Inicia el servicio del servidor web en segundo plano.
 
## Apartado 8: Generación de Datos en Formato SenML (JSON)
* **Descripción:** Generación periódica (cada 10 segundos) de un paquete de datos JSON bajo el estándar SenML, registrando lecturas simuladas de temperatura, unidad de medida, timestamp UNIX y la dirección del ESP32 como identificador único.
* **Lógica y Funciones:**
  * `generarSenMLJSON()`: Construye y serializa el objeto JSON siguiendo la especificación SenML.
  * `JsonDocument doc` / `doc.add<JsonObject>()`: Crea el array raíz y añade el objeto de registro según exige el estándar.
  * **Campos SenML (`bn`, `bt`, `bu`, `n`, `v`):** Asigna el identificador (`bn`), tiempo base UNIX (`bt`), unidad en Celsius (`bu` = "Cel"), nombre de la variable (`n` = "temperature") y valor simulado (`v`).
  * `serializeJson(doc, jsonOutput)`: Convierte la estructura JSON en una cadena de texto para su envío o impresión.
  * `leerTemperaturaSimulada()`: Genera valores flotantes aleatorios de temperatura entre 20.0 y 30.0 °C.
  * `millis()`: Controla la generación del paquete cada 10 000 ms de manera no bloqueante.

## Apartado 10: Integración en la Nube con Adafruit IO (MQTT)

### 10.1. Publicación de Datos mediante MQTT
* **Descripción:** Conexión al broker MQTT de Adafruit IO y publicación periódica (cada 10 segundos) de datos simulados de temperatura en un feed específico.
* **Lógica y Funciones:**
  * `PubSubClient client(espClient)`: Instancia el cliente MQTT sobre la conexión TCP Wi-Fi.
  * `client.setServer(server, port)`: Define la dirección del broker (`io.adafruit.com`) y el puerto MQTT (1883).
  * `client.connect("ESP32S3Client", username, key)`: Autentica el cliente en Adafruit IO mediante usuario y clave de API.
  * `client.publish(feed, dato.c_str())`: Transmite el valor de temperatura formateado como cadena de texto al feed especificado.
  * `client.loop()`: Mantiene activa la sesión MQTT y procesa los eventos de red de fondo.
  * `creaTemperatura()`: Genera lecturas simuladas aleatorias de temperatura entre 20.0 y 30.0 °C.

### 10.2. Suscripción y Recepción de Datos
* **Descripción:** Suscripción al feed de Adafruit IO para recibir y procesar en tiempo real los datos o comandos publicados externamente (por ejemplo, desde la web).
* **Lógica y Funciones:**
  * `client.setCallback(callback)`: Asigna la función encargada de procesar las lecturas entrantes al recibir un mensaje del broker.
  * `client.subscribe(feed)`: Registra la suscripción del ESP32 al feed objetivo tras establecer la conexión MQTT.
  * `callback(topic, payload, length)`: Rutina que convierte el vector de bytes (`payload`) recibido a una cadena de texto y la imprime por el puerto serie.
  * `client.loop()`: Escucha de forma continua el socket para capturar y disparar la función `callback` ante nuevos mensajes.
