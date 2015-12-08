# Tutoriel d'assemblage de Remora v1.3

<!--Liste de toutes les images utilisées-->
[Schéma implantation]: ../ProgrammateurFilPilote_1.3_Schematic.png
[PCB face avant]: ../ProgrammateurFilPilote_1.3_Recto.png
[PCB face arrière]: ../ProgrammateurFilPilote_1.3_Verso.png
[Le contenu du kit]: Images/Le_contenu_du_kit.jpg
[Outillage]: Images/Outillage.jpg


## Remora

![image](Images/Photo43.jpeg)

Remora, c'est un programmateur de chauffages par fils pilotes.

- Il peut contrôler 7 zones et un chauffe-eau (via 1 relais)
- Il se commande via des requêtes HTTP en WiFi
- Il est open source et open hardware


## Contenu du kit
![Le contenu du kit]

- 1 NodeMCU
- 1 Boîtier RAIL DIN (4U : 71 x 90 x 58 mm)
- 1 Circuit imprimé Remora (v1.3)
- 1 Circuit imprimé Adaptateur NodeMCU->Remora (v1.1a)
- 1 Alimentation 5V RAIL DIN (1U - 12W)
- 14 Optotriacs
- 15 Diodes
- 1 MCP23017
- 1 Support MCP23017
- 2 Connecteur 1x12pt femelle Remora->NodeMCU
- 1 Barrette 40 pins male
- 1 Barrette 40 pins femelle
- 1 LED rouge 3mm
- 1 Transistors NPN
- 1 Led RGB 5mm
- 1 Relais
- 2 Borniers 3pts
- 6 Borniers 2 pts
- 1 Optocoupleur Teleinfo
- 1 Transistor BSS138
- 4 Resistance 10KΩ (format 1206)
- 5 Resistance 4.7KΩ (format 1206)
- 15 Resistance 390Ω (format 1206)
- 2 Condensateur 100nF (format 1206)
- 1 Connecteur 1x4pt OLED femelle
- 1 Connecteur Grove


## Outillage

Pour réaliser le module, vous aurez besoin de :

- 1 Fer à souder à pointe fine [^1]
[^1]: J'utilise un Antex XS25.
- 1 Bobine de fil d'étain (de préférence 0,5 mm)
- 1 Multimètre (pour vérifier les soudures)
- 1 Pince coupante
- 1 Petite pince (ou brucelle)
- 1 troisième main (facultatif mais bien utile) 

![Outillage]


## Schéma d'implantation

![Schéma implantation]

*Nota : Il est bien utile d'avoir un exemplaire imprimé du schéma d'implantation près de soi lorsque l'on fait les soudures*

## Le circuit imprimé

![PCB face avant]  
**Face avant**

![PCB face arrière]  
**Face arrière**

Identifiant du composant	| Description
--------------------------	| -----------------------------
R1, R17, R19, R20			| Résistance de 4,7KΩ
R2, R18, R22				| Résistances de 10KΩ
R3 à R16, R21				| Résistances de 390Ω
D1 à D15					| Diodes de redressement
T1							| Transistor NPN (2N3904)
T2							| Transistor Opto BSS138
OP1 à OP14					| Optotriacs
C1, C2						| Condesateurs 100nF
IC1							| MCP23017
OK1							| Optocoupleur LTV-814S


Non utilisés dans le kit : R23, R24



## Soudure

L'objectif est de souder les composants dans un ordre nous permettant de ne pas être gêné.  
La stratégie est simple : **on va souder les petits composants avant les gros, en commençant par la face avant, puis la face arrière.**

### Face avant

----------

##### Soudure des diodes D1, D3, D5, D7, D9, D11 et D13
'M7' est indiqué sur chacune des diodes. La barre à gauche du 'M' indique la cathode.  
**Il faut souder chaque diode de cette face en mettant la barre du côté de D1, D3...**
![image](Images/Photo1.jpeg)

