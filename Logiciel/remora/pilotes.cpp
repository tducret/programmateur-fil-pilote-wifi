// **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//
// **********************************************************************************

#include "pilotes.h"

#if defined (REMORA_BOARD_V10)
  int SortiesFP[NB_FILS_PILOTES*2] = { FP1,FP2,FP3,FP4,FP5,FP6,FP7 };
#else
  int SortiesFP[NB_FILS_PILOTES*2] = { FP1,FP2,FP3,FP4,FP5,FP6 };
#endif
char etatFP[NB_FILS_PILOTES+1] = "";
char memFP[NB_FILS_PILOTES+1] = ""; //Commandes des fils pilotes mémorisées (utile pour le délestage/relestage)
int nivDelest = 0; // Niveau de délestage actuel (par défaut = 0, pas de délestage)
// Correspond au nombre de fils pilotes délestés (entre 0 et nombre de zones)
uint8_t plusAncienneZoneDelestee = 1;
// Numéro de la zone qui est délestée depuis le plus de temps (entre 1 et nombre de zones)
// C'est la première zone à être délestée
unsigned long timerDelestRelest = 0; // Timer de délestage/relestage

unsigned long counterHighStateFP[NB_FILS_PILOTES]; //Compteur de secondes dans l'état haut uniquement
unsigned long counterLowStateFP[NB_FILS_PILOTES];  //Compteur de secondes dans l'état bas uniquement

#ifdef SPARK
  Timer* ptConfort12Timer; //Timer pour la mise à jour des compteurs toutes les secondes
#endif
#ifdef ESP8266
  os_timer_t* ptConfort12Timer; //Timer pour la mise à jour des compteurs toutes les secondes
#endif

#if defined (REMORA_BOARD_V12)
  // Instanciation de l'I/O expander
  Adafruit_MCP23017 mcp;
#endif

/* ======================================================================
Function: setfp
Purpose : selectionne le mode d'un des fils pilotes
Input   : commande numéro du fil pilote + commande optionelle
          C=Confort, A=Arrêt, E=Eco, H=Hors gel, 1=Confort-1, 2=Confort-2
          ex: 1A => FP1 Arrêt
              41 => FP4 confort -1
              6C => FP6 confort
              72 => FP7 confort -2
          Si la commande est absente la fonction retourne l'état du FP
          ex: 1 => si état FP1 est "arret" retourne code ASCII du "A" (65)
Output  : 0 ou etat commande, si ok -1 sinon
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int setfp(String command)
{
  command.trim();
  command.toUpperCase();

  Serial.print("setfp=");
  Serial.println(command);

  int returnValue = -1;

  // Vérifier que l'on demande l'état d'un seul fil pilote
  if (command.length() == 1)
  {
    uint8_t fp = command[0]-'0';

    // demande valide
    // retourner l'état du fil pilote (char)
    if (fp >= 1 && fp <= NB_FILS_PILOTES)
      returnValue = (etatFP[fp-1])  ;
  }
  else if (command.length() == 2)
  {
    // numéro du fil pilote concerné, avec conversion ASCII > entier
    // la commande est vérifiée dans fpC, pas besoin de traiter ici
    uint8_t fp = command[0]-'0';
    char cOrdre= command[1];
    if ( (fp < 1 || fp > NB_FILS_PILOTES) ||
        (cOrdre!='C' && cOrdre!='E' && cOrdre!='H' && cOrdre!='A' && cOrdre!='1' && cOrdre!='2' ))
    {
        // erreur
        Serial.println("Argument incorrect");
    }
    else
    {
      memFP[fp-1] = cOrdre; // On mémorise toujours la commande demandée
      char cOrdreEnCours = etatFP[fp-1]; // Quel est l'état actuel du fil pilote?
      if (cOrdreEnCours != 'D')
      {
        // Si un délestage est en cours sur ce fil pilote, on n'exécute pas la commande
        // La commande est néanmoins mémorisé
        // Elle sera appliquée lors du relestage
        returnValue = setfp_interne(fp, cOrdre);
      }
    }
  }
  return(returnValue);
}

/* ======================================================================
Function: setfp_interne
Purpose : selectionne le mode d'un des fils pilotes
Input   : numéro du fil pilote (1 à NB_FILS_PILOTE)
          ordre à appliquer
          C=Confort, A=Arrêt, E=Eco, H=Hors gel, 1=Confort-1, 2=Confort-2, D=Délestage
          ex: 1,'A' => FP1 Arrêt
              4,'1' => FP4 confort -1
              6,'C' => FP6 confort
              7,'2' => FP7 confort -2
              5,'D' => FP5 délestage (=> hors-gel et blocage des nouvelles commandes)
Output  : 0 si ok -1 sinon
Comments: non exposée par l'API spark car on y gère le délestage
====================================================================== */
int setfp_interne(uint8_t fp, char cOrdre)
{
  // Vérifier que le numéro du fil pilote ne dépasse le MAX et
  // que la commande est correcte
  // Pour le moment les ordres Confort-1 et Confort-2 ne sont pas traités
  // 'D' correspond à délestage

  Serial.print("setfp_interne : fp=");
  Serial.print(fp);
  Serial.print(" ; cOrdre=");
  Serial.println(cOrdre);

  if ( (fp < 1 || fp > NB_FILS_PILOTES) ||
      (cOrdre!='C' && cOrdre!='E' && cOrdre!='H' && cOrdre!='A' && cOrdre!='1' && cOrdre!='2' && cOrdre!='D') )
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
    etatFP[fp-1]=cOrdre;
    Serial.print("etatFP=");
    Serial.println(etatFP);

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
        // Confort - 1 => 1/1 pendant 3 secondes puis 0/0 pendant 257 secondes
        case '1':
        // Confort - 2 => 1/1 pendant 7 secondes puis 0/0 pendant 253 secondes
        case '2':
        {
          // on commence par un niveau haut
          fpcmd1=HIGH; fpcmd2=HIGH;

          counterHighStateFP[fp-1] = 1;
          counterLowStateFP[fp-1]  = 0;
        }
        break;
        // Délestage => Hors gel => Commande 1/0
        case 'D': fpcmd1=HIGH; fpcmd2=LOW;  break;
    }

    // On positionne les sorties physiquement
    _digitalWrite(SortiesFP[2*(fp-1)], fpcmd1);
    _digitalWrite(SortiesFP[2*(fp-1)+1], fpcmd2);
    return (0);
  }
}

