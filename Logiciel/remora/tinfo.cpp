// **********************************************************************************
// Teleinfo management file for remora project
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//
//           13/04/2015 Theju
//                      Modification des variables cloud teleinfo
//                      (passage en 1 seul appel) et liberation de variables
// **********************************************************************************

#include "tinfo.h"

#ifdef MOD_TELEINFO
// Instanciation de l'objet Téléinfo
TInfo tinfo;
#endif

uint mypApp   = 0;
uint myiInst  = 0;
uint myindexHC= 0;
uint myindexHP= 0;
uint myimax= 0;
uint myisousc	= ISOUSCRITE; // pour calculer la limite de délestage
char myPeriode[8]= "";
char mytinfo[250] ="";
char mycompteur[64] ="";
float ratio_delestage = DELESTAGE_RATIO;
float ratio_relestage = RELESTAGE_RATIO;
float myDelestLimit = 0.0;
float myRelestLimit = 0.0;
uint etatrelais       = 0; // Etat du relais
bool tramevalide			= false;

unsigned long tinfo_led_timer = 0; // Led blink timer

ptec_e ptec; // Puissance tarifaire en cours


/* ======================================================================
Function: ADPSCallback
Purpose : called by library when we detected a ADPS on any phase
Input   : phase number
            0 for ADPS (monophase)
            1 for ADIR1 triphase
            2 for ADIR2 triphase
            3 for ADIR3 triphase
Output  : -
Comments: should have been initialised in the main sketch with a
          tinfo.attachADPSCallback(ADPSCallback())
====================================================================== */
void ADPSCallback(uint8_t phase)
{
  // Led Rouge
  LedRGBON(COLOR_RED);
  tinfo_led_timer = millis();

  // Monophasé
  if (phase == 0 ) {
    Serial.println(F("ADPS"));
  } else {
    Serial.print(F("ADPS Phase "));
    Serial.println('0' + phase);
  }
}

/* ======================================================================
Function: DataCallback
Purpose : callback when we detected new or modified data received
Input   : linked list pointer on the concerned data
          value current state being TINFO_VALUE_ADDED/TINFO_VALUE_UPDATED
Output  : -
Comments: -
====================================================================== */
void DataCallback(ValueList * me, uint8_t flags)
{
  // Do whatever you want there
  Serial.print(me->name);
  Serial.print('=');
  Serial.print(me->value);

  Serial.print(" Flags=0x");
  Serial.print(flags, HEX);

  if ( flags & TINFO_FLAGS_NOTHING ) Serial.print(F(" Nothing"));
  if ( flags & TINFO_FLAGS_ADDED )   Serial.print(F(" Added"));
  if ( flags & TINFO_FLAGS_UPDATED ) Serial.print(F(" Updated"));
  if ( flags & TINFO_FLAGS_EXIST )   Serial.print(F(" Exist"));
  if ( flags & TINFO_FLAGS_ALERT )   Serial.print(F(" Alert"));

	// Nous venons de recevoir la puissance tarifaire en cours
	// To DO : gérer les autres types de contrat
	if (!strcmp(me->name, "PETC"))
	{
	 	// Récupération de la période tarifaire en cours
		strncpy(myPeriode, me->value, strlen(me->value));

		// Determination de la puissance tarifaire en cours
		// To DO : gérer les autres types de contrat
		if (!strcmp(me->value, "HP..")) ptec= PTEC_HP;
		if (!strcmp(me->value, "HC..")) ptec= PTEC_HC;
	}

  Serial.println();
}

/* ======================================================================
Function: NewFrame
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : -
Comments: -
====================================================================== */
void NewFrame(ValueList * me)
{
  char buff[32];

  // Light the RGB LED
  LedRGBON(COLOR_GREEN);
  tinfo_led_timer = millis();

  #if defined (ESP8266)
  	//sprintf( buff, "New Frame (%ld Bytes free)", ESP.getFreeHeap() );
  #elif defined (SPARK)
  	//sprintf( buff, "New Frame (%ld Bytes free)", System.freeMemory());
  #else
  	//sprintf( buff, "New Frame");
  #endif
  //Serial.println(buff);

  // Pour l'init
  tramevalide = true;
}

