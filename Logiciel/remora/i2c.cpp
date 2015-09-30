// **********************************************************************************
// I2C management source file for remora project
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-01-22 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "i2c.h"

/* ======================================================================
Function: i2c_init
Purpose : initialize the I2C function and setup
Input   : -
Output  : -
Comments: -
====================================================================== */
bool i2c_init(void)
{
  // Set i2C speed
  #if defined (SPARK)
    Wire.setSpeed(CLOCK_SPEED_400KHZ);
  #elif defined (ESP8266)
    // Sepecific ESP8266 to set I2C Speed
    Wire.setClock(400000);
  #endif
  
  Wire.begin();
}

/* ======================================================================
Function: i2c_detect
Purpose : check that a adressed device respond
Input   : I2C device address
Output  : true is seen (ACKed device) false otherwise
Comments: i2c_init should have been called before
====================================================================== */
bool i2c_detect(uint8_t _address)
{
  Wire.beginTransmission(_address);
  return (Wire.endTransmission() == 0 ? true : false);
}

/* ======================================================================
Function: i2c_scan
Purpose : scan I2C bus and display result
Input   : address wanted to search (0xFF)
Output  : true if I2C device found at address given
Comments: mostly used for debug purpose
====================================================================== */
uint8_t i2c_scan()
{
  byte error, address;
  uint8_t nDevices = 0;

  unsigned long start = millis();

  Serial.println(F("Scanning I2C bus ..."));

  // slow down i2C speed in case of slow device
  #if defined (SPARK)
    Wire.setSpeed(CLOCK_SPEED_100KHZ);
  #elif defined (ESP8266)
    // Sepecific ESP8266 to set I2C Speed
    Wire.setClock(100000);
  #endif

  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print(F("I2C device found at address 0x"));
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);

      if (address>=0x20 && address<=0x27)
        Serial.println("-> MCP23017 !");
      else if (address==0x3C || address==0x3D)
        Serial.println("-> OLED !");
      else if (address==0x29 || address==0x39 || address==0x49)
        Serial.println("-> TSL2561 !");
      else
        Serial.println("-> Unknown device !");

      nDevices++;
    }
  }

  Serial.print(nDevices);
  Serial.print(F(" I2C devices found, scan took "));
  Serial.print(millis()-start);
  Serial.println(" ms");

  // Get back to full speed
  // slow down i2C speed in case of slow device
  #if defined (SPARK)
    Wire.setSpeed(CLOCK_SPEED_400KHZ);
  #elif defined (ESP8266)
    // Sepecific ESP8266 to set I2C Speed
    Wire.setClock(400000);
  #endif

  return (nDevices);
}
