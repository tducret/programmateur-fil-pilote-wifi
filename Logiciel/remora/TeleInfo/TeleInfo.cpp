/*
TeleInfo - une bibliothèque de décodage des trames du compteur électrique pour Spark Core
Inspiré de la philo de la bibliothèque TinyGPS de Mikal Hart
D'après le document ERDF-NOI-CPT_02E - Version 2 - 01/03/2008
Copyright (C) 2014 Thibault Ducret
Licence MIT
*/

#include "application.h"
#include "TeleInfo.h"

//définition des étiquettes
#define _ETIQ_ADCOMPTEUR	"ADCO"
#define _ETIQ_OPTARIF		"OPTARIF"
#define _ETIQ_ISOUSC		"ISOUSC"
#define _ETIQ_INDEXHC		"HCHC"
#define _ETIQ_INDEXHP		"HCHP"
#define _ETIQ_PERTARIF		"PTEC"
#define _ETIQ_IINST			"IINST"
#define _ETIQ_AVERTDEP		"ADPS"
#define _ETIQ_IMAX			"IMAX"
#define _ETIQ_PAPP			"PAPP"
#define _ETIQ_TYPEHORHPHC	"HHPHC"
#define _ETIQ_MOTETAT		"MOTDETAT"


TeleInfo::TeleInfo()
  :  _iSousc(TELEINFO_UINT_INVALIDE)
  ,  _indexHC(TELEINFO_UINT_INVALIDE)
  ,  _indexHP(TELEINFO_UINT_INVALIDE)
  ,  _iInst(TELEINFO_UINT_INVALIDE)
  ,  _iMax(TELEINFO_UINT_INVALIDE)
  ,  _pApp(TELEINFO_UINT_INVALIDE)
{
  _adCompteur[0]=0;
  _opTarif[0]=0;
  _perTarif[0]=0;
  _avertDep[0]=0;
  _typeHoraireHPHC[0]=0;
  _motEtat[0]=0;
  _nbCarEtat2 = 0;
  _nbCarEtat3 = 0;
  _nbCarEtat4 = 0;
}

//
// méthodes publiques
//