/* ======================================================================
Function: NewFrame
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : -
Comments: it's called only if one data in the frame is different than
          the previous frame
====================================================================== */
void UpdatedFrame(ValueList * me)
{
  char buff[32];

  // Light the RGB LED (purple) and set timer
  LedRGBON(COLOR_MAGENTA);
  tinfo_led_timer = millis();

  #if defined (ESP8266)
  	//sprintf( buff, "Updated Frame (%ld Bytes free)", ESP.getFreeHeap() );
  #elif defined (SPARK)
  	//sprintf( buff, "Updated Frame (%ld Bytes free)", System.freeMemory());
  #else
  	//sprintf( buff, "Updated Frame");
  #endif
  //Serial.println(buff);

	// Mise à jour des variables "cloud"
	if ( tinfo.valueGet("PAPP"  , buff)) mypApp    = atoi(buff);
	if ( tinfo.valueGet("IINST" , buff)) myiInst   = atoi(buff);
	if ( tinfo.valueGet("HCHC"  , buff)) myindexHC = atol(buff);
	if ( tinfo.valueGet("HCHP"  , buff)) myindexHP = atol(buff);
	if ( tinfo.valueGet("ISOUSC", buff)) myisousc  = atoi(buff);
	if ( tinfo.valueGet("IMAX"  , buff)) myimax    = atoi(buff);

	// Calcul de quand on déclenchera le délestage
	myDelestLimit = myisousc * ratio_delestage;

	// Calcul de quand on déclenchera le relestage
	myRelestLimit = myisousc * ratio_relestage;

	//On publie toutes les infos teleinfos dans un seul appel :
	sprintf(mytinfo,"{\"papp\":%u,\"iinst\":%u,\"isousc\":%u,\"ptec\":%u,\"indexHP\":%u,\"indexHC\":%u,\"imax\":%u,\"ADCO\":%u}",mypApp,myiInst,myisousc,ptec,myindexHP,myindexHC,myimax,mycompteur);
	// Posibilité de faire une pseudo serial avec la fonction suivante :
	//Spark.publish("Teleinfo",mytinfo);

	// Ok nous avons une téléinfo fonctionelle
	tramevalide = true;
}

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

	#ifdef SPARK
	Serial1.begin(1200);  // Port série RX/TX on serial1 for Spark
	#endif

	// Init teleinfo
  tinfo.init();

  // Attach the callback we need
  tinfo.attachADPS(ADPSCallback);
  tinfo.attachData(DataCallback);
  tinfo.attachNewFrame(NewFrame);
  tinfo.attachUpdatedFrame(UpdatedFrame);

	// Doit-on attendre une trame valide
	if (wait_frame)
	{
		// debut du time out
		long started = millis();

		// ici on attend une trame ou le time out
		while (!tramevalide && millis()-started<TINFO_FRAME_TIMEOUT*1000)
		{
			// Main loop de la teleinfo
			tinfo_loop();

			// Jamais on ne bloque le "core" firmware du Spark
			// sous perte de deconnexion au cloud
			#ifdef SPARK
			SPARK_WLAN_Loop();
			#endif
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
#ifdef MOD_TELEINFO
	char c;
	uint8_t nb_char=0;

	// Caractère présent sur la sérial téléinfo ?
	// On prendra maximum 8 caractères par passage
	// les autres au prochain tour, çà evite les
	// long while bloquant pour les autres traitements
	#ifdef SPARK
		while (Serial1.available() && nb_char<8)
		{
			c = (Serial1.read() & 0x7F);
			tinfo.process(c);
			nb_char++;
		}
	#else
		while (Serial.available() && nb_char<8)
		{
			c = (Serial.read() & 0x7F);
			tinfo.process(c);
			nb_char++;
		}
	#endif

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
			//Le courant est suffisamment bas pour relester
			if (myiInst < myRelestLimit)
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

  // Do we have RGB led timer expiration ?
  if (tinfo_led_timer && (millis()-tinfo_led_timer >= TINFO_LED_BLINK_MS))
  {
      LedRGBOFF(); // Light Off the LED
      tinfo_led_timer=0; // Stop virtual timer
  }

#endif
}
