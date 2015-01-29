// **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//
// **********************************************************************************

#ifndef REMORA_h
#define REMORA_h

// Spark Core main firmware include file
#include "application.h"

// Définir ici le type de carte utilsé
//#define REMORA_BOARD_V10  // Version 1.0
//#define REMORA_BOARD_V11  // Version 1.1
#define REMORA_BOARD_V12  // Version 1.2

// Librairies du projet remora
#include "MCP23017.h"
#include "RFM69registers.h"
#include "RFM69.h"
#include "SSD1306.h"
#include "GFX.h"
#include "ULPN_RF_Protocol.h"
#include "TeleInfo.h"
//#include "OLED_local.h"
//#include "mfGFX_local.h"

// Includes du projets remora
#include "linkedlist.h"
#include "i2c.h"
#include "rfm.h"
#include "display.h"
#include "pilotes.h"
#include "tinfo.h"

//  Définir ici les modules utilisés sur la carte Remora
//
#define MOD_RF69      /* Module RF  */
#define MOD_OLED      /* Afficheur  */
#define MOD_TELEINFO  /* Teleinfo   */
//#define MOD_RF_OREGON   /* Reception des sondes orégon */

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
extern uint16_t status; // status global de l'application

// Function exported for other source file
// =======================================

#endif
