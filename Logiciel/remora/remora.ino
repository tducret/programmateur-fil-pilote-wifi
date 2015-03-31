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

// Variables globales
// ==================
// status global de l'application
uint16_t status = 0;

// Nombre de deconexion cloud detectée
int cloud_disconnect = 0;

/* ======================================================================
Function: spark_expose_cloud
Purpose : declare et expose les variables et fonctions cloud
Input   :
Output  : -
Comments: -
====================================================================== */
void spark_expose_cloud(void)
{
  Serial.println("spark_expose_cloud()");

  #ifdef MOD_TELEINFO
    // Déclaration des variables "cloud" pour la téléinfo (10 variables au maximum)
    // je ne sais pas si les fonction cloud sont persistentes
    // c'est à dire en cas de deconnexion/reconnexion du wifi
    // si elles sont perdues ou pas, à tester
    Spark.variable("papp", &mypApp, INT);
    Spark.variable("iinst", &myiInst, INT);
    //Spark.variable("isousc", &myisousc, INT);
    Spark.variable("indexhc", &myindexHC, INT);
    Spark.variable("indexhp", &myindexHP, INT);
    Spark.variable("periode", &myPeriode, STRING); // Période tarifaire en cours (string)
    //Spark.variable("iperiode", (ptec_e *)&ptec, INT); // Période tarifaire en cours (numerique)

    // Récupération d'une valeur d'étiquette
    Spark.variable("tinfo", &mytinfo, STRING);
  #endif

  // Déclaration des fonction "cloud" (4 fonctions au maximum)
  Spark.function("fp",    fp);
  Spark.function("setfp", setfp);

  // Déclaration des variables "cloud"
  Spark.variable("nivdelest", &nivDelest, INT); // Niveau de délestage (nombre de zones délestées)
  //Spark.variable("disconnect", &cloud_disconnect, INT);
  Spark.variable("etatfp", &etatFP, STRING); // Etat actuel des fils pilotes
  Spark.variable("memfp", &memFP, STRING); // Etat mémorisé des fils pilotes (utile en cas de délestage)

  // relais pas disponible sur les carte 1.0
  #ifndef REMORA_BOARD_V10
    Spark.function("relais", relais);
    Spark.variable("etatrelais", &etatrelais, INT);
  #endif
}


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
  Serial.print("RSSI : "); Serial.print(WiFi.RSSI());Serial.println("dB");

  Serial.print("Compilé avec les fonctions : ");

  #ifdef REMORA_BOARD_V12
    Serial.print("MCP23017 ");
  #endif
  #ifdef MOD_OLED
    Serial.print("OLED ");
  #endif
  #ifdef MOD_TELEINFO
    Serial.print("TELEINFO ");
  #endif
  #ifdef MOD_RF69
    Serial.print("RFM69 ");
  #endif

  Serial.println();

  // Init des fils pilotes
  if (pilotes_setup())
    status |= STATUS_MCP ;

  #ifdef MOD_OLED
    // Initialisation de l'afficheur
    if (display_setup())
    {
      status |= STATUS_OLED ;
      // Splash screen
      display_splash();
    }
  #endif

  #ifdef MOD_TELEINFO
    // Initialiser la téléinfo et attente d'une trame valide
    if (tinfo_setup(true))
    {
      status |= STATUS_TINFO;
      Serial.println("Teleinfo OK!");
    }
    else
    {
      Serial.println("Teleinfo non fonctionelle!");
    }
  #endif

  #ifdef MOD_RF69
    // Initialisation RFM69 Module
    if ( rfm_setup())
      status |= STATUS_RFM ;
  #endif

  // Led verte durant le test
  RGB.color(0, 255, 0);

  // Enclencher le relais 2 secondes
  // si dispo sur la carte
  #ifndef REMORA_BOARD_V10
    Serial.print("Relais=ON   ");
    relais("1");
    delay(2000);
    Serial.println("Relais=OFF");
    relais("0");
  #endif

  // nous avons fini, led Jaune
  RGB.color(255, 255, 0);

  // Hors gel, désactivation des fils pilotes
  initFP();

  // Rendre à dispo nos API
  spark_expose_cloud();

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
  static bool refreshDisplay   = false;
  static uint8_t lastsec = Time.second();
  static bool lastcloudstate = Spark.connected();
  bool currentcloudstate ;

  // Par defaut on rafraichit à minima toutes les secondes
  if (Time.second() != lastsec )
  {
    refreshDisplay = true;
    lastsec= Time.second();
  }

  #ifdef MOD_TELEINFO
  // Vérification de la reception d'une 1ere trame téléinfo
    if (status & STATUS_TINFO)
      tinfo_loop();
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

  // recupération de l'état de connexion au cloud spark
  currentcloudstate = Spark.connected();

  // La connexion cloud vient de chager d'état ?
  if (lastcloudstate != currentcloudstate)
  {
    // Mise à jour de l'état
    lastcloudstate=currentcloudstate;

    // on vient de se reconnecter ?
    if (currentcloudstate)
    {
      // on pubie à nouveau nos affaires et led verte
      spark_expose_cloud();
      RGB.color( 0, 255, 0);
    }
    else
    {
      // on compte la deconnexion led rouge
      cloud_disconnect++;
      Serial.print("Perte de conexion au cloud #");
      Serial.println(cloud_disconnect);
      RGB.color( 255, 0, 0);
    }
  }
}
