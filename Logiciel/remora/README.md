Version Logicielle pour toutes les versions du matériel
=======================================================

Cette version logicielle est compatible avec la version matérielle [1.2][1] ainsi que les anciennes versions des cartes electroniques. Elle est aussi compatible avec les cartes à base d'ESP8266 via un [adaptateur](5)

Installation
------------

- Télécharger l'environnement de développement [Particle-Dev][2] puis lancer l'IDE.
- Télécharger l'archive du repo logiciel [github][3] et le copier sur votre disque dur puis le décompresser
- Une fois l'IDE ouvert, menu File/Open et ouvrir le dossier programmateur-fil-pilote-wifi-master\Logiciel\remora
- Dans le menu Particle/Log in to Particle Cloud, entrez vos identifiants Particle pour vous connecter
- Dans le menu Particle/Select Device selectionnez votre Spark Core ou Particle Photon
- Ouvrir ensuite depuis l'IDE les fichers remora.ino et remora.h
- Selectionner la version de carte utilisé dans le fichier remora.h (les defines REMORA_BOARD_Vxx)
- Selectionner les modules utilisés dans le fichier remora.h (les defines MOD_xxxx)
- Selectionner l'onglet remora.ino
- Lancer la compilation+upload (icone éclair en haut à gauche)
- Si vous avez une erreur de type "App code was invalid" durant la compilation, supprimer le ficher README.md du dossier

API Exposée
-----------

Description complète bientôt

A faire
-------

- Mettre des icones plus sympas sur l'afficheur
- Gérer les retour de sondes RF, pour le moment seules les trames sont affichées sur la serial, rien n'est encore fait
- Gérer le RF des sondes Oregon Scientific
- pour le moment seul les [OLED][4] I2C sont gérés
- tout autre idée est bienvenue

Historiques des Modifications
-----------------------------
16/02/2015 : Ajout délestage cascadocyclique / Possibilité de ne récupérer l'état que d'un seul fil pilote

14/04/2015 : Ajout d'une variable spark pour la teleinfo. Passage en un seul appel pour script jeedom. Les variables d'origine restent utilisables.

30/09/2015 : voir le post [dédié][6].   



Exemple
-------

Pour les photos la téléinfo n'était pas branchée sur la carte, c'est pour celà que les compteurs et le bargraphe sont à 0

<img src="https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/Logiciel/images/remora01.jpg" alt="Top"    width="45%" height="45%">&nbsp;
<img src="https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/Logiciel/images/remora02.jpg" alt="Bottom" width="45%" height="45%">
<br>

Avec la téléinfo branchée sur la carte, le bargraph indique la puissance instantanée par rapport au contrat souscrit, ici soit 11% car on consomme 5A sur un contrat de 45A. Nous sommes en heures pleines (affichage sur fond blanc)
<img align="center" src="https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/Logiciel/images/remora03.jpg" alt="Bottom" width="45%" height="45%">


[1]: https://github.com/thibdct/programmateur-fil-pilote-wifi/tree/master/Mat%C3%A9riel/1.2
[2]: https://www.particle.io/dev
[3]: https://github.com/thibdct/programmateur-fil-pilote-wifi/archive/master.zip
[4]: http://www.ebay.com/itm/291216700457
[5]: https://github.com/hallard/Particle2NodeMCU/blob/master/README.md
[6]: https://community.hallard.me/topic/92/nouvelle-version-remora-compatible-esp8266
