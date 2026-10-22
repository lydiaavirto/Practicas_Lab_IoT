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

void callback(char *topic, byte *payload, unsigned int length){
  Serial.print("Temperatura recibida desde el navegador [");
  Serial.print(topic);
  Serial.print("]:");

String temp = "";
  for (int i = 0; i < length; i++)
  {
      temp.concat((char)payload[i]);
  }
  Serial.println(temp);
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
  client.setCallback(callback);
}

void loop() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT de Adafruit...");
    
    if (client.connect("ESP32S3Client", username, key)) {
      Serial.println(" ¡Conectado con éxito!");
      // Suscribirse al feed
      client.subscribe(feed);
      Serial.print("Escuchando publicaciones en: ");
      Serial.println(feed);
    } else {
      Serial.print(" Falló la conexión, rc=");
      Serial.print(client.state());
      Serial.println(". Reintentando en 5 segundos...");
      delay(5000);
    }
  }
  client.loop();
}