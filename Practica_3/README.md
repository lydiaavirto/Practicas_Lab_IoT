# Práctica 3: Comunicaciones Wi-Fi con ESP32

---

## Descripción General

El propósito global de esta práctica es diseñar, implementar y validar comunicaciones inalámbricas a bajo nivel (sockets TCP/IP, cliente NTP y comandos ICMP ping) hasta la implementación de protocolos de nivel de aplicación (servidor Web HTTP embebido y HTTP POST y MQTT) con ESP32.

## ¿Cómo se usa este código?

Cada apartado se gestiona de la siguiente forma:

1. **Configuración del entorno:**
   * Selecciona el entorno `env:esp32` (o `env:esp32dev`) en la barra inferior de PlatformIO.
   * Modifica en el archivo `platformio.ini` la propiedad `build_src_filter` indicando la ruta del apartado que deseas probar (ej. `+<apartado_1/apartado1.cpp>`).
2. **Carga y Monitoreo:**
   * Compila y sube el firmware a la placa ESP32.
   * Abre la consola serie configurada a **115200 baudios**.

## Estructura del Proyecto

```text
.
├── src/
│   ├── apartado_1/       # Conexión Wi-Fi STA y verificación ICMP Ping
│   ├── apartado_2/       # Cliente TCP Socket y sincronización NTP
│   ├── apartado_3/       # Envío de datos del acelerómetro sobre TCP
│   ├── apartado_4/       # Servidor Web embebido (HTTP)
│   ├── apartado_5/       # Generación SenML (JSON) y cliente FTP
│   └── apartado_6/       # Integración Cloud Adafruit IO (HTTP POST / MQTT)
├── platformio.ini        # Configuración de entornos y build_src_filter
└── README.md              # Documentación principal del proyecto
```

---

## Apartado 1: Conexión Wi-Fi y Verificación de Conectividad (Ping)

* **Descripción:** Inicialización de la interfaz inalámbrica del ESP32 en modo Estación (STA), conexión a un punto de acceso Wi-Fi local mediante asignación dinámica de dirección IPv4 por DHCP y comprobación de la conectividad hacia Internet mediante el envío de paquetes ICMP (ping) al dominio `www.google.com`.

* **Lógica y Funciones:**
  * `WiFi.mode(WIFI_STA)`: Configura la radio de la ESP32 para actuar como cliente (Estación) en lugar de punto de acceso.
  * `WiFi.disconnect()`: Descarta conexiones o credenciales previas almacenadas en la memoria NVS para forzar un arranque limpio.
  * `WiFi.begin(ssid, password)`: Inicia la fase de asociación y autenticación con el punto de acceso configurado.
  * `WiFi.status()`: Comprueba de forma periódica mediante un bucle de *polling* (con un límite de 30 intentos y pausas de $500\text{ ms}$) si la interfaz ha alcanzado el estado `WL_CONNECTED`.
  * `WiFi.localIP()`: Extrae la dirección IP asignada a la placa tras completar la negociación DHCP.
  * `Ping.ping(remote_host)`: Utiliza la librería `<ESP32Ping.h>` para enviar peticiones de eco ICMP hacia `www.google.com`, lo que permite validar tanto la resolución de nombres DNS como la salida bidireccional a la WAN.
