#include <Wire.h>

void setup() {
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);             // Leonardo: wait for serial monitor

}

void loop()
{
  // CMD/REG = 0
  // EE/RDAC = 1
  // A4 = 1
  // A3 = 1
  // A2 = 0
  // A1 = 1
  // A0 = 1
  uint8_t address = 0x2f;
  byte error;

  for (uint8_t i=0; i < 256; i++)
  {
    // set data
    Serial.print("Setting EEMEM15 to ");
    Serial.println(i);
    
    Wire.beginTransmission(address);
    //Wire.write((uint8_t)0x2f);
    //Wire.write((uint8_t)i);
    
    Wire.write(0x01);
    // Input resistance value, 0x80(128)
    Wire.write(0x80);
    
    error = Wire.endTransmission();

    Serial.print("Return code ");
    Serial.println(error);

    byte result;
    Serial.print("Reading EEMEM15: ");


    // read the result back
    // set pointer
    Wire.beginTransmission(address);
    Wire.write((uint8_t)0x2f);
    //Wire.endTransmission();

    // read back
    Wire.requestFrom((uint8_t)0x2f,1);
    result = Wire.read();
    
    Serial.println(result);

    Serial.print("Return code ");
    Serial.println(error);
    
    delay(1000);           // wait 5 seconds for next scan
  }
}
