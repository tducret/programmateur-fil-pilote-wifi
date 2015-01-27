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

// Tout est inclus dans le fichier remora.h
// Pour activer des modules spécifiques ou
// changer différentes configurations il
// faut le faire dans le fichier remora.h
#include "remora.h"

#ifdef MOD_TELEINFO
// Instanciation de l'objet Téléinfo
TeleInfo ti;
#endif

// Variables globales au projet
// ============================
bool trameComplete            = false;
bool refreshDisplay           = false;
char myPeriode[8]             = "";
unsigned int delestageEnCours = 1;
unsigned int nbDelestage      = 0;
unsigned int mypApp           = 0;
unsigned int myiInst          = 0;
unsigned int myindexHC        = 0;
unsigned int myindexHP        = 0;
unsigned int etatrelais       = 0; // Etat du relais
unsigned int myisousc         = ISOUSCRITE; // pour calculer la limite de délestage
int isousc                    = ISOUSCRITE;
float ratio_intensite         = DELESTAGE_RATIO;
uint16_t status               = 0; // status global de l'application

ptec_e ptec; // Puissance tarifaire en cours

float myDelestLimit = 0.0;

/* ======================================================================
Function: setup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : -
Comments: -
====================================================================== */
void setup()
{
  uint8_t rf_version = 0;
  bool start = false;
  char c;
  long started ;

  // On prend le controle de la LED RGB
  // En rouge nous ne sommes pas encore prêt
  RGB.control(true);
  RGB.brightness(32);
  RGB.color(255, 0,0);

  // Init bus I2C
  i2c_init();

  // nous sommes en GMT+1
  Time.zone(+1);

  Serial.begin(115200); // Port série USB

  // C'est parti
  started = millis();

  // Attendre que le core soit bien connecté à la serial
  // car en cas d'update le core perd l'USB Serial a
  // son reboot et sous windows faut reconnecter quand
  // on veut débugguer, et si on est pas synchro on rate
  // le debut du programme, donc petite pause le temps de
  // reconnecter le terminal série sous windows
  // Une fois en prod c'est plus necessaire, c'est vraiment
  // pour le développement (time out à 10s)
  while(!start)
  {
    // Il suffit du time out ou un caractère reçu
    // sur la liaison série USB pour démarrer
    if (Serial.available() || millis()-started >= 10000)
      start = true;

    // On clignote en rouge pour indiquer l'attente
    RGB.color(255, 0,0);
    delay(50);

    // Jamais on ne bloque le "core" firmware du Spark
    // sous perte de deconnexion au cloud
    SPARK_WLAN_Loop();

    // On clignote en rouge pour indiquer l'attente
    RGB.color(0,0,0);
    delay(100);
  }

  // Et on affiche nos paramètres
  Serial.println("Core Network settings");
  Serial.print("IP   : "); Serial.println(WiFi.localIP());
  Serial.print("Mask : "); Serial.println(WiFi.subnetMask());
  Serial.print("GW   : "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSDI : "); Serial.println(WiFi.SSID());
  Serial.print("RSSI : "); Serial.print(WiFi.RSSI());
  Serial.println("dB");

  // Init de l'I/O expander des fils pilotes
  if (pilotes_setup())
    status |= STATUS_MCP ;

  #ifdef MOD_OLED
    // Initialisation de l'afficheur
    if (display_setup())
      status |= STATUS_OLED ;
  #endif

  #ifdef MOD_TELEINFO
    Serial.println("Initializing Teleinfo Serial");
    Serial1.begin(1200);  // Port série RX/TX

    // Déclaration des variables "cloud" pour la téléinfo
    Spark.variable("papp", &mypApp, INT);
    Spark.variable("iinst", &myiInst, INT);
    Spark.variable("indexhc", &myindexHC, INT);
    Spark.variable("indexhp", &myindexHP, INT);
    Spark.variable("periode", &myPeriode, STRING); // Période tarifaire en cours (string)
    Spark.variable("iperiode", &ptec, INT); // Période tarifaire en cours (numerique)

  #endif

  #ifdef MOD_RF69
    // Initialisation RFM69 Module
    if ( rfm_setup())
      status |= STATUS_RFM ;
  #endif


  // Led verte durant le test
  RGB.color(0, 255, 0);

  // Splash screen
  display_splash();

  // Enclencher le relais 2 secondes
  Serial.print("Relais=ON   ");
  relais("1");
  delay(2000);
  Serial.println("Relais=OFF");
  relais("0");

  // nous avons fini, led Jaune
  RGB.color(255, 255, 0);

  // Hors gel, désactivation des fils pilotes
  delestage();

  // Déclaration des fonction "cloud"
  Spark.function("fp", fpControl);
  Spark.function("setfp", setFP);
  Spark.function("relais", relais);

  // Déclaration des variables "cloud"
  Spark.variable("nbdelest", &nbDelestage, INT);
  Spark.variable("etatfp", &etatFP, STRING);
  Spark.variable("etatrelais", &etatrelais, INT);

  // On etteint la LED embarqué du core
  RGB.color( 0, 0, 0);

  Serial.println("Starting main loop");
}

/* ======================================================================
Function: loop
Purpose : boucle principale du programme
Input   : -
Output  : -
Comments: -
====================================================================== */
void loop()
{
  char c;
  static uint8_t lastsec = Time.second();

  // Par defaut on rafraichit à minima toutes les secondes
  if (Time.second() != lastsec )
  {
    refreshDisplay = true;
    lastsec= Time.second();
  }

  #ifdef MOD_TELEINFO
    // Caractère présent sur la sérial téléinofo ?
    while (Serial1.available())
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
        if (!strcmp(myPeriode,"HP..")) ptec=PTEC_HP;
        if (!strcmp(myPeriode,"HC..")) ptec=PTEC_HC;

        // 1ere trame ?
        // Ok nous avons une téléinfo fonctionelle
        if( (status & STATUS_TINFO) == 0 )
          status |= STATUS_TINFO;

        // Ok à ce stade nous venons de recevoir une Trame valide

        // on rafraichira l'affichage
        refreshDisplay = true;

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
    } // While serial teleinfi
  #endif

  #ifdef MOD_RF69
    // Vérification de la reception d'une trame RF
    if (status & STATUS_RFM)
      rfm_loop();
  #endif

  #ifdef MOD_OLED
    // pour le moment on se contente d'afficher la téléinfo
    screen_state = screen_teleinfo;

    // Modification d'affichage et afficheur présent ?
    if (refreshDisplay && (status & STATUS_OLED))
      display_loop();
  #endif

  // çà c'est fait
  refreshDisplay = false;

  // on laisse un peu de temp au système
  delay(20);
}
