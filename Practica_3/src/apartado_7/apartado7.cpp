#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

// Credenciales Wi-Fi
const char* ssid     = "NOMBRE WIFI";
const char* password = "CONTRASE�A";

// Servidores NTP y Zona Horaria (España - Península)
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";

// Servidor web
AsyncWebServer web(80);

// Función para obtener la hora formateada utilizando time.h y strftime
String obtenerHoraFormateada() {
  time_t now;
  time(&now); // Lee el tiempo Unix actual en microsegundos
  
  struct tm timeinfo;
  localtime_r(&now, &timeinfo); // Aplica la zona horaria configurada (time_zone)

  char buffer[10];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  return String(buffer);
}

// Poner el reloj a 00:00:00
void resetearReloj(){
  struct timeval tv = {82800, 0};
  settimeofday(&tv, NULL);
}

void setup() {
  Serial.begin(115200);

  // Memoria Flash del micro
  if (!SPIFFS.begin(true)) {
    Serial.println("¡Error al montar SPIFFS!");
    return;
  }

  // Conectar a la red Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
  Serial.print("Dirección IP: http://");
  Serial.println(WiFi.localIP());

  // Sincronizar la hora
  configTzTime(time_zone, ntpServer1, ntpServer2);
  Serial.println("Sincronizando hora con servidores NTP...");

  // Por si no coge la hora
  struct tm timeinfo;
  int reintentos = 0;
  while (!getLocalTime(&timeinfo) && reintentos < 10) {
    delay(500);
    reintentos++;
  }
  
  if (reintentos < 10) {
    Serial.println("-> Hora NTP sincronizada correctamente.");
  }

  // Se abre la pagina con la interfaz del reloj y el boton reset
  web.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    configTzTime(time_zone, ntpServer1, ntpServer2); // Vuelve a pedir la hora 
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Se manda la hora al servidor
  web.on("/hora", HTTP_GET, [](AsyncWebServerRequest *request){  
  request->send(200, "text/plain", obtenerHoraFormateada());
  });

  // Mandamos el reset
  web.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request){
    resetearReloj();
    request->send(200, "text/plain", "OK");
  });

  // Arrancar el servidor
  web.begin();
  Serial.println("Servidor Web iniciado.");
}

void loop() {
  
  
}