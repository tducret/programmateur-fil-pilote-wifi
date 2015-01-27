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

//  Définir ici les modules utilsés sur la carte Remora
//
#define MOD_RF69      /* Module RF  */
#define MOD_OLED      /* Afficheur  */
#define MOD_TELEINFO  /* Teleinfo   */
//#define MOD_RF_OREGON   /* Reception des sondes orégon */

// Spark Core main firmware include file
#include "application.h"

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

// Tarif en cours au format numérique
enum ptec_e { PTEC_HP, PTEC_HC };

#define LED_PIN     8
#define RELAIS_PIN  9

#define ISOUSCRITE 30
#define DELESTAGE_RATIO 0.9 //ratio en % => 90%
#define IMAX 35 // sera mis à jour à la reception de trame teleinfo

// Masque de bits pour le status global de l'application
#define STATUS_MCP    0x0001 // I/O expander detecté
#define STATUS_OLED   0x0002 // Oled detecté
#define STATUS_RFM    0x0004 // RFM69  detecté
#define STATUS_TINFO  0x0008 // Trame téléinfo detecté

// Variables exported to other source file
// ========================================
// define var for whole project
extern char myPeriode[];
extern unsigned int mypApp;
extern unsigned int myiInst;
extern unsigned int myindexHC;
extern unsigned int myindexHP;
extern unsigned int myisousc;
extern unsigned int etatrelais;
extern uint16_t status; // status global de l'application
extern ptec_e ptec;

// Function exported for other source file
// =======================================

#endif
