// Code to get MPL3115A2 working without using provided library code

#include <Wire.h>

void setup()
{
   Wire.begin(); 
   Serial.begin(9600);
}

void loop()
{
    
  
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
