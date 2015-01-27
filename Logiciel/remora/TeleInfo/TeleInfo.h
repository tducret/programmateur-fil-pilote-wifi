/*
TeleInfo - une bibliothèque de décodage des trames du compteur électrique pour Spark Core
Inspiré de la philo de la bibliothèque TinyGPS de Mikal Hart
D'après le document ERDF-NOI-CPT_02E - Version 2 - 01/03/2008
Copyright (C) 2014 Thibault Ducret
Licence MIT
*/
#ifndef TeleInfo_h
#define TeleInfo_h

#include <stdlib.h>
#include <string.h>

#define TAILLE_MAX_CHAINE_ETIQ 8 // taille maximale pour la chaine de caractères d'une étiquette
#define TAILLE_MAX_CHAINE_DONNEE 12 // taille maximale pour la chaine de caractères d'une donnée
#define NB_MAX_GROUPES_DANS_TRAME 30 // nombre maximum de groupes (étiquette+donnée) dans une trame
#define TELEINFO_CHAINE_INVALIDE "NNNNNNNNNNNN"
#define TELEINFO_UINT_INVALIDE 65535

typedef struct {
    char etiquette[TAILLE_MAX_CHAINE_ETIQ+1]; // +1 pour ajouter un '\0' à la fin de la chaine
    char donnee[TAILLE_MAX_CHAINE_DONNEE+1]; // idem
} GroupeTeleInfo;

class TeleInfo
{

public:

  // Fonctions
  TeleInfo();
  bool decode(char c); // traiter un caractère reçu par la liaison TéléInfo
  TeleInfo &operator << (char c) {decode(c); return *this;}

  // adresse du compteur
  inline char *adCompteur() { return _adCompteur; }

  // option tarifaire
  inline char *opTarif() { return _opTarif; }

  // intensité souscrite (en A)
  inline unsigned int iSousc() { return _iSousc; }

  // index option Heures Creuses (en Wh)
  inline unsigned int indexHC() { return _indexHC; }

  // index option Heures Pleines (en Wh)
  inline unsigned int indexHP() { return _indexHP; }

  // période tarifaire en cours
  inline char *perTarif() { return _perTarif; }

  // intensité instantanée (en A)
  inline unsigned int iInst() { return _iInst; }

  // avertissement de dépassement de puissance souscrite (en A)
  inline char *avertDep() { return _avertDep; }

  // intensité maximale appelée (en A)
  inline unsigned int iMax() { return _iMax; }

  // puissance apparente (en VA)
  inline unsigned int pApp() { return _pApp; }

  // type d'horaire heures pleines heures creuses
  inline char *typeHoraireHPHC() { return _typeHoraireHPHC; }

  // mot d'état du compteur
  inline char *motEtat() { return _motEtat; }

private:
  enum {_0_ATTENDRE_DEBUT_TRAME=0, _1_ATTENDRE_DEBUT_GROUPE=1,
  		_2_CREER_ETIQUETTE=2, _3_CREER_DONNEE=3, _4_VERIFIER_CHECKSUM=4,
  		_5_ATTENDRE_GROUPE_OU_FIN_TRAME=5, _10_TRAITER_ERREUR=10};

  // propriétés
  char _adCompteur[TAILLE_MAX_CHAINE_DONNEE], _new_adCompteur[TAILLE_MAX_CHAINE_DONNEE];
  char _opTarif[TAILLE_MAX_CHAINE_DONNEE], _new_opTarif[TAILLE_MAX_CHAINE_DONNEE];
  unsigned int _iSousc, _new_iSousc;
  unsigned int _indexHC, _new_indexHC;
  unsigned int _indexHP, _new_indexHP;
  char _perTarif[TAILLE_MAX_CHAINE_DONNEE], _new_perTarif[TAILLE_MAX_CHAINE_DONNEE];
  unsigned int  _iInst, _new_iInst;
  char  _avertDep[TAILLE_MAX_CHAINE_DONNEE], _new_avertDep[TAILLE_MAX_CHAINE_DONNEE];
  unsigned int _iMax, _new_iMax;
  unsigned int _pApp, _new_pApp;
  char _typeHoraireHPHC[TAILLE_MAX_CHAINE_DONNEE], _new_typeHoraireHPHC[TAILLE_MAX_CHAINE_DONNEE];
  char _motEtat[TAILLE_MAX_CHAINE_DONNEE], _new_motEtat[TAILLE_MAX_CHAINE_DONNEE];

  // variables de la machine à états de décodage de la trame
  unsigned int _etat, _nbCarEtat2, _nbCarEtat3, _nbCarEtat4;
  char _etiquette[TAILLE_MAX_CHAINE_ETIQ];
  char _donnee[TAILLE_MAX_CHAINE_DONNEE];
  char _checksum;
  GroupeTeleInfo _tableauGroupes[NB_MAX_GROUPES_DANS_TRAME];
  unsigned int _nbGroupes;

  // fonctions internes
  void affecterVariables();
};

#endif
