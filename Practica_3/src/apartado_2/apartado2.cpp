#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

// Credenciales Wi-Fi
const char* ssid     = "HONOR de Lydia";
const char* password = "123456788";

// Servidores NTP y Zona Horaria (España - Península)
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const char* time_zone  = "CET-1CEST,M3.5.0,M10.5.0/3"; 

void mostrarHoraLocal() {
  struct tm timeinfo;

  // Espera hasta 10000 ms (10 segundos) a que el servidor NTP responda
  if (!getLocalTime(&timeinfo, 10000)) {
    Serial.println("Error: No se pudo obtener la hora del servidor NTP.");
    return;
  }

  // Imprimir fecha y hora formateada: DD/MM/AAAA HH:MM:SS
  char buffer[64];
  strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);
  Serial.print("Fecha y hora actualizadas: ");
  Serial.println(buffer);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.printf("Conectando a: %s\n", ssid);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n¡Conectado con éxito!");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());

    // Sincronización NTP con zona horaria aplicada directamente
    Serial.println("\nConfigurando hora NTP...");
    configTzTime(time_zone, ntpServer1, ntpServer2);

    mostrarHoraLocal();
  } else {
    Serial.println("\nError: No se pudo conectar a la red Wi-Fi.");
  }
}

void loop() {
  // Bucle principal
}