  // **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           13/04/2015 Theju
//                      Modification des variables cloud teleinfo
//                      (passage en 1 seul appel) et liberation de variables
//           15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//
// **********************************************************************************

// Tout est inclus dans le fichier remora.h
// Pour activer des modules spécifiques ou
// changer différentes configurations il
// faut le faire dans le fichier remora.h
#include "remora.h"

#ifdef SPARK
  #include "MCP23017.h"
  #include "SSD1306.h"
  #include "GFX.h"
  #include "ULPNode_RF_Protocol.h"
  #include "LibTeleinfo.h"
  #include "WebServer.h"
  #include "display.h"
  #include "i2c.h"
  #include "pilotes.h"
  #include "rfm.h"
  #include "tinfo.h"
  #include "linked_list.h"
  #include "route.h"
  #include "RadioHead.h"
  #include "RH_RF69.h"
  #include "RHDatagram.h"
  #include "RHReliableDatagram.h"
#endif

// Arduino IDE need include in main INO file
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
  #include <WiFiUDP.h>
  #include <Wire.h>
  #include <SPI.h>
  #include "./MCP23017.h"
  #include "./SSD1306.h"
  #include "./GFX.h"
  #include "./ULPNode_RF_Protocol.h"
  #include "./LibTeleinfo.h"
#endif

// Variables globales
// ==================
// status global de l'application
uint16_t status = 0;
unsigned long uptime = 0;
// Nombre de deconexion cloud detectée
int my_cloud_disconnect = 0;


#ifdef SPARK
  // Particle WebServer
  WebServer server("", 80);
#endif

#ifdef ESP8266
  // ESP8266 WebServer
  ESP8266WebServer server(80);
  // Udp listener for OTA
  WiFiUDP OTA;
  // Use WiFiClient class to create a connection to WEB server
  WiFiClient client;
#endif

/* ======================================================================
Function: spark_expose_cloud
Purpose : declare et expose les variables et fonctions cloud
Input   :
Output  : -
Comments: -
====================================================================== */
#ifdef SPARK
void spark_expose_cloud(void)
{
  Serial.println("spark_expose_cloud()");

  #ifdef MOD_TELEINFO
    // Déclaration des variables "cloud" pour la téléinfo (10 variables au maximum)
    // je ne sais pas si les fonction cloud sont persistentes
    // c'est à dire en cas de deconnexion/reconnexion du wifi
    // si elles sont perdues ou pas, à tester
    // -> Theju: Chez moi elles persistes, led passe verte mais OK
    //Spark.variable("papp", &mypApp, INT);
    //Spark.variable("iinst", &myiInst, INT);
    //Spark.variable("isousc", &myisousc, INT);
    //Spark.variable("indexhc", &myindexHC, INT);
    //Spark.variable("indexhp", &myindexHP, INT);
    //Spark.variable("periode", &myPeriode, STRING); // Période tarifaire en cours (string)
    //Spark.variable("iperiode", (ptec_e *)&ptec, INT); // Période tarifaire en cours (numerique)

    // Récupération des valeurs d'étiquettes :
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
#endif


// ====================================================
// Following are dedicated to ESP8266 Platform
// Wifi management and OTA updates
// ====================================================
#ifdef ESP8266
/* ======================================================================
Function: CheckOTAUpdate
Purpose : Check if we need to update the firmware over the Air
Input   : -
Output  : -
Comments: If upgraded, no return, perform update and reboot ESP
====================================================================== */
void CheckOTAUpdate(void)
{
  //OTA detection
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size = OTA.parseInt();

    LedRGBON(COLOR_MAGENTA);

    Serial.print(F("Update Start: ip:"));
    Serial.print(remote);
    Serial.printf(", port:%d, size:%dKB\n", port, size/1024);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)) {
      Serial.print(F("Update Begin Error"));
      return;
    }

    // Declare Wifi client connexion
    WiFiClient ota_client;

    if (ota_client.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()) {
        written = Update.write(ota_client);
        if(written > 0)
        {
          LedRGBOFF();
          ota_client.print(written, DEC);
          LedRGBON(COLOR_MAGENTA);
        }
      }
      LedRGBOFF();
      Serial.setDebugOutput(false);

      if(Update.end()) {
        ota_client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(ota_client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }

    // Be sure to re enable it
    OTA.begin(DEFAULT_OTA_PORT);
  }
}

