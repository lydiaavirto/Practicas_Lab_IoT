#include <Arduino.h>
#include <Wire.h>

const uint8_t DIRECCION_I2C = 0x08;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Wire.begin(); // Inicializa I2C como Master (SDA=A4, SCL=A5)
}

void loop() {
  // Enviar comando para encender el LED ('1')
  Wire.beginTransmission(DIRECCION_I2C);  // tramsnite al ESP por la direccion 0x08
  Wire.write('1');  // manda el valor para encender el led
  Wire.endTransmission();   // deja de transmitir

  delay(2000);

  // Enviar comando para apagar el LED ('0')
  Wire.beginTransmission(DIRECCION_I2C);   // tramsnite al ESP por la direccion 0x08
  Wire.write('0');   // manda el valor para apagar el led
  Wire.endTransmission();    // deja de transmitir

  delay(2000);

  // sacado de: https://docs.arduino.cc/tutorials/nano-33-ble-sense/i2c/
}