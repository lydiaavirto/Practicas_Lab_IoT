#include <Arduino.h>


const uint8_t PIN_LED = 10;
bool estado = 0;

void Tarea1( void * parameter )
{
  for(;;){
    Serial.println("Hola mundo");
    delay(1000);
  }
}

void Tarea2( void * parameter)
{
  for(;;){
    estado = !estado;
    digitalWrite(PIN_LED, estado);
    delay(200);
  }
}

void setup() {

  Serial.begin(112500);
  delay(1000);

  xTaskCreate(Tarea1,"Tarea1",10000,NULL,1,NULL);
  xTaskCreate(Tarea2,"Tarea2",10000,NULL,1,NULL);

  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  delay(1000);
}

