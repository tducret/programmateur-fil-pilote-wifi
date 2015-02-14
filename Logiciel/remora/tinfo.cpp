// **********************************************************************************
// Teleinfo management file for remora project
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//
// **********************************************************************************

#include "tinfo.h"

#ifdef MOD_TELEINFO
// Instanciation de l'objet Téléinfo
TeleInfo ti;
#endif

uint mypApp   = 0;
uint myiInst  = 0;
uint myindexHC= 0;
uint myindexHP= 0;
uint myisousc	= ISOUSCRITE; // pour calculer la limite de délestage
//int  isousc   = ISOUSCRITE;
char myPeriode[8]= "";
float ratio_intensite = DELESTAGE_RATIO;
float myDelestLimit = 0.0;
uint delestageEnCours = 1;
uint nbDelestage      = 0;
uint etatrelais       = 0; // Etat du relais
bool tramevalide			= false;

ptec_e ptec; // Puissance tarifaire en cours


/* ======================================================================
Function: tinfo_setup
Purpose : prepare and init stuff, configuration, ..
Input   : indique si on doit bloquer jusqu'à reception 1ere trame téléinfo
Output  : false si on devait attendre et time out expiré, true sinon
Comments: -
====================================================================== */
bool tinfo_setup(bool wait_frame)
{
	Serial.print("Initializing Teleinfo...");

	Serial1.begin(1200);  // Port série RX/TX

	// Doit-on attendre une trame valide
	if (wait_frame)
	{
		// debut du time out
		long started = millis();

		// ici on attend une trame ou le time out
		while (tramevalide==false && millis()-started<TINFO_FRAME_TIMEOUT*1000)
		{
			// Main loop de la teleinfo
			tinfo_loop();

			// Jamais on ne bloque le "core" firmware du Spark
			// sous perte de deconnexion au cloud
			SPARK_WLAN_Loop();
		}
	}

	Serial.println("OK!");

	return (wait_frame && !tramevalide ? false : true);
}

/* ======================================================================
Function: tinfo_loop
Purpose : gestion des trames reçues par la librairie teleinfo
Input   : -
Output  : -
Comments: -
====================================================================== */
void tinfo_loop(void)
{
	char c;
	uint8_t nb_char=0;

	#ifdef MOD_TELEINFO

	// Caractère présent sur la sérial téléinfo ?
	// On prendra maximum 8 caractères par passage
	// les autres au prochain tour, çà evite les
	// long while bloquant pour les autres traitements
	while (Serial1.available() && nb_char++<8)
	{
		// Recupération du caractère
		c = (Serial1.read() & 0x7F);

		// Avons-nous une trame complète ?
		if (ti.decode(c))
		{
			// Mise à jour des variables "cloud"
			mypApp    = ti.pApp();
			myiInst   = ti.iInst();
			myindexHC = ti.indexHC();
			myindexHP = ti.indexHP();
			myisousc  = ti.iSousc();

			// Calcul de quand on déclenchera le délestage
			myDelestLimit = myisousc * ratio_intensite;

			// Récupération de la période tarifaire en cours
			strncpy(myPeriode, ti.perTarif(), sizeof(ti.perTarif()));

			// Determination de la puissance tarifaire en cours
			// To DO : gérer les autres types de contrat
			if (!strcmp(myPeriode,"HP..")) ptec= PTEC_HP;
			if (!strcmp(myPeriode,"HC..")) ptec= PTEC_HC;

			// 1ere trame ?
			// Ok nous avons une téléinfo fonctionelle
			tramevalide = true;

			// Ok à ce stade nous venons de recevoir une Trame valide

			// Faut-il enclencher le delestage ?
			// To DO : gestion d'un delestage plus fin de type ou
			//         ou coupe les Pilotes à un 1 et on vérifie
			//         que c'est OK
			if (myiInst >= myDelestLimit)
			{
				if (delestageEnCours == 0)
				{
					//delestageEnCours ne passe jamais à 1 ???
					nbDelestage += 1; // on vient juste de passer en délestage
					delestage();
				}
			}
			else
			{
				delestageEnCours = 0; // Pas de délestage
			}
		} // If trame complète
	} // While serial teleinfo

	#endif
}
