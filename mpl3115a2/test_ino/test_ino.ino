// Code to get MPL3115A2 working without using provided library code

#include <Wire.h>

void setup()
{
  Wire.begin(); 
  Serial.begin(9600);

  // Setup barometer
  //byte tempSetting = IIC_Read(0x26);
  //tempSetting &= ~(1<<7);
  //IIC_Write(0x26, tempSetting);

  //Setup altimeter
  byte tempSetting = IIC_Read(0x26);
  tempSetting |= (1<<7);
  IIC_Write(0x26, tempSetting);

  // Setup oversample
  byte sampleRate = 7;
  sampleRate <<= 3;

  tempSetting = IIC_Read(0x26);
  tempSetting &= B11000111;
  tempSetting |= sampleRate;
  IIC_Write(0x26, tempSetting);

  //Enable event flags
  IIC_Write(0x13, 0x07);
}

void loop()
{
  //float pressure = readPressure();
  //Serial.print("Pressure: ");
  //Serial.print(pressure, 2);

  float altitude = readAltitude();
  Serial.print("Altitude: ");
  Serial.print(altitude, 2);

  float temp = readTemp();
  Serial.print("\tTemp: ");
  Serial.print(temp, 2);

  Serial.println();

  delay(5000);

}

float readAltitude()
{
  toggle();
  int counter = 0;

  while((IIC_Read(0x00) & (1<<1)) == 0)
  {
    if(++counter > 600)
      return -999;
    delay(1);
  } 

  Wire.beginTransmission(0x60);
  Wire.write(0x01);
  Wire.endTransmission(false);

  if(Wire.requestFrom(0x60, 3) != 3)
  { 
    return -999;
  }

  byte msb, csb, lsb;
  msb = Wire.read();
  csb = Wire.read();
  lsb = Wire.read();

  float tempcsb = (lsb>>4)/16.0;

  float altitude = (float)((msb << 8) | csb) + tempcsb;

  return altitude;
}

float readPressure()
{
  if(IIC_Read(0x00) & (1<<2) == 0)
    toggle(); 

  int counter = 0;
  while(IIC_Read(0x00) & (1<<2) == 0)
  {
    if(++counter > 600) 
      return -999; 
    delay(1);
  }

  Wire.beginTransmission(0x60);
  Wire.write(0x01);
  Wire.endTransmission(false);

  if(Wire.requestFrom(0x60, 3) != 3) 
  {
    return -999; 
  }

  byte msb, csb, lsb;
  msb = Wire.read();
  csb = Wire.read();
  lsb = Wire.read();

  toggle();
  long pressure_whole = (long)msb<<16 | (long)csb<<8 | (long)lsb;
  pressure_whole >>= 6;

  lsb &= B00110000;
  lsb >>= 4;

  float pressure_decimal = (float)lsb/4.0;

  float pressure = (float)pressure_whole + pressure_decimal;

  return pressure;
}

float readTemp()
{
  if(IIC_Read(0x00) & (1<<1) == 0)
    toggle();

  int counter = 0;
  while((IIC_Read(0x00) & (1<<1)) == 0)
  {
    if(++counter > 600)
      return -999;
    delay(1);
  } 

  Wire.beginTransmission(0x60);
  Wire.write(0x04);
  Wire.endTransmission(false);

  if(Wire.requestFrom(0x60,2) != 2)
  {
    return -999; 
  }
  byte msb, lsb;
  msb = Wire.read();
  lsb = Wire.read();

  toggle();

  word foo = 0;
  bool negSign = false;

  if(msb > 0x7F)
  {
    foo = ~((msb << 8) + lsb) +1;
    msb = foo >> 8;
    lsb = foo & 0x00F0;
    negSign = true; 
  }

  float templsb = (lsb>>4)/16.0;

  float temp = (float)(msb + templsb);
  if(negSign)
    temp = 0 - temp;

  return temp;
}

void toggle()
{
  byte tempSetting = IIC_Read(0x26);
  tempSetting &= ~(1<<1);
  IIC_Write(0x26, tempSetting);

  tempSetting = IIC_Read(0x26);
  tempSetting |= (1<<1);
  IIC_Write(0x26, tempSetting); 
}

void IIC_Write(byte regAddress, byte value)
{
  // 
  Wire.beginTransmission(0x60);
  Wire.write(regAddress);
  Wire.write(value);
  byte result = Wire.endTransmission(true);

  if(!result)
  {
    return;
  }
  else
  {
    Serial.print("Error stopping!\n");
    Wire.endTransmission(true);
  }    
}

byte IIC_Read(byte regAddress)
{
  Wire.beginTransmission(0x60);
  Wire.write(regAddress);
  Wire.endTransmission(false);
  Wire.requestFrom(0x60, 1);
  return Wire.read();
}