**Astuce de soudure CMS**
J'utilise ma "troisième main" et une simple pointe pour maintenir les composants de petite taille
![image](Images/Photo2.jpeg)

*Autre astuce, la favorite de Charles qui n'a pas de 3eme main*
Regardez cette [video][3] à 1M50 c'est beaucoup plus clair que mes explications (pas besoin de flux au pinceau, pas d'inquiètude) et très bien détaillé. 
Attenton contraitement à la vidéo, pour souder le 2eme pad il vaut mieux chauffer le pas plutot que mettre la soudure sur la panne du fer (sauf si vous avez du flux).

Donc en résumé :
- Soudure dans une main, fer dans l'autre et on met un peu de soudure sur une pastille du PCB de chaque composant à placer
- Pince + composant dans une main, fer dans l'autre et on les place en chauffaut la soudure dejà placée
- Soudure dans une main, fer dans l'autre et on soude les autres pastilles des composants déjà placés.


Voici ce que ça donne de plus près, retour à la méthode de Thibault
![image](Images/Photo3.jpeg)

Et une fois les 7 diodes terminées

![image](Images/Photo4.jpeg)

----------

##### Soudure des optotriacs OP1, OP3, OP5, OP7, OP9, OP11 et OP13
Il existe 2 sortes d'optotriacs. Leur fonctionnement et soudure sont indentiques.

Vous verrez 'P168J' (le composant est jaune)
![image](Images/Photo6.jpeg)
ou 'SF22S4' (le composant est noir)
![image](Images/Photo5.jpeg).

Pour les 2 sortes, un point est présent sur le coin en bas à gauche du composant.  
**Il faut bien s'assurer que le point du composant correspond au point sur le circuit imprimé**

On procède à la soudure comme précédemment
![image](Images/Photo7.jpeg)

Ce qui nous donne
![image](Images/Photo8.jpeg)

Et une fois les 7 optotriacs terminés
![image](Images/Photo9.jpeg)

----------

##### Soudure des résistances R3, R5, R7, R9, R11, R13 et R15 (390Ω)
Il est indiqué '391' dessus (comme 39x10^1=390Ω). Pas de sens à respecter.
![image](Images/Photo10.jpeg)

Et une fois les 7 résistances terminées
![image](Images/Photo11.jpeg)

----------

De la même manière, procédez aux soudures suivantes :

Soudure de la résistance R18 (10KΩ) : Il est indiqué '103' dessus 

Soudure de la résistance R17 (4,7KΩ) : Il est indiqué '472' dessus 

![image](Images/Photo12.jpeg)

----------

##### Soudure de la diode D15
'M7' est indiqué dessus. La barre à gauche du 'M' indique la cathode.  
**Il faut souder la diode en mettant la barre du côté du '7' de '1N4007**
![image](Images/Photo13.jpeg)

----------

Soudure de la résistance R2 (10KΩ) : Il est indiqué '103' dessus

Soudure du transistor T2 (**Très petit. Attention de bien mettre ses pattes sur la carte**)

Soudure de la résistance R22 (10KΩ) : Il est indiqué '103' dessus 

Soudure de la résistance R1 (4,7KΩ) : Il est indiqué '472' dessus 

Soudure de la résistance R21 (390Ω) : Il est indiqué '391' dessus
![image](Images/Photo14.jpeg)

----------

Soudure des condensateurs C1 et C2 : Pas de sens particulier
![image](Images/Photo15.jpeg)

----------

Soudure des résistances R19 et R20 (4,7KΩ) : Il est indiqué '472' dessus  
*Pas de photo*

### Face arrière

##### Soudure des diodes D2, D4, D6, D8, D10, D12, D14  
'M7' est indiqué sur chacune des diodes. La barre à gauche du 'M' indique la cathode. **Attention : Pour cette face, il faut souder chaque diode en mettant la barre du côté des borniers FP1, FP2...**