/* ======================================================================
Function: WifiHandleConn
Purpose : Handle Wifi connection / reconnection and OTA updates
Input   : -
Output  : state of the wifi status
Comments: -
====================================================================== */
int WifiHandleConn()
{
  int ret = WiFi.status();

  // Wait for connection if disconnected
  if ( ret != WL_CONNECTED ) {

    // Orange we're not connected anymore
    LedRGBON(COLOR_ORANGE);

    Serial.print(F("Connecting to: "));
    Serial.print(DEFAULT_WIFI_SSID);
    Serial.print(F("..."));

    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASS);

    ret = WiFi.waitForConnectResult();
    if ( ret != WL_CONNECTED) {
      LedRGBON(COLOR_RED);
      Serial.println(F("Connection failed!"));
    } else {
      LedRGBON(COLOR_GREEN);
      Serial.println(F("Connected"));
      Serial.print(F("IP address   : ")); Serial.println(WiFi.localIP());
      Serial.print(F("MAC address  : ")); Serial.println(WiFi.macAddress());

      MDNS.begin(DEFAULT_HOSTNAME);
      MDNS.addService("arduino", "tcp", DEFAULT_OTA_PORT);
      OTA.begin(DEFAULT_OTA_PORT);

      // just in case your sketch sucks, keep update OTA Available
      // Trust me, when coding and testing it happens, this could save
      // the need to connect FTDI to reflash
      // Usefull just after 1st connexion when called from setup() before
      // launching potentially bugging main()
      for (uint8_t i=0; i<= 10; i++) {
        LedRGBON(COLOR_MAGENTA);
        delay(100);
        LedRGBOFF();
        delay(200);
        CheckOTAUpdate();
      }
    }
  }

  // Handle OTA if we're connected
  if ( ret == WL_CONNECTED )
    CheckOTAUpdate();

  return ret;
}
#endif

