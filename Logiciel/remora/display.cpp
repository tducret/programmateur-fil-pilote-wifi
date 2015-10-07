// **********************************************************************************
// OLED display management source file for remora project
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-01-22 - First release
//
// 15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//
// All text above must be included in any redistribution.
// **********************************************************************************
#include "display.h"

// Instantiate OLED (no reset pin)
Adafruit_SSD1306 display(-1);

// Différents état de l'affichage possible
const char * screen_name[] = {"RF", "System", "Teleinfo"};
screen_e screen_state;

/* ======================================================================
Function: displaySplash
Purpose : display setup splash screen OLED screen
Input   : -
Output  : -
Comments: -
====================================================================== */
void display_splash(void)
{
  display.clearDisplay() ;
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.print("  REMORA\n");
  display.print("Fil Pilote\n");
  display.setTextSize(1);
  //display.print(WiFi.localIP());
  display.display();
}

/* ======================================================================
Function: displaySys
Purpose : display Téléinfo related information on OLED screen
Input   : -
Output  : -
Comments: -
====================================================================== */
void displayTeleinfo(void)
{
  uint percent = 0;

  // Effacer le buffer de l'affichage
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  // si en heure pleine inverser le texte sur le compteur HP
  if (ptec == PTEC_HP )
    display.setTextColor(BLACK, WHITE); // 'inverted' text

  display.print("Pleines ");
  display.printf("%09ld\n", myindexHP);
  display.setTextColor(WHITE); // normaltext

  // si en heure creuse inverser le texte sur le compteur HC
  if (ptec == PTEC_HC )
    display.setTextColor(BLACK, WHITE); // 'inverted' text

  display.print("Creuses ");
  display.printf("%09ld\n", myindexHC);
  display.setTextColor(WHITE); // normaltext

  // Poucentrage de la puissance totale
  percent = (uint) myiInst * 100 / myisousc ;

  //Serial.print("myiInst="); Serial.print(myiInst);
  //Serial.print("  myisousc="); Serial.print(myisousc);
  //Serial.print("  percent="); Serial.println(percent);

  // Information additionelles
  display.printf("%d W %d%%  %3d A", mypApp, percent, myiInst);

  // etat des fils pilotes
  display.setCursor(0,32);
  display.setTextSize(2);
  #ifdef SPARK
  display.printf("%02d:%02d:%02d",Time.hour(),Time.minute(),Time.second());
  #endif

  display.setCursor(0,48);
  display.printf("%s  %c", etatFP, etatrelais+'0' );

  // Bargraphe de puissance
  display.drawVerticalBargraph(114,0,12,40,1, percent);

  display.setTextColor(BLACK, WHITE); // 'inverted' text
}

/* ======================================================================
Function: displaySys
Purpose : display system related information on OLED screen
Input   : -
Output  : -
Comments: -
====================================================================== */
void displaySys(void)
{
  // To DO
}


/* ======================================================================
Function: displayRf
Purpose : display RF related information on OLED screen
Input   : -
Output  : -
Comments: -
====================================================================== */
void displayRf(void)
{/*
  int16_t percent;

  display.printf("RF69 G%d I%d", NETWORK_ID, NODE_ID);

  // rssi range 0 (0%) to 115 (100%)
  percent = ((module.rssi+115) * 100) / 115;

  //displayClearline(current_line);
  //display.setCursor(0,current_line);
  //display.print(F("["));
  //display.print(module.size);
  display.printf("G%d I%d %d%%\n", module.groupid, module.nodeid,percent);
  display.printf("[%02X] ", module.size);

  byte n = module.size;
  uint8_t * p = module.data;

  // Max 4 data per line on LCD
  if (n>12)
    n=12;

  for (byte i = 0; i < n; ++i)
    display.printf("%02X ", *p++);

  //display.drawHorizontalBargraph(106,current_line+1, 22, 6, 1, percent);

  display.printf("%d%% ", percent);
  */
}

/* ======================================================================
Function: display_setup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : true if OLED module found, false otherwise
Comments: -
====================================================================== */
bool display_setup(void)
{
  bool ret = false;

  Serial.print("Initializing OLED...Searching...");
  Serial.flush();

  // Par defaut affichage des infos de téléinfo
  screen_state = screen_teleinfo;

  // Init et detection des modules I2C
  if (!i2c_detect(OLED_I2C_ADDRESS)) {
    Serial.println("Not found!");
  } else {
    Serial.print("Setup...");
    Serial.flush();

    // initialize with the I2C addr for the 128x64
    display.begin(OLED_I2C_ADDRESS);
    display.clearDisplay() ;
    display.display();
    Serial.println("OK!");
    ret = true;
  }

  Serial.flush();

  return (ret);
}

/* ======================================================================
Function: display_loop
Purpose : main loop for OLED display
Input   : -
Output  : -
Comments: -
====================================================================== */
void display_loop(void)
{
  // Si pas de heartbeat via la teleinfo, le faire
  // via l'affichage
  #ifndef MOD_TELEINFO
    LedRGBON(COLOR_BLUE);
  #endif

  display.setCursor(0,0);

  if (screen_state==screen_sys)
    displaySys();
  else if (screen_state==screen_rf)
    displayRf();
  else if (screen_state==screen_teleinfo)
    displayTeleinfo();

  // Affichage physique sur l'écran
  display.display();

  #ifndef MOD_TELEINFO
    LedRGBOFF();
  #endif

}
