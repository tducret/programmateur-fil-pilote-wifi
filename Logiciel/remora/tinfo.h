// **********************************************************************************
// Teleinfo header file for remora project
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           15/09/2015 Charles-Henri Hallard Utilisation Librairie Teleinfo Universelle
//
// **********************************************************************************
#ifndef TINFO_h
#define TINFO_h

#include "remora.h"

// Nombre de secondes pendant lesquelles
// nous pouvons attendre une etiquette/trame téléinfo
#define TINFO_DATA_TIMEOUT 1
#define TINFO_FRAME_TIMEOUT 3
#define ISOUSCRITE 30 // sera mis à jour à la reception de trame teleinfo
#define DELESTAGE_RATIO 0.9 //ratio en % => 90%
#define RELESTAGE_RATIO 0.8 //ratio en % => 80%
#define IMAX 35 // sera mis à jour à la reception de trame teleinfo
#define TINFO_LED_BLINK_MS  150 // Time of RGB LED blink

// Tarif en cours au format numérique
enum ptec_e { PTEC_HP = 1, PTEC_HC = 2 };

// Variables exported to other source file
// ========================================
extern TInfo tinfo;
extern unsigned int mypApp;
extern unsigned int myiInst;
extern unsigned int myindexHC;
extern unsigned int myindexHP;
extern unsigned int myisousc;
extern ptec_e ptec; // Puisance tarifaire en cours
extern char myPeriode[];
extern char mytinfo[];
extern char myAction[];

extern int      etatrelais;
extern float    myDelestLimit;
extern float    myRelestLimit;

// Function exported for other source file
// =======================================
bool tinfo_setup(bool);
void tinfo_loop();

#endif