/* ======================================================================
Function: timeAgo
Purpose : format total seconds to human readable text
Input   : second
Output  : pointer to string
Comments: -
====================================================================== */
char * timeAgo(unsigned long sec)
{
  static char buff[16];

  // Clear buffer
  buff[0] = '\0';

  if (sec < 2) {
    sprintf_P(buff,PSTR("just now"));
  } else if (sec < 60) {
    sprintf_P(buff,PSTR("%d seconds ago"), sec);
  } else if (sec < 120) {
    sprintf_P(buff,PSTR("1 minute ago"));
  } else if (sec < 3600) {
    sprintf_P(buff,PSTR("%d minutes ago"), sec/60);
  } else if (sec < 7200) {
    sprintf_P(buff,PSTR("1 hour ago"));
  } else if (sec < 86400) {
    sprintf_P(buff,PSTR("%d hours ago"), sec/3660);
  } else if (sec < 172800) {
    sprintf_P(buff,PSTR("yesterday"));
  } else if (sec < 604800) {
    sprintf_P(buff,PSTR("%d days ago"), sec/86400);
  }
  return buff;
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

  #ifdef SPARK
    bool start = false;
    long started ;

    // On prend le controle de la LED RGB pour faire
    // un heartbeat si Teleinfo ou OLED ou RFM69
    #if defined (MOD_TELEINFO) || defined (MOD_OLED) || defined (MOD_RF69)
    RGB.control(true);
    RGB.brightness(128);
    // En jaune nous ne sommes pas encore prêt
    LedRGBON(COLOR_YELLOW);
    #endif

    // nous sommes en GMT+1
    Time.zone(+1);

    // Rendre à dispo nos API, çà doit être fait
    // très rapidement depuis le dernier firmware
    spark_expose_cloud();

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
      if (Serial.available() || millis()-started >= 2000)
        start = true;

      // On clignote en jaune pour indiquer l'attente
      LedRGBON(COLOR_YELLOW);
      delay(50);

      // Jamais on ne bloque le firmware
      // sous perte de deconnexion du réseau
      _yield();

      // On clignote en rouge pour indiquer l'attente
      LedRGBOFF();
      delay(100);
    }

    // Et on affiche nos paramètres
    Serial.println("Core Network settings");
    Serial.print("IP   : "); Serial.println(WiFi.localIP());
    Serial.print("Mask : "); Serial.println(WiFi.subnetMask());
    Serial.print("GW   : "); Serial.println(WiFi.gatewayIP());
    Serial.print("SSDI : "); Serial.println(WiFi.SSID());
    Serial.print("RSSI : "); Serial.print(WiFi.RSSI());Serial.println("dB");

    //  WebServer / Command
    //server.setDefaultCommand(&handleRoot);
    //webserver.addCommand("json", &sendJSON);
    //webserver.addCommand("tinfojsontbl", &tinfoJSONTable);
    //webserver.setFailureCommand(&handleNotFound);

    // start the webserver
    //server.begin();

  #elif defined (ESP8266)
    // Init de la téléinformation
    Serial.begin(1200, SERIAL_7E1);

    // Connection au Wifi ou Vérification
    WifiHandleConn();
  #endif

  // Init bus I2C
  i2c_init();

  Serial.print("Compile avec les fonctions : ");

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

  #ifdef MOD_RF69
    // Initialisation RFM69 Module
    if ( rfm_setup())
      status |= STATUS_RFM ;
  #endif

  #ifdef MOD_TELEINFO
    // Initialiser la téléinfo et attente d'une trame valide
    // Le status est mis à jour dans les callback de la teleinfo
    tinfo_setup(true);
  #endif

  // Led verte durant le test
  LedRGBON(COLOR_GREEN);

  // Enclencher le relais 1 seconde
  // si dispo sur la carte
  #ifndef REMORA_BOARD_V10
    Serial.print("Relais=ON   ");
    relais("1");
    for (uint8_t i=0; i<20; i++)
    {
      delay(10);
      // Ne pas bloquer la reception et
      // la gestion de la téléinfo
      #ifdef MOD_TELEINFO
        tinfo_loop();
      #endif
    }
    Serial.println("Relais=OFF");
    relais("0");
  #endif

  // nous avons fini, led Jaune
  LedRGBON(COLOR_YELLOW);

  // Hors gel, désactivation des fils pilotes
  initFP();

  // On etteint la LED embarqué du core
  LedRGBOFF();

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
  static bool refreshDisplay = false;
  static bool lastcloudstate;
  static unsigned long previousMillis = 0;  // last time update
  unsigned long currentMillis = millis();
  bool currentcloudstate ;

  // Gérer notre compteur de secondes
  if ( millis()-previousMillis > 1000) {
    // Ceci arrive toute les secondes écoulées
    previousMillis = currentMillis;
    uptime++;
    refreshDisplay = true ;
  }

  #ifdef MOD_TELEINFO
    // Vérification de la reception d'une 1ere trame téléinfo
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

  #if defined (SPARK)
  // recupération de l'état de connexion au cloud SPARK
  currentcloudstate = Spark.connected();
  #elif defined (ESP8266)
  // recupération de l'état de connexion au Wifi
  currentcloudstate = WiFi.status()==WL_CONNECTED ? true:false;
  #endif

  // La connexion cloud vient de chager d'état ?
  if (lastcloudstate != currentcloudstate)
  {
    // Mise à jour de l'état
    lastcloudstate=currentcloudstate;

    // on vient de se reconnecter ?
    if (currentcloudstate)
    {
      // on pubie à nouveau nos affaires
      // Plus necessaire
      #ifdef SPARK
      // spark_expose_cloud();
      #endif

      // led verte
      LedRGBON(COLOR_GREEN);
    }
    else
    {
      // on compte la deconnexion led rouge
      my_cloud_disconnect++;
      Serial.print("Perte de conexion au cloud #");
      Serial.println(my_cloud_disconnect);
      LedRGBON(COLOR_RED);
    }
  }


  #ifdef SPARK
  char buff[64];
  int len = 64;

  // process incoming connections one at a time forever
  server.processConnection(buff, &len);
  #endif


  // Connection au Wifi ou Vérification
  #ifdef ESP8266
  WifiHandleConn();
  #endif

}
