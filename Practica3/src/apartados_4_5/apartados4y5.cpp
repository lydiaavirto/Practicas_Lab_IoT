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

// Direccion y puerto del socket
const char* direccionIP = "10.162.54.145";
const uint16_t puerto = 21;

String comando = "";

bool mandandoHora = false;

WiFiClient cliente;

// Control de tiempo 
unsigned long inicio = 0;
const unsigned long final = 1000;

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

  Serial.println("\n Wifi conectado");

  // Sincronización NTP con zona horaria aplicada directamente
    
  configTzTime(time_zone, ntpServer1, ntpServer2);
  struct tm timeinfo;

  // Espera hasta 10000 ms (10 segundos) a que el servidor NTP responda
  if (getLocalTime(&timeinfo, 10000)) {
    Serial.println("Hora sincronizada");
  }

  // Conexión con el socket
  if(cliente.connect(direccionIP, puerto)){
    Serial.println("Exito");
    cliente.println("Exito");
  }
  else{
    Serial.println("Error");
  }
  
}

void loop() {
    if(!cliente.connected()){
        if(cliente.connect(direccionIP, puerto)){
            Serial.println("Reconectando");
        }
    }
    while(cliente.available()){
        char c = cliente.read();
        if(c == '\r' || c == '\n'){
            if(comando=="start"){
                mandandoHora = true;
                Serial.println("Mandando hora... ");
                cliente.println("Mandando hora... ");
            }
            else if(comando=="stop"){
                mandandoHora = false;
                cliente.println("Envio detenido");
                Serial.println("Envio detenido");
            }
            else if(comando.length()>0){
                cliente.println("Comando no reconocido");
                Serial.println("Comando no reconocido");
            }
            comando = "";
        }
        else{
            comando += c;
        }
    }
  
  if(mandandoHora){
    unsigned long actualMillis = millis();

    if(actualMillis-inicio>=final){
        inicio = actualMillis;

        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 100)) {
        // Imprimir fecha y hora formateada en el socket: DD/MM/AAAA HH:MM:SS
        char buffer[64];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);

        cliente.println(buffer); // para ver desde el socket
        Serial.println(buffer);  // para ver desde el terminal que lo hace bien
        }
    }
  }
}