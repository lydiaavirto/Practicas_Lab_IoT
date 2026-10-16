#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Ping.h>

const char* ssid     = "HONOR de Lydia";
const char* password = "123456788";

const char* remote_host = "www.google.com";

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Limpiar conexiones previas
  delay(100);

  Serial.printf("Intentando conectar a: %s\n", ssid);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n¡Conectado con éxito!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nError de conexión. Código de estado:");
    Serial.println(WiFi.status()); 
    // 1 = No SSID avail, 4 = Connect failed, 5 = Connection lost, 6 = Wrong password
  }
Serial.print("Pinging host ");
  Serial.println(remote_host);

  if(Ping.ping(remote_host)) {
    Serial.println("Success!!");
  } else {
    Serial.println("Error :(");
  }

}

void loop() {}