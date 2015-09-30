// **********************************************************************************
// OLED display management header file for remora project
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-01-22 - First release
//
// 15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "remora.h"

#define OLED_I2C_ADDRESS    0x3C

// OLED Parameters
#define OLED_LINE_0 0
#define OLED_LINE_1 8
#define OLED_LINE_2 16
#define OLED_LINE_3 24
#define OLED_LINE_4 32
#define OLED_LINE_5 40
#define OLED_LINE_6 48
#define OLED_LINE_7 56
//#define OLED_CHAR_SIZE  5
#define OLED_CHAR_SIZE  7

#define OLED_SPACE1 OLED_CHAR_SIZE
#define OLED_SPACE2 OLED_CHAR_SIZE*2
#define OLED_SPACE3 OLED_CHAR_SIZE*3
#define OLED_SPACE4 OLED_CHAR_SIZE*4

// The various screen states supported
typedef enum
{
  screen_rf = 0,
  screen_sys,
  screen_teleinfo,
  screen_end
} screen_e;

// Variables exported for other source file
// ========================================
extern Adafruit_SSD1306 display; /* OLED */
extern const char * screen_name[] ;
extern screen_e screen_state ;

// Function exported for other source file
// =======================================
void display_splash();
bool display_setup();
void display_loop();

#endif
