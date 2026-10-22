#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Credenciales Wi-Fi
const char* ssid     = "HONOR de Lydia";
const char* password = "123456788";

// Credenciales Adafruit
const char* username     = "lydiaavirto";
const char* key = "aio_HDWO28RDZYBsHGsTeQVhly6OcT0r";
const char* server     = "io.adafruit.com";
const char* feed = "lydiaavirto/feeds/temperatura";
const int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long temp = 0.0;

// Función para simular lecturas de temperatura flotantes (entre 20.0 y 30.0 °C)
float creaTemperatura() {
  return 20.0 + (random(0, 100) / 10.0); 
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

  client.setServer(server, port);  
}

void loop() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT de Adafruit...");
    
    if (client.connect("ESP32S3Client", username, key)) {
      Serial.println(" ¡Conectado con éxito!");
    } else {
      Serial.print(" Falló la conexión, rc=");
      Serial.print(client.state());
      Serial.println(". Reintentando en 5 segundos...");
      delay(5000);
    }
  }
  client.loop();
  
  // Mandamos el dato
  temp = creaTemperatura();
  String dato = String(temp);
  client.publish("lydiaavirto/feeds/temperatura", dato.c_str()); //no usa string sino cadena estilo char*

  Serial.print("Publicando en Adafruit IO: ");
  Serial.println(dato);

  delay(10000);

}