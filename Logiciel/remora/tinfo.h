// **********************************************************************************
// Teleinfo header file for remora project
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//
// **********************************************************************************
#ifndef TINFO_h
#define TINFO_h

#include "remora.h"

// Nombre de secondes pendant lesquelles
// nous pouvons attendre une trame téléinfo
// a l'initialisation
#define TINFO_FRAME_TIMEOUT 5
#define ISOUSCRITE 30 // sera mis à jour à la reception de trame teleinfo
#define DELESTAGE_RATIO 0.9 //ratio en % => 90%
#define RELESTAGE_RATIO 0.8 //ratio en % => 80%
#define IMAX 35 // sera mis à jour à la reception de trame teleinfo



// Tarif en cours au format numérique
enum ptec_e { PTEC_HP = 1, PTEC_HC = 2 };

// Variables exported to other source file
// ========================================
extern unsigned int mypApp;
extern unsigned int myiInst;
extern unsigned int myindexHC;
extern unsigned int myindexHP;
extern unsigned int myisousc;
extern ptec_e ptec; // Puisance tarifaire en cours
extern char myPeriode[];
extern char mytinfo[];

extern uint     etatrelais;
extern float    myDelestLimit;
extern float    myRelestLimit;

// Function exported for other source file
// =======================================
bool tinfo_setup(bool);
void tinfo_loop();

#endif
