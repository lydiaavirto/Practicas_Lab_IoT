#include <Arduino.h>
#include <ArduinoBLE.h>

// UUID con mi nombre
const char* uuid = "4C594449-4100-0000-0000-000000000000";

BLEService servicio =(uuid);

void setup(){
  Serial.begin(15200);

  if(!BLE.begin()){ // Enciende el modulo BLE
    Serial.println("Error al inicial el BLE");
    while(1);
  }
  
  BLE.setLocalName("NanoBLE_Lydia");
  BLE.setAdvertisedService(servicio);
  BLE.addService(servicio);
  BLE.advertise(); // activa el proceso de advertising

  Serial.println("Advertising con UUID personalizado iniciado.");
  Serial.print("UUID emitido: ");
  Serial.println(uuid);
}

void loop(){

}