##### Soudure des optotriacs OP2, OP4, OP6, OP8, OP10, OP12, OP14 
**Il faut bien s'assurer que le point du composant correspond au point sur le circuit imprimé**

Soudure des résistances R4, R6, R8, R10, R12, R14, R16 (390Ω) : Il est indiqué '391' dessus 
![image](Images/Photo17.jpeg)

----------

Soudure du support pour IC1
![image](Images/Photo18.jpeg)
Et de l'autre côté
![image](Images/Photo19.jpeg)

----------

Soudure du transistor T1 (** Attention à la polarité : il faut repérer la forme arrondie du composant **)
![image](Images/Photo20.jpeg)

On soude les pattes du transistor de l'autre côté, puis on coupe les bouts qui dépassent. 
![image](Images/Photo22.jpeg)
**On garde un bout de patte pour la suite**

### Zoom sur l'adaptateur NodeMCU vers Particle Photon/Spark Core

On soude le bout de patte de transistor coupé pour faire un pont sur JP2
![image](Images/Photo21.jpeg)

On découpe un bout de barrette male pour faire 2 barrettes de 12 pins chacune.  
Puis, on les insère dans les barrettes femelles de 12 pins.
![image](Images/Photo23.jpeg)

On insère ces barrettes dans l'adaptateur (**En veillant au côté de l'adaptateur choisi**)
![image](Images/Photo24.jpeg)

On insère le tout sur la carte Remora
![image](Images/Photo25.jpeg)

On soude les 24 pins
![image](Images/Photo26.jpeg)

Pui, on retourne la carte avec précaution
![image](Images/Photo27.jpeg)

Et on soude les 24 pins du dessous
![image](Images/Photo28.jpeg)

Une fois les soudures sèches, on retire l'adaptateur de la carte Remora.  
On insère le NodeMCU sur l'adaptateur
![image](Images/Photo29.jpeg)

On retourne l'adaptateur
![image](Images/Photo30.jpeg)

On soude les 32 pins
![image](Images/Photo31.jpeg)

### Retour sur la carte Remora

Soudure du connecteur OLED
![image](Images/Photo32.jpeg)

----------

Soudure du connecteur Grove
![image](Images/Photo33.jpeg)

----------

On assemble 1 bornier de 3 points + 4 borniers de 2 points
![image](Images/Photo34.jpeg)

Puis on les soude
![image](Images/Photo35.jpeg)
![image](Images/Photo36.jpeg)

On soude ensuite le bornier de 3 points (5V, GND, 3V3) ainsi que le bornier de 2 points (I1, I2)

----------

Soudure du relais
![image](Images/Photo38.jpeg)

----------

Soudure de la petite LED rouge (**Observez bien le sens : la patte la plus longue est du côté des borniers I1, I2**)
![image](Images/Photo37.jpeg)

----------

Soudure de la grosse LED RGB (multicolore)
** Faire bien attention aux pins de cette LED **
![image](Images/Photo39.jpeg)

On coupe ensuite les pattes qui dépassent des 2 LEDs
![image](Images/Photo41.jpeg)

### Retour sur la face avant

Soudure de l'optocoupleur OK1 (**Attention à la polarité : le point sur le composant est en face du + sur la carte **)
![image](Images/Photo42.jpeg)

## Rendu final

### Face avant
![image](Images/Photo43.jpeg)

### Face arrière
![image](Images/Photo44.jpeg)

### Test des soudures

### Montage dans le tableau électrique

TODO

## Programmation du NodeMCU

Le logiciel à flasher dans le NodeMCU se trouve sur ce [repo github][1], c'est maintenant le nouveau repo officiel, l'ancien est toujour [disponible][2] pour historique.
TODO

## Test du module complet

TODO

[1]: https://github.com/hallard/remora_soft
[2]: https://github.com/thibdct/programmateur-fil-pilote-wifi/tree/master/Logiciel/remora
[3]: http://youtu.be/3NN7UGWYmBY