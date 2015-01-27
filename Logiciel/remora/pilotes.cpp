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

#include "pilotes.h"

int SortiesFP[NB_FILS_PILOTES*2] = { FP1,FP2,FP3,FP4,FP5,FP6,FP7 };
char etatFP[20] = "";

// Instanciation de l'I/O expander
Adafruit_MCP23017 mcp;

/* ======================================================================
Function: pilotes_Setup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : true if MCP23017 module found, false otherwise
Comments: -
====================================================================== */
bool pilotes_setup(void)
{
	Serial.print("Initializing MCP23017...");

	// Détection du MCP23017
	if (!i2c_detect(MCP23017_ADDRESS))
	//if (i2c_detect(0xFF))
	{
		Serial.println("Not found!");
		return (false);
	}
	else
	{
		Serial.println("OK!");

		// et l'initialiser
		mcp.begin();

		// Mettre toutes les PIN en sortie
		mcp.writeRegister(MCP23017_IODIRA,0x00);
		mcp.writeRegister(MCP23017_IODIRB,0x00);
	}

	// ou l'a trouvé
	return (true);
}

/* ======================================================================
Function: setFP
Purpose : selectionne le mode d'un des fils pilotes
Input   : commande numéro du fil pilote (1 à NB_FILS_PILOTE) + commande
					C=Confort, A=Arrêt, E=Eco, H=Hors gel, 1=Eco-1, 2=Eco-2
					ex:	1A => FP1 Arrêt
							41 => FP4 eco -1 (To DO)
							6C => FP6 confort
							72 => FP7 eco -2 (To DO)
Output  : 0 si ok -1 sinon
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int setFP(String command)
{
	command.trim();
	command.toUpperCase();

	// Vérifier que l'on a la commande d'un seul fil pilote (2 caractères)
	if (command.length() != 2)
	{
		return (-1);
	}
	else
	{
		// numéro du fil pilote concerné, avec conversion ASCII > entier
		// la commande est vérifiée dans fpC, pas besoin de traiter ici
		uint8_t fp = command[0]-'0';
		char cOrdre= command[1];

		// Vérifier que le numéro du fil pilote ne dépasse le MAX et
		// que la commande est correcte
		// Pour le moment les ordres Eco-1 et Eco-2 ne sont pas traités
		if ( (fp < 1 || fp > NB_FILS_PILOTES) ||
				(cOrdre!='C' && cOrdre!='E' && cOrdre!='H' && cOrdre!='A') )
		{
				// erreur
				return (-1);
		}
		// Ok ici tout est correct
		else
		{
			// Commande à passer
			uint8_t fpcmd1, fpcmd2;

			// tableau d'index de 0 à 6 pas de 1 à 7
			// on en profite pour Sauver l'état
			etatFP[--fp]=cOrdre;

			switch (cOrdre)
			{
					// Confort => Commande 0/0
					case 'C': fpcmd1=LOW;  fpcmd2=LOW;  break;
					// Eco => Commande 1/1
					case 'E': fpcmd1=HIGH; fpcmd2=HIGH; break;
					// Hors gel => Commande 1/0
					case 'H': fpcmd1=HIGH; fpcmd2=LOW;  break;
					// Arrêt => Commande 0/1
					case 'A': fpcmd1=LOW;  fpcmd2=HIGH; break;
					// Eco - 1
					case '1': { /* to DO */ } ; break;
					// Eco - 2
					case '2': { /* to DO */ }; break;
			}

			// On positionne les sorties physiquement
			mcp.digitalWrite(SortiesFP[2*fp+0], fpcmd1);
			mcp.digitalWrite(SortiesFP[2*fp+1], fpcmd2);
			return (0);
		}
	}
}

/* ======================================================================
Function: delestage
Purpose : met tous les fils pilotes en mode hors-gel
Input   : -
Output  : -
Comments: -
====================================================================== */
void delestage(void)
{
	// buffer contenant la commande à passer à setFP
	char cmd[] = "xH" ;

	// On positionne tous les FP en Hors-Gel
	for (uint8_t i=1; i<=NB_FILS_PILOTES; i+=1)
	{
		cmd[0]='0' + i;
		setFP(cmd);
	}
}

/* ======================================================================
Function: fpControl
Purpose : selectionne le mode d'un ou plusieurs les fils pilotes d'un coup
Input   : liste des commandes
					-=rien, C=Confort, A=Arrêt, E=Eco, H=Hors gel, 1=Eco-1, 2=Eco-2,
					ex:	1A => FP1 Arrêt
							CCCCCCC => Commande tous les fils pilote en mode confort (ON)
							AAAAAAA => Commande tous les fils pilote en mode arrêt
							EEEEEEE => Commande tous les fils pilote en mode éco
							CAAAAAA => Tous OFF sauf le fil pilote 1 en confort
							A-AAAAA => Tous OFF sauf le fil pilote 2 inchangé
							E-CHA12 => FP2 Eco  , FP2 inchangé, FP3 confort, FP4 hors gel
												 FP5 arrêt, FP6 Eco-1		, FP7 Eco-2
Output  : 0 si ok -1 sinon
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int fpControl(String command)
{
	command.trim();
	command.toUpperCase();

	// Vérifier que l'on a la commande de tous les fils pilotes
	if (command.length() != NB_FILS_PILOTES)
	{
	    return(-1) ;
	}
	else
	{
    int8_t returnValue = 0; // Init à 0 => OK
		char 	 cmd[] = "xx" ; // buffer contenant la commande à passer à setFP

    // envoyer les commandes pour tous les fils pilotes
		for (uint8_t i=1; i<=NB_FILS_PILOTES; i++)
		{
			cmd[0] = i ;
			cmd[1] = command[i-1]; // l'index de la chaine commence à 0 donc i-1

			// Si on ne doit pas laisser le fil pilote inchangé
			if (cmd[1] != '-' )
			{
				// ok ici au cas ou la commande setFP n'est pas bonne
				// on positionne le code de retour à -1 mais on
				// continue le traitement, les suivantes sont
				// peut-être correctes
				if (setFP(cmd) == -1)
					returnValue = -1;
			}
		}
    return returnValue;
	}
}

/* ======================================================================
Function: relais
Purpose : selectionne l'état du relais
Input   : état du relais (0 ouvert, 1 fermé)
Output  : etat du relais (0 ou 1)
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int relais(String command)
{
	command.trim();

	// Vérifier que l'on a la commande d'un seul caractère
	if (command.length() != 1)
		return (-1);

	// On souhaite le desactiver ?
	if ( command[0] == '0' )
	{
		// Relais desactivé
		etatrelais=0;

		// Etteindre le relais et la LED
		mcp.digitalWrite(RELAIS_PIN, LOW);
		mcp.digitalWrite(LED_PIN, LOW);

	}
	// On souhaire l'activer ?
	else if (command[0] == '1')
	{
		// Relais enclenché
		etatrelais=1;

		// Allumer le relais et la LED
		mcp.digitalWrite(RELAIS_PIN, HIGH);
		mcp.digitalWrite(LED_PIN, HIGH);
	}

	return (etatrelais);
}
