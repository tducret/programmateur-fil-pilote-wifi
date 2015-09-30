// **********************************************************************************
// I2C management header file for remora project
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
#ifndef I2C_h
#define I2C_h

#include "remora.h"

#ifdef ESP8266
#include <Wire.h>
#endif

// Variables exported to other source file
// ========================================
// define RF var for whole project

// Function exported for other source file
// =======================================
bool    i2c_init(void);
bool    i2c_detect(uint8_t);
uint8_t i2c_scan(void);

#endif
