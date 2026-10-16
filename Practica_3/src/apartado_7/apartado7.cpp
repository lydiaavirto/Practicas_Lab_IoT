#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h> 

// Credenciales Wi-Fi
const char* ssid     = "HONOR de Lydia";
const char* password = "123456788";

// Servidores NTP y Zona Horaria (España - Península)
const char* ntpServer = "pool.ntp.org";
const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";

// Servidor Web 
WebServer server(80);

// Función para formatear la hora actual como String
String obtenerHoraFormateada() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 100)) {  // Coge la hora actual
    return "Error al obtener la hora";
  }
  char buffer[64];
  strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);
  return String(buffer);
}

// Ruta Raíz ('/'): Muestra la interfaz web HTML
void handleRoot() {  
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Servidor Web ESP32-S3</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; margin-top: 50px; }";
  html += "h1 { color: #333; }";
  html += ".clock { font-size: 2.5rem; color: #007bff; margin: 20px 0; font-weight: bold; }";
  html += ".btn { background-color: #dc3545; color: white; border: none; padding: 12px 24px; font-size: 1rem; cursor: pointer; border-radius: 5px; text-decoration: none; display: inline-block; }";
  html += ".btn:hover { background-color: #c82333; }";
  html += "</style>";
  // Script AJAX para actualizar la hora cada segundo en la pantalla sin refrescar la página
  html += "<script>";
  html += "setInterval(function() {";
  html += "  fetch('/time').then(response => response.text()).then(data => {";
  html += "    document.getElementById('reloj').innerText = data;";
  html += "  });";
  html += "}, 1000);";
  html += "</script>";
  html += "</head><body>";
  html += "<h1>ESP32-S3 - Servidor de Hora</h1>";
  html += "<div class='clock' id='reloj'>" + obtenerHoraFormateada() + "</div>";
  html += "<form action='/reset' method='POST'>";
  html += "<button class='btn' type='submit'>Resetear hora a las 0:00</button>";
  html += "</form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Ruta '/time': Devuelve únicamente el texto de la hora para las peticiones AJAX
void handleTime() {
  server.send(200, "text/plain", obtenerHoraFormateada());
}

// Ruta '/reset': Cambia la hora interna del sistema a las 00:00:00
void handleReset() {
  time_t now;
  time(&now);
  struct tm *timeinfo = localtime(&now);

  // Forzar horas, minutos y segundos a 0
  timeinfo->tm_hour = 0;
  timeinfo->tm_min  = 0;
  timeinfo->tm_sec  = 0;

  time_t t_reset = mktime(timeinfo);
  struct timeval tv = { .tv_sec = t_reset, .tv_usec = 0 };
  
  // Modificar la hora en el reloj interno usando la función declarada en <time.h>
  settimeofday(&tv, NULL);

  Serial.println("¡Hora reseteada manualmente a las 00:00:00!");

  // Redirigir de vuelta a la página principal
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1. Conexión Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Wi-Fi Conectado!");
  Serial.print("Dirección IP del servidor WEB: http://");
  Serial.println(WiFi.localIP());

  // 2. Configurar la hora con NTP
  configTzTime(time_zone, ntpServer);
  struct tm t_check;
  if (getLocalTime(&t_check, 10000)) {
    Serial.println("Hora NTP sincronizada con éxito.");
  }

  // 3. Definir rutas del servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/time", HTTP_GET, handleTime);
  server.on("/reset", HTTP_POST, handleReset);

  // Iniciar el servidor HTTP
  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  // Atender las peticiones web entrantes
  server.handleClient();
}