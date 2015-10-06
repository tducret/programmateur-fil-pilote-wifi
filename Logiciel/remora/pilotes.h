// **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//
// **********************************************************************************

#ifndef PILOTES_h
#define PILOTES_h

#include "remora.h"

// Fils pilotes
#define NB_FILS_PILOTES 7

// Cartes Version 1.0 et 1.1
#if defined (REMORA_BOARD_V10) || defined (REMORA_BOARD_V11)
  // Les fils pilotes sont connectés de la façon suivante sur le core
  // # Fil Pilote    Digital Port
  // FP1 (a et b)  ->  D6,D7
  // FP2 (a et b)  ->  D4/D5
  // FP3 (a et b)  ->  D2/D3
  // FP4 (a et b)  ->  A6/A7
  // FP5 (a et b)  ->  A4/A5
  // FP6 (a et b)  ->  A2/A3
  // FP7 (a et b)  ->  A0,A1
  // Relais        ->  A1 (attention partagé avec FP7)
  #define FP1 D6,D7
  #define FP2 D4,D5
  #define FP3 D2,D3
  #define FP4 A6,A7
  #define FP5 A4,A5
  #define FP6 A2,A3
  #if defined (REMORA_BOARD_V10)
  #define FP7 A0,A1
  #else
  #undef NB_FILS_PILOTES
  #define NB_FILS_PILOTES 6
  #endif
#else
  // Les fils pilotes sont connectés de la façon suivante sur l'I/O expander
  // # Fil Pilote    MCP IO/Port   Digital Port
  // FP1 (a et b) -> GPB6, GPB7  ->  14/15
  // FP2 (a et b) -> GPB4, GPB5  ->  12/13
  // FP3 (a et b) -> GPB2, GPB3  ->  10/11
  // FP4 (a et b) -> GPA1, GPA0  ->  1/0
  // FP5 (a et b) -> GPA3, GPA2  ->  3/2
  // FP6 (a et b) -> GPA5, GPA4  ->  5/4
  // FP7 (a et b) -> GPA7, GPA6  ->  7/6
  // Relais       -> GPB1        ->  9
  // LED          -> GPB0        ->  8
  #define FP1 14,15
  #define FP2 12,13
  #define FP3 10,11
  #define FP4  1,0
  #define FP5  3,2
  #define FP6  5,4
  #define FP7  7,6

  #define RELAIS_PIN  9
  #define LED_PIN     8
#endif

// Variables exported to other source file
// ========================================
extern Adafruit_MCP23017 mcp;
extern int SortiesFP[];
extern char etatFP[];
extern char memFP[];
extern int nivDelest;
extern uint8_t plusAncienneZoneDelestee;
extern unsigned long timerDelestRelest;

// Function exported for other source file
// =======================================
bool pilotes_setup(void);
bool pilotes_loop(void);
void delester1zone(void);
void relester1zone(void);
void decalerDelestage(void);
void initFP(void);
int setfp(String);
int setfp_interne(uint8_t fp, char cOrdre);
int fp(String);
int relais(String);

#endif
