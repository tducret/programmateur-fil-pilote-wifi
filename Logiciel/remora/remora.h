// **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//            15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//
// **********************************************************************************
#ifndef REMORA_h
#define REMORA_h

// Spark Core main firmware include file
#ifdef SPARK
#include "application.h"
#endif

// Définir ici le type de carte utilsé
//#define REMORA_BOARD_V10  // Version 1.0
//#define REMORA_BOARD_V11  // Version 1.1
#define REMORA_BOARD_V12  // Version 1.2

//  Définir ici les modules utilisés sur la carte Remora
#define MOD_RF69      /* Module RF  */
#define MOD_OLED      /* Afficheur  */
#define MOD_TELEINFO  /* Teleinfo   */
//#define MOD_RF_OREGON   /* Reception des sondes orégon */

// Librairies du projet remora Pour Particle
#ifdef SPARK
  #include "MCP23017.h"
  #include "SSD1306.h"
  #include "GFX.h"
  #include "ULPNode_RF_Protocol.h"
  #include "LibTeleinfo.h"
  #include "WebServer.h"

  #include "display.h"
  #include "i2c.h"
  #include "pilotes.h"
  #include "rfm.h"
  #include "tinfo.h"
  #include "linked_list.h"
  #include "route.h"
  #include "RadioHead.h"
  #include "RH_RF69.h"
  #include "RHDatagram.h"
  #include "RHReliableDatagram.h"

  //#include "OLED_local.h"
  //#include "mfGFX_local.h"

  #define _yield()  Particle.process()
#endif

// Librairies du projet remora Pour Particle
#ifdef ESP8266
  #if defined (REMORA_BOARD_V10) || defined (REMORA_BOARD_V11)
  #error "La version ESP8266 NodeMCU n'est pas compatible avec les cartes V1.1x"
  #endif

  // Définir ici les identifiants de
  // connexion à votre réseau Wifi
  #define DEFAULT_WIFI_SSID "************"
  #define DEFAULT_WIFI_PASS "************"
  #define DEFAULT_OTA_PORT  8266
  #define DEFAULT_HOSTNAME  "remora"
  #include "Arduino.h"
  #include "./MCP23017.h"
  //#include "./RFM69registers.h"
  //#include "./RFM69.h"
  #include "./SSD1306.h"
  #include "./GFX.h"
  #include "./ULPNode_RF_Protocol.h"
  #include "./LibTeleinfo.h"

  #define _yield()  yield()
#endif

// Includes du projets remora
#include "linked_list.h"
#include "i2c.h"
#include "rfm.h"
#include "display.h"
#include "pilotes.h"
#include "tinfo.h"

// RGB LED related MACROS
#if defined (SPARK)
  #define COLOR_RED     255,   0,   0
  #define COLOR_ORANGE  255, 127,   0
  #define COLOR_YELLOW  255, 255,   0
  #define COLOR_GREEN     0, 255,   0
  #define COLOR_CYAN      0, 255, 255
  #define COLOR_BLUE      0,   0, 255
  #define COLOR_MAGENTA 255,   0, 255

  #define LedRGBOFF() RGB.color(0,0,0)
  #define LedRGBON(x) RGB.color(x)

  // RFM69 Pin mapping
  #define RF69_CS  SS // default SPI SS Pin
  #define RF69_IRQ 2

#elif defined (ESP8266)
  #define COLOR_RED     rgb_brightness, 0, 0
  #define COLOR_ORANGE  rgb_brightness, rgb_brightness>>1, 0
  #define COLOR_YELLOW  rgb_brightness, rgb_brightness, 0
  #define COLOR_GREEN   0, rgb_brightness, 0
  #define COLOR_CYAN    0, rgb_brightness, rgb_brightness
  #define COLOR_BLUE    0, 0, rgb_brightness
  #define COLOR_MAGENTA rgb_brightness, 0, rgb_brightness

  // On ESP8266 we use NeopixelBus library to drive neopixel RGB LED
  //#define LedRGBOFF() { rgb_led.SetPixelColor(0,0,0,0); rgb_led.Show(); }
  //#define LedRGBON(x) { rgb_led.SetPixelColor(0,x); rgb_led.Show(); }
  #define LedRGBOFF() {}
  #define LedRGBON(x) {}

  // RFM69 Pin mapping
  #define RF69_CS   15
  #define RF69_IRQ  2
#endif

// Ces modules ne sont pas disponibles sur les carte 1.0 et 1.1
#if defined (REMORA_BOARD_V10) || defined (REMORA_BOARD_V11)
  #undef MOD_RF69
  #undef MOD_OLED
  #undef MOD_RF_OREGON

  // en revanche le relais l'est sur la carte 1.1
  #ifdef REMORA_BOARD_V11
    #define RELAIS_PIN A1
  #endif

  // Creation macro unique et indépendante du type de
  // carte pour le controle des I/O
  #define _digitalWrite(p,v)  digitalWrite(p,v)
  #define _pinMode(p,v)       pinMode(p,v)

// Carte 1.2+
#else
  #define LED_PIN    8
  #define RELAIS_PIN 9

  // Creation macro unique et indépendante du type de
  // carte pour le controle des I/O
  #define _digitalWrite(p,v)  mcp.digitalWrite(p,v)
  #define _pinMode(p,v)       mcp.pinMode(p,v)
#endif

// Masque de bits pour le status global de l'application
#define STATUS_MCP    0x0001 // I/O expander detecté
#define STATUS_OLED   0x0002 // Oled detecté
#define STATUS_RFM    0x0004 // RFM69  detecté
#define STATUS_TINFO  0x0008 // Trame téléinfo detecté

// Variables exported to other source file
// ========================================
// define var for whole project

// status global de l'application
extern uint16_t status;
extern unsigned long uptime ;


#ifdef SPARK
  // Particle WebServer
  //extern WebServer server("", 80);
#endif

#ifdef ESP8266
  // ESP8266 WebServer
  extern ESP8266WebServer server;
#endif


extern uint16_t status; // status global de l'application

// Function exported for other source file
// =======================================
char * timeAgo(unsigned long);

#endif
