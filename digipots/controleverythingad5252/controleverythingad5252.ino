// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// AD5252
// This code is designed to work with the AD5252_I2CPOT_10K I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Potentiometers?sku=AD5252_I2CPOT_10K#tabs-0-product_tabset-2

#include<Wire.h>

// AD5252 I2C address is 0x2C(44)
#define Addr 0x2C

#define EEReg 0x2F
#define RDAC1Reg 0x01
#define RDAC3Reg 0x03

#define TolReg 0x3B

byte count = 0;

void setReg(byte reg, byte val)
{
  // Start I2C transmission
  Wire.beginTransmission(Addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}


void getReg(byte reg, int nBytes, byte* data)
{
  Wire.beginTransmission(Addr);   // Start I2C transmission
  Wire.write(EEReg);   // Select data register
  Wire.endTransmission();   // Stop I2C transmission
  Wire.requestFrom(Addr, nBytes);  // Request 1 byte of data
  int count = 0;
  while (Wire.available() == 1)  // Read 1 byte of data
  {
    byte tByte = Wire.read();
    data[count] = tByte;
    if (count >= nBytes) break;
  }
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
}

void loop()
{
  
  // set registers
  setReg(RDAC1Reg, count);
  setReg(RDAC3Reg, count);
  setReg(EEReg, count);
  //count += 255;
  //if (count >= 256) count = 0;
  if (count == 0) count = 255;
  else if (count == 255) count = 0;
  delay(100);

  // read back
  byte data;
  getReg(EEReg, 1, &data);


  Serial.print("EEREG and RDAC: ");
  Serial.println(data);
  //Serial.println(" K");
  delay(5000);
}