/* ======================================================================
Function: updateFPCounter
Purpose : met à jour les compteurs et change d'état les fils pilotes en mode Confort-1 et Confort-2
Input   : -
Output  : -
Comments: -
====================================================================== */
void updateFPCounter(_timer_callback_arg)
{
  for (uint8_t i=0; i<NB_FILS_PILOTES; i+=1)
  {
    switch (etatFP[i])
    {
        // Confort - 1
        case '1':
        {
          if ( counterLowStateFP[i] > 0)
          { // si on est dans l'état bas
            if ( counterLowStateFP[i] >= 297)
            { // si on a atteint le temps de la periode basse, passage en periode haute
              _digitalWrite(SortiesFP[2*i]  , HIGH);
              _digitalWrite(SortiesFP[2*i+1], HIGH);

              counterHighStateFP[i] = 1;
              counterLowStateFP[i]  = 0;
            }
            else
            { // sinon incrémentation du compteur bas
              counterHighStateFP[i]  = 0;
              counterLowStateFP[i]  += 1;
            }
          }
          else
          { // sinon on est dans l'état haut
            if ( counterHighStateFP[i] >= 3)
            { // si on a atteint le temps de la periode haute, passage en periode basse
              _digitalWrite(SortiesFP[2*i]  , LOW);
              _digitalWrite(SortiesFP[2*i+1], LOW);

              counterHighStateFP[i] = 0;
              counterLowStateFP[i]  = 1;
            }
            else
            { // sinon incrémentation du compteur haut
              counterHighStateFP[i] += 1;
              counterLowStateFP[i]   = 0;
            }
          }
        }
        break;
        // Confort - 2
        case '2':
        {
          if ( counterLowStateFP[i] > 0)
          { // si on est dans l'état bas
            if ( counterLowStateFP[i] >= 293)
            { // si on a atteint le temps de la periode basse, passage en periode haute
              _digitalWrite(SortiesFP[2*i]  , HIGH);
              _digitalWrite(SortiesFP[2*i+1], HIGH);

              counterHighStateFP[i] = 1;
              counterLowStateFP[i]  = 0;
            }
            else
            { // sinon incrémentation du compteur bas
              counterHighStateFP[i]  = 0;
              counterLowStateFP[i]  += 1;
            }
          }
          else
          { // sinon on est dans l'état haut
            if ( counterHighStateFP[i] >= 7)
            { // si on a atteint le temps de la periode haute, passage en periode basse
              _digitalWrite(SortiesFP[2*i]  , LOW);
              _digitalWrite(SortiesFP[2*i+1], LOW);

              counterHighStateFP[i] = 0;
              counterLowStateFP[i]  = 1;
            }
            else
            { // sinon incrémentation du compteur haut
              counterHighStateFP[i] += 1;
              counterLowStateFP[i]   = 0;
            }
          }
        }
        break;
        default:
        { // on réinitialise dans les autres modes
          counterHighStateFP[i] = 0;
          counterLowStateFP[i]  = 0;
        }
        break;
    }
  }
}

