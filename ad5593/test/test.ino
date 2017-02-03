// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// AD5252
// This code is designed to work with the AD5252_I2CPOT_10K I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Potentiometers?sku=AD5252_I2CPOT_10K#tabs-0-product_tabset-2

#include<Wire.h>

// AD5252 I2C address is 0x2C(44)
#define Addr1 0x2C
#define Addr2 0x2F

#define EEReg 0x2F
#define RDAC1Reg 0x01
#define RDAC3Reg 0x03

#define TolReg 0x3B

#define low 20
#define high 200
byte count = 20;

void setReg(byte address, byte reg, byte val)
{
  // Start I2C transmission
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}


void getReg(byte address, byte reg, int nBytes, byte* data)
{
  Wire.beginTransmission(address);   // Start I2C transmission
  Wire.write(EEReg);   // Select data register
  Wire.endTransmission();   // Stop I2C transmission
  Wire.requestFrom(address, nBytes);  // Request 1 byte of data
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
  Serial.print("Setting to ");
  Serial.println(count);
  setReg(Addr1, RDAC1Reg, count);
  setReg(Addr1, RDAC3Reg, count);
  setReg(Addr1, EEReg, count);
  //count += 255;
  //if (count >= 256) count = 0;
  if (count == low) count = high;
  else if (count == high) count = low;
  delay(100);

  // read back
  byte data;
  getReg(Addr1, EEReg, 1, &data);
  //getReg(Addr1, TolReg, 1, &data);


  Serial.print("EEREG and RDAC: ");
  Serial.println(data);
  //Serial.println(" K");
  delay(5000);
}
