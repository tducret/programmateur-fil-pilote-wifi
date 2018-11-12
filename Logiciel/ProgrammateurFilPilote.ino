/*
Programmateur Fil Pilote et Suivi Conso
Copyright (C) 2014 Thibault Ducret
Licence MIT
*/

// Includes
#include "TeleInfo.h"

// Defines
#define NB_FILS_PILOTES 7
#define ISOUSCRITE 30
#define DELESTAGE_RATIO 0.9 //ratio en % => 90%
#define IMAX 35

// ======== MAIN ============

// Fils pilotes
int SortiesFP[NB_FILS_PILOTES*2] = { D6,D7,D4,D5,D2,D3,A6,A7,A4,A5,A2,A3,A0,A1 };
int isousc = ISOUSCRITE;
float ratio_intensite = DELESTAGE_RATIO;

void initCdeFilsPilotes();
int fpControl(String command);
int setFP(String command);
void delestage();
int fpC(int i, char cOrdre);

unsigned int delestageEnCours = 1;
int nbDelestage = 0;

// Téléinfo
TeleInfo ti; // objet téléinfo
char c; // pour stocker un caractère reçu sur le port série
bool trameComplete = false;
int mypApp = TELEINFO_UINT_INVALIDE;
int myiInst = TELEINFO_UINT_INVALIDE;
int myindexHC = TELEINFO_UINT_INVALIDE;
int myindexHP = TELEINFO_UINT_INVALIDE;
unsigned int myisousc = TELEINFO_UINT_INVALIDE; // pour calculer la limite de délestage
float myDelestLimit = 0.0;
boolean init_teleinfo = false;
char etatFP[20] = "";


void setup()
{
  // == Fils pilotes ==
  initCdeFilsPilotes();
  Particle.function("fp", fpControl);
  Particle.function("setfp", setFP);
  Particle.variable("nbdelest", nbDelestage);
  Particle.variable("etatfp", etatFP);
   
  // == Téléinfo ==
  Particle.variable("papp", mypApp);
  Particle.variable("iinst", myiInst);
  Particle.variable("indexhc", myindexHC);
  Particle.variable("indexhp", myindexHP);
  
  Serial.begin(115200); // Port série USB
  Serial1.begin(1200);  // Port série RX/TX
}

void loop()
{
  updateTeleinfo();
	if (myiInst >= myDelestLimit) {
  
  
          if (delestageEnCours == 0)
          {
            //delestageEnCours ne passe jamais à 1 ???
			nbDelestage += 1; // on vient juste de passer en délestage
      	    delestage();
          }
	}
	else {
		delestageEnCours = 0; // Pas de délestage
	}
}

void updateTeleinfo() {

  while (Serial1.available())
  {
    c = (Serial1.read() & 0x7F);
    trameComplete = ti.decode(c);
    if (trameComplete)
    {
      mypApp=(int) ti.pApp();
      myiInst=(int) ti.iInst();
      myindexHC=(int) ti.indexHC();
      myindexHP=(int) ti.indexHP();
	  
	  if(!init_teleinfo) {
		myisousc = ti.iSousc();
		myDelestLimit = myisousc * ratio_intensite;
		init_teleinfo = true;
	  }
    }
  }

}

void initCdeFilsPilotes()
{
  unsigned int i;
  for (i=0;i<(NB_FILS_PILOTES*2);i+=1) // 2*nbFilPilotes car 2 pins pour commander 1 fil pilote
  {
        pinMode(SortiesFP[i], OUTPUT); // Chaque commande de fil pilote est une sortie
  }
  delay(1000);
  delestage(); // Hors gel
}

int fpControl(String command)
{
// C = Confort, A = Arrêt, E = Eco, H = Hors gel
// CCCCCCC => Commande tous les fils pilote en mode confort (ON)
// AAAAAAA => Commande tous les fils pilote en mode arrêt
// EEEEEEE => Commande tous les fils pilote en mode éco
// CAAAAAA => Tous OFF sauf le fil pilote 1

	unsigned int i;
	int returnValue = -1; // Init à -1 => Erreur
	
	command.trim();
	command.toUpperCase();
	
	if (command.length() == NB_FILS_PILOTES) // Vérifier que l'on a la commande de tous les fils pilotes
	{
		for (i=1; i<=NB_FILS_PILOTES; i+=1)
		{
			fpC(i, command.charAt(i-1)); //charAt(i-1) car l'index de "command" commence à 0
		}
		returnValue = 0; // OK
	}

   return returnValue;
}

int setFP(String command)
{
// C = Confort, A = Arrêt, E = Eco, H = Hors gel
// command = 1A => FP1 = Arrêt
	unsigned int i;
	
	int returnValue = -1; // Init à -1 => Erreur
	
	command.trim();
	command.toUpperCase();
	
	if (command.length() == 2) // Vérifier que l'on a la commande d'un seul fil pilote
	{
		i = command.charAt(0) - '0'; // numéro du fil pilote concerné, avec conversion ASCII > entier
		returnValue = fpC(i, command.charAt(1));
	}
	return returnValue;
}

int fpC(int i, char cOrdre)
{
// i => numéro du fil pilote (de 1 à NB_FILS_PILOTE)
// cOrdre => C = Confort, A = Arrêt, E = Eco, H = Hors gel

	int returnValue = -1; // Init à -1 => Erreur
	
	i = i-1; // On commence à 0
	
	if (i >= 0 && i < NB_FILS_PILOTES) // Vérifier que le numéro du fil pilote ne dépasse le MAX
	{		
		if (cOrdre == 'C') // Confort => Commande 0/0
		{
			digitalWrite(SortiesFP[2*i], LOW);
			digitalWrite(SortiesFP[(2*i)+1], LOW);
			etatFP[i]='C';
	 	}
		else if (cOrdre == 'E') // Eco => Commande 1/1
		{
			digitalWrite(SortiesFP[2*i], HIGH);
			digitalWrite(SortiesFP[(2*i)+1], HIGH);
			etatFP[i]='E';
		}
		else if (cOrdre == 'H') // Hors gel => Commande 1/0
		{
			digitalWrite(SortiesFP[2*i], HIGH);
			digitalWrite(SortiesFP[(2*i)+1], LOW);
			etatFP[i]='H';
		}
		else // Arrêt => Commande 0/1
		{
			digitalWrite(SortiesFP[2*i], LOW);
			digitalWrite(SortiesFP[(2*i)+1], HIGH);
			etatFP[i]='A';
	 	}
	 	returnValue = 0; // OK
	}
   return returnValue;
}

void delestage()
{
	unsigned int i;
	
	for (i=1; i<=NB_FILS_PILOTES; i+=1)
	{
		fpC(i,'H');
	}
	
	return;
}