/* ======================================================================
Function: initFP
Purpose : met tous les fils pilotes en mode hors-gel
Input   : -
Output  : -
Comments: -
====================================================================== */
void initFP(void)
{
  // buffer contenant la commande à passer à setFP
  char cmd[] = "xH" ;

  // On positionne tous les FP en Hors-Gel et on initialise les compteurs à 0
  for (uint8_t i=1; i<=NB_FILS_PILOTES; i+=1)
  {
    cmd[0]='0' + i;
    setfp(cmd);

    counterHighStateFP[i-1] = 0;
    counterLowStateFP[i-1]  = 0;
  }

  // lancement du timer pour la gestion des modes Confort-1 et Confort-2
  #ifdef SPARK
    ptConfort12Timer = new Timer(1000, updateFPCounter );
    ptConfort12Timer->start();
  #endif
  #ifdef ESP8266
    ptConfort12Timer = new os_timer_t;
    os_timer_setfn(ptConfort12Timer, updateFPCounter, NULL);
    os_timer_arm(ptConfort12Timer, 1000, true);
  #endif

}

/* ======================================================================
Function: delester1zone
Purpose : déleste une zone de plus
Input   : variables globales nivDelest et plusAncienneZoneDelestee
Output  : màj variable globale nivDelest
Comments: -
====================================================================== */
void delester1zone(void)
{
  uint8_t numFp; // numéro du fil pilote à délester

  Serial.print("delester1zone() : avant : nivDelest=");
  Serial.print(nivDelest);
  Serial.print(" ; plusAncienneZoneDelestee=");
  Serial.println(plusAncienneZoneDelestee);

  if (nivDelest < NB_FILS_PILOTES) // On s'assure que l'on n'est pas au niveau max
  {
    nivDelest += 1;
    numFp = ((plusAncienneZoneDelestee-1 + nivDelest-1) % NB_FILS_PILOTES)+1;
    setfp_interne(numFp, 'D');
  }

  Serial.print("delester1zone() : apres : nivDelest=");
  Serial.print(nivDelest);
  Serial.print(" ; plusAncienneZoneDelestee=");
  Serial.println(plusAncienneZoneDelestee);
}

/* ======================================================================
Function: relester1zone
Purpose : retire le délestage d'une zone
Input   : variables globales nivDelest et plusAncienneZoneDelestee
Output  : màj variable globale nivDelest et plusAncienneZoneDelestee
Comments: -
====================================================================== */
void relester1zone(void)
{
  uint8_t numFp; // numéro du fil pilote à passer HORS-GEL

  Serial.print("relester1zone() : avant : nivDelest=");
  Serial.print(nivDelest);
  Serial.print(" ; plusAncienneZoneDelestee=");
  Serial.println(plusAncienneZoneDelestee);

  if (nivDelest > 0) // On s'assure qu'un délestage est en cours
  {
    nivDelest -= 1;
    numFp = plusAncienneZoneDelestee;
    char cOrdreMemorise = memFP[numFp-1]; //On récupére la dernière valeur de commande pour cette zone
    setfp_interne(numFp,cOrdreMemorise);
    plusAncienneZoneDelestee = (plusAncienneZoneDelestee % NB_FILS_PILOTES) + 1;
  }

  Serial.print("relester1zone() : apres : nivDelest=");
  Serial.print(nivDelest);
  Serial.print(" ; plusAncienneZoneDelestee=");
  Serial.println(plusAncienneZoneDelestee);
}

/* ======================================================================
Function: decalerDelestage
Purpose : fait tourner la ou les zones délestées
Input   : variables globales nivDelest et plusAncienneZoneDelestee
Output  : màj variable globale plusAncienneZoneDelestee
Comments: -
====================================================================== */
void decalerDelestage(void)
{
  Serial.print("decalerDelestage() : avant : nivDelest=");
  Serial.print(nivDelest);
  Serial.print(" ; plusAncienneZoneDelestee=");
  Serial.println(plusAncienneZoneDelestee);

  if (nivDelest > 0 && nivDelest < NB_FILS_PILOTES)
  // On ne peut pas faire tourner les zones délestées s'il n'y en a aucune en cours
  // de délestage, ou si elles le sont toutes
  {
    relester1zone();
    delester1zone();
  }

  Serial.print("decalerDelestage() : apres : nivDelest=");
  Serial.print(nivDelest);
  Serial.print(" ; plusAncienneZoneDelestee=");
  Serial.println(plusAncienneZoneDelestee);
}

