#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <time.h>

// Credenciales Wi-Fi
const char* ssid     = "HONOR de Lydia";
const char* password = "123456788";

// Servidor NTP y Zona Horaria
const char* ntpServer = "pool.ntp.org";
const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";

// Control de tiempo para generar el JSON cada 10 segundos
unsigned long previousMillis = 0;
const unsigned long interval = 10000; // 10.000 ms = 10 s

// Función para simular lecturas de temperatura flotantes (entre 20.0 y 30.0 °C)
float leerTemperaturaSimulada() {
  return 20.0 + (random(0, 100) / 10.0); 
}

// Función que construye el paquete JSON siguiendo el estándar SenML (RFC 8428)
String generarSenMLJSON() {
  // 1. Obtener la marca de tiempo Epoch UNIX actual
  time_t now = time(NULL);

  // 2. Crear el documento JSON usando ArduinoJson v6/v7
  JsonDocument doc;

  // SenML requiere que la raíz sea un Array [...]
  JsonObject record = doc.add<JsonObject>();

  // Definición de campos SenML
  record["bn"] = "urn:dev:mac:" + WiFi.macAddress(); // Base Name (Identificador único)
  record["bt"] = (long)now;                          // Base Time (Timestamp UNIX en segundos)
  record["bu"] = "Cel";                              // Base Unit (UCUM: "Cel" = Celsius)
  record["n"]  = "temperature";                      // Name
  record["v"]  = leerTemperaturaSimulada();          // Value

  // 3. Serializar (convertir el documento a texto JSON)
  String jsonOutput;
  serializeJson(doc, jsonOutput);

  return jsonOutput;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Wi-Fi Conectado!");

  // Obtener la hora NTP para la marca temporal real
  configTzTime(time_zone, ntpServer);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10000)) {
    Serial.println("Hora NTP sincronizada.");
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Ejecutar exactamente cada 10 segundos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Generar el JSON SenML
    String senmlJson = generarSenMLJSON();

    // Imprimir el resultado por el puerto serie
    Serial.println("\n--- Nuevo Paquete SenML (10s) ---");
    Serial.println(senmlJson);
  }
}