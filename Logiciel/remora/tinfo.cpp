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
char myPeriode[8]= "";
char mytinfo[200] ="";
char mycompteur[64] ="";
float ratio_delestage = DELESTAGE_RATIO;
float ratio_relestage = RELESTAGE_RATIO;
float myDelestLimit = 0.0;
float myRelestLimit = 0.0;
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
			myDelestLimit = myisousc * ratio_delestage;

			// Calcul de quand on déclenchera le relestage
			myRelestLimit = myisousc * ratio_relestage;

			// Récupération de la période tarifaire en cours
			strncpy(myPeriode, ti.perTarif(), sizeof(ti.perTarif()));

			// Determination de la puissance tarifaire en cours
			// To DO : gérer les autres types de contrat
			if (!strcmp(myPeriode,"HP..")) ptec= PTEC_HP;
			if (!strcmp(myPeriode,"HC..")) ptec= PTEC_HC;

			//On publie toutes les infos teleinfos dans un seul appel :
			sprintf(mytinfo,"{\"papp\":%u,\"iinst\":%u,\"isousc\":%u,\"Periode\":%u,\"indexHP\":%u,\"indexHC\":%u}",mypApp,myiInst,myisousc,ptec,myindexHP,myindexHC);
			//Spark.publish("Teleinfo",mytinfo);

			// On creer les compteurs :
			sprintf(mycompteur,"{\"indexHP\":\"%u\",\"indexHC\":\"%u\"}",myindexHP,myindexHC);
			// Spark.publish("Compteur",mycompteur);

			// 1ere trame ?
			// Ok nous avons une téléinfo fonctionelle
			tramevalide = true;

			// Ok à ce stade nous venons de recevoir une Trame valide

			// Faut-il enclencher le delestage ?
			if (myiInst > myDelestLimit) //On dépasse le courant max?
			{
				if ((millis() - timerDelestRelest) > 5000L)
				{
					//On ne passe pas dans la boucle si l'on a délesté ou relesté une zone il y a moins de 5s
					//On évite ainsi de délester d'autres zones avant que le délestage précédent ne fasse effet
					delester1zone();
					timerDelestRelest = millis();
				}
			}
			else
			{
				if (nivDelest > 0 && (millis() - timerDelestRelest) > 180000L)
				// Un délestage est en cours (nivDelest > 0)
				// Le délestage/relestage de la dernière zone date de plus de 3 minutes
				// On attend au moins ce délai pour relester ou décaler
				// pour éviter les délestage/relestage trop rapprochés
				{
					if (myiInst < myRelestLimit)
					//Le courant est suffisamment bas pour relester
					{
						relester1zone();
						timerDelestRelest = millis();
					}
					else
					{
						// On fait tourner le délestage
						// ex : AVANT = "DDCEEEE" => APRES = "CDDEEEE"
						decalerDelestage();
						timerDelestRelest = millis();
					}
				}
			}
		} // If trame complète
	} // While serial teleinfo

	#endif
}
