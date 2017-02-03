// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// AD5252
// This code is designed to work with the AD5252_I2CPOT_10K I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Potentiometers?sku=AD5252_I2CPOT_10K#tabs-0-product_tabset-2

#include<Wire.h>

// AD5252 I2C address is 0x2C(44)
#define Addr 0x11

// mode bits D7:D4
#define mConfig     0b00000000
#define mDacWrite   0b00010000
#define mAdcRead    0b01000000
#define mDacRead    0b01010000
#define mGpioRead   0b01100000
#define mRegRead    0b01110000

// pointer values D3:D0
#define pAdcSequence    0b0010
#define pGeneralPurpose 0b0011
#define pAdcPinConfig   0b0100
#define pDacPinConfig   0b0101
#define pPulldownConfig 0b0110

// general-purpose control register bits
#define gpAdcPrecharge   0b1000000000
#define gpAdcBuffer       0b100000000
#define gpLock             0b10000000
#define gpWriteAllDacs      0b1000000
#define gpAdcRange           0b100000
#define gpDacRange            0b10000

// I/O pin configuration registers
#define pin7 0b10000000
#define pin6 0b01000000
#define pin5 0b00100000
#define pin4 0b00010000
#define pin3 0b00001000
#define pin2 0b00000100
#define pin1 0b00000010
#define pin0 0b00000001

// adc sequence register
#define adcRep   0b1000000000
#define adcTemp   0b100000000

// adc data mask
#define adcFromAdc 0b1000000000000000
#define adcChannel 0b0111000000000000
#define adcValue   0b0000111111111111


void setReg(byte address, byte reg, int nBytes, byte* val)
{
  // Start I2C transmission
  Wire.beginTransmission(address);
  Wire.write(reg);
  for (int i=0; i<nBytes; i++)
  {
    Wire.write(val[i]);
  }
  Wire.endTransmission();
}

void setReg(byte address, byte reg, byte val)
{
  // Start I2C transmission
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void setReg(byte address, byte reg, uint16_t val)
{
  // Start I2C transmission
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(val >> 8);  // msb
  Wire.write(val & 255); // lsb
  Wire.endTransmission();
}

// format the data from the adcRead
// returns true if it was from the adc
bool formatAdcData(uint16_t data, byte* channel, uint16_t* value)
{
  bool fromAdc = (data & adcFromAdc) == 0;
  channel = (data & adcChannel) >> 12;
  value = (data & adcValue);
}

// read from register
// returns number of bytes read
int getReg(byte address, byte reg, int nBytes, byte* data)
{
  Wire.beginTransmission(address);   // Start I2C transmission
  Wire.write(reg);   // Select data register
  Wire.endTransmission();   // Stop I2C transmission
  Wire.requestFrom(address, nBytes);  // Request 1 byte of data
  int count = 0;
  int c = 0;
  while (Wire.available() == 1)  // Read 1 byte of data
  {
    byte tByte = Wire.read();
    data[count] = tByte;
    c += 1;
    if (count >= nBytes) break;
  }
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  /*
  // set up adc sequence
  Serial.print("Setting Adc pin config...");
  uint16_t val = pin2;
  Serial.print(mConfig | pAdcPinConfig, BIN);
  Serial.print(" ");
  Serial.println(val, BIN);
  setReg(Addr, mConfig | pAdcPinConfig, val);
  Serial.println("Done.");
  
  Serial.println("Setting Adc sequence...");
  val = pin2;
  Serial.print(mConfig | pAdcSequence, BIN);
  Serial.print(" ");
  Serial.println(val, BIN);
  setReg(Addr, mConfig | pAdcSequence, val);
  Serial.println("Done.");
  */
  /*
  Wire.beginTransmission(Addr);
  Wire.write(0b100);
  Wire.write(0b00000000);
  Wire.write(0b10000001);
  Wire.endTransmission();
  */
  uint16_t val = pin7 | pin0;
  setReg(Addr, mConfig | pAdcSequence, val); 
  

  Wire.beginTransmission(Addr);
  Wire.write(0b10);
  Wire.write(0b10);
  Wire.write(0b10000001);
  Wire.endTransmission();
}

void loop()
{
  int nBytes = 2;
  
  // read the ADC
  byte *data = new byte[nBytes];
  //Serial.print("Reading Adc...");
  //getReg(Addr, mAdcRead, nBytes, data);
  //Serial.println("Done.");

  Wire.beginTransmission(Addr);
  Wire.write(mAdcRead);
  Wire.endTransmission();
  Wire.requestFrom(Addr, 2);
  byte msb = Wire.read();
  byte lsb = Wire.read();

  Serial.print(msb, BIN);
  Serial.print(" ");
  Serial.println(lsb, BIN);

  /*
  Serial.println("Data: ");
  for (int i=0; i<(nBytes/2); i++)
  {
    uint16_t dpart = (data[i] << 8) + data[i+1];
    Serial.print(B0);
    Serial.print(data[i], BIN);
    Serial.print(" ");
    Serial.println(data[i+1], BIN);

    byte channel = 0;
    uint16_t value = 0;
    bool fromAdc = formatAdcData(dpart, &channel, &value);
    Serial.print("Channel ");
    Serial.print(channel);
    Serial.print(": ");
    Serial.print(value);
    Serial.print(" (");
    Serial.print(fromAdc);
    Serial.println(")");
  }
  Serial.println("");
  */

  delay(2000);
}