/* ======================================================================
Function: fp
Purpose : selectionne le mode d'un ou plusieurs les fils pilotes d'un coup
Input   : liste des commandes
          -=rien, C=Confort, A=Arrêt, E=Eco, H=Hors gel, 1=Confort-1, 2=Confort-2,
          ex: 1A => FP1 Arrêt
              CCCCCCC => Commande tous les fils pilote en mode confort (ON)
              AAAAAAA => Commande tous les fils pilote en mode arrêt
              EEEEEEE => Commande tous les fils pilote en mode éco
              CAAAAAA => Tous OFF sauf le fil pilote 1 en confort
              A-AAAAA => Tous OFF sauf le fil pilote 2 inchangé
              E-CHA12 => FP2 Eco  , FP2 inchangé, FP3 confort, FP4 hors gel
                        FP5 arrêt, FP6 Confort-1    , FP7 Confort-2
Output  : 0 si ok -1 sinon
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int fp(String command)
{
  command.trim();
  command.toUpperCase();

  Serial.print("fp=");
  Serial.println(command);


  // Vérifier que l'on a la commande de tous les fils pilotes
  if (command.length() != NB_FILS_PILOTES)
  {
      return(-1) ;
  }
  else
  {
    int8_t returnValue = 0; // Init à 0 => OK
    char   cmd[] = "xx" ; // buffer contenant la commande à passer à setFP

    // envoyer les commandes pour tous les fils pilotes
    for (uint8_t i=1; i<=NB_FILS_PILOTES; i++)
    {
      cmd[0] = '0' + i ;
      cmd[1] = command[i-1]; // l'index de la chaine commence à 0 donc i-1

      // Si on ne doit pas laisser le fil pilote inchangé
      if (cmd[1] != '-' )
      {
        // ok ici au cas ou la commande setFP n'est pas bonne
        // on positionne le code de retour à -1 mais on
        // continue le traitement, les suivantes sont
        // peut-être correctes
        if (setfp(cmd) == -1)
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
  uint8_t cmd = command[0];

  Serial.print("relais=");
  Serial.println(command);

  // Vérifier que l'on a la commande d'un seul caractère
  if (command.length()!=1 || (cmd!='1' && cmd!='0'))
    return (-1);

  // Conversion en 0,1 numerique
  etatrelais= cmd - '0';

    // Allumer/Etteindre le relais et la LED
  #ifdef RELAIS_PIN
    _digitalWrite(RELAIS_PIN, etatrelais);
  #endif
  #ifdef LED_PIN
    _digitalWrite(LED_PIN, etatrelais);
  #endif

  return (etatrelais);
}

/* ======================================================================
Function: pilotes_Setup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : true if MCP23017 module found, false otherwise
Comments: -
====================================================================== */
bool pilotes_setup(void)
{
  // Cartes Version 1.0 et 1.1 pilotage part port I/O du spark
  #if defined (REMORA_BOARD_V10) || defined (REMORA_BOARD_V11)

    // 2*nbFilPilotes car 2 pins pour commander 1 fil pilote
    for (uint8_t i=0; i < (NB_FILS_PILOTES*2); i++)
      _pinMode(SortiesFP[i], OUTPUT); // Chaque commande de fil pilote est une sortie

    #ifdef RELAIS_PIN
      _pinMode(RELAIS_PIN, OUTPUT);
    #endif
    #ifdef LED_PIN
      _pinMode(LED_PIN, OUTPUT);
    #endif

  // Cartes Version 1.2+ pilotage part I/O Expander
  #else
    Serial.print("Initializing MCP23017...Searching...");
    Serial.flush();

    // Détection du MCP23017
    if (!i2c_detect(MCP23017_ADDRESS))
    {
      Serial.println("Not found!");
      Serial.flush();
      return (false);
    }
    else
    {
      Serial.print("Setup...");
      Serial.flush();

      // et l'initialiser
      mcp.begin();

      // Mettre les 16 I/O PIN en sortie
      mcp.writeRegister(MCP23017_IODIRA,0x00);
      mcp.writeRegister(MCP23017_IODIRB,0x00);
      Serial.println("OK!");
      Serial.flush();
    }
  #endif

  // ou l'a trouvé
  return (true);
}