bool TeleInfo::decode(char c)
{
  bool trame_valide = false;

  switch(_etat)
  {
  case _0_ATTENDRE_DEBUT_TRAME:
    if (c == 0x02) _etat=_1_ATTENDRE_DEBUT_GROUPE;
    // on reste dans l'état 0 si on n'a pas reçu le caractère de début de trame
    break;

  case _1_ATTENDRE_DEBUT_GROUPE:
    if (c == 0x0A) {
      _nbGroupes = 0;
      memset(_tableauGroupes,0,sizeof(_tableauGroupes)); // RAZ tableau des groupes
      _etat=_2_CREER_ETIQUETTE;
    }
    else {_etat=_10_TRAITER_ERREUR;}
    break;

  case _2_CREER_ETIQUETTE:
    if (_nbCarEtat2 == 0) // on arrive pour la première fois dans l'état 2
    {
      memset(_etiquette,0,sizeof(_etiquette)); // RAZ étiquette
    }

    if (c == 0x20) // fin de l'étiquette, on passe à l'état suivant
    {
      _nbCarEtat2 = 0;
      _etat=_3_CREER_DONNEE;
    }
    else if (_nbCarEtat2 < TAILLE_MAX_CHAINE_ETIQ)
    {
      _etiquette[_nbCarEtat2]=c; // concaténation avec le caractère reçu
      _nbCarEtat2 += 1;
    }
    else
    {
      _nbCarEtat2 = 0;
      _etat=_10_TRAITER_ERREUR;
    }
    break;

  case _3_CREER_DONNEE:
    if (_nbCarEtat3 == 0) // on arrive pour la première fois dans l'état 3
    {
      memset(_donnee,0,sizeof(_donnee)); // RAZ donnée
    }

    if (c == 0x20) // fin de la donnée, on passe à l'état suivant
    {
      _nbCarEtat3 = 0;
      _etat=_4_VERIFIER_CHECKSUM;
    }
    else if (_nbCarEtat3 < TAILLE_MAX_CHAINE_DONNEE)
    {
      _donnee[_nbCarEtat3]=c; // concaténation avec le caractère reçu
      _nbCarEtat3 += 1;
    }
    else
    {
      _nbCarEtat3 = 0;
      _etat=_10_TRAITER_ERREUR;
    }
    break;

  case _4_VERIFIER_CHECKSUM:
    if (_nbCarEtat4 == 0) // on arrive pour la première fois dans l'état 4
    {
      _checksum = c;
      _nbCarEtat4 += 1;
    }
    else if (c == 0x0D) // fin du checksum, on vérifie et on passe à l'état suivant
    {
      _nbCarEtat4 = 0;
      // TODO : vérification du checksum
      if (_nbGroupes >= NB_MAX_GROUPES_DANS_TRAME) // on ne doit pas dépasser la capacité du tableau des groupes
      {
        _etat=_10_TRAITER_ERREUR;
      }
      else
      {
        int8_t i ;
        unsigned char sum = 32 ;    // Somme des codes ASCII du message + un espace

        // Calcul de la checksum sur l'étiquette
        for (i=0; i < strlen(_etiquette); i++)
          sum = sum + _etiquette[i] ;

        // Calcul de la checksum sur la donnée
        for (i=0; i < strlen(_donnee); i++)
          sum = sum + _donnee[i] ;

        // Si la checksum est bonne, on ajoute le groupe dans le tableau
        // sinon ce groupe sera ignoré pour cette fois-ci
        if ( _checksum == ((sum & 63) + 32) )
        {
          memcpy(_tableauGroupes[_nbGroupes].etiquette,_etiquette,sizeof(_etiquette));
          memcpy(_tableauGroupes[_nbGroupes].donnee,_donnee,sizeof(_donnee));
          
          // on ajoute le groupe dans le tableau
          _nbGroupes += 1;
        }

        // et on passe à la suite
        _etat=_5_ATTENDRE_GROUPE_OU_FIN_TRAME;
      }
    }
    else
    {
      _nbCarEtat4 = 0;
      _etat=_10_TRAITER_ERREUR;
    }
    break;

  case _5_ATTENDRE_GROUPE_OU_FIN_TRAME:
    if (c == 0x0A) {_etat=_2_CREER_ETIQUETTE;} // début d'un nouveau groupe
    else if (c == 0x03) // fin de la trame
    { // TRAME VALIDE !
      trame_valide = true;
      affecterVariables();
      _etat=_0_ATTENDRE_DEBUT_TRAME;
    }
    else {_etat=_10_TRAITER_ERREUR;}
    break;

  case _10_TRAITER_ERREUR:
    // erreur
    _etat=_0_ATTENDRE_DEBUT_TRAME;
    break;
  }

  return trame_valide;
}

//
// fonctions internes
//

void TeleInfo::affecterVariables()
{
  unsigned int i;

  for (i=0; i<_nbGroupes; i+=1)
  {
	if (strcmp(_tableauGroupes[i].etiquette,_ETIQ_ADCOMPTEUR) == 0)
	{
  	    memcpy(_adCompteur,_tableauGroupes[i].donnee,sizeof(_adCompteur));
  	}
  	else if (strcmp(_tableauGroupes[i].etiquette,_ETIQ_PAPP) == 0)
  	{
  		_pApp=(unsigned int) atoi(_tableauGroupes[i].donnee);
  	}
  	else if (strcmp(_tableauGroupes[i].etiquette,_ETIQ_IINST) == 0)
  	{
  		_iInst=(unsigned int) atoi(_tableauGroupes[i].donnee);
  	}
  	else if (strcmp(_tableauGroupes[i].etiquette,_ETIQ_INDEXHC) == 0)
  	{
  		_indexHC=(unsigned int) atoi(_tableauGroupes[i].donnee);
  	}
  	else if (strcmp(_tableauGroupes[i].etiquette,_ETIQ_INDEXHP) == 0)
  	{
  		_indexHP=(unsigned int) atoi(_tableauGroupes[i].donnee);
  	}
    else if (strcmp(_tableauGroupes[i].etiquette,_ETIQ_PERTARIF) == 0)
    {
      memcpy(_perTarif,_tableauGroupes[i].donnee,sizeof(_ETIQ_PERTARIF));
    }
  }
}
