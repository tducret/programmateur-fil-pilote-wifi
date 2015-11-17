# Tutoriel d'assemblage de Remora v1.3

<!--Liste de toutes les images utilisées-->
[Schéma implantation]: ../ProgrammateurFilPilote_1.3_Schematic.png
[PCB face avant]: ../ProgrammateurFilPilote_1.3_Recto.png
[PCB face arrière]: ../ProgrammateurFilPilote_1.3_Verso.png


## Remora

*Insérer ici une photo du module assemblé*

Remora, c'est un programmateur de chauffages par fils pilotes.

- Il peut contrôler 7 zones et un chauffe-eau (via 1 relais)
- Il se commande via des requêtes HTTP en WiFi
- Il est open source et open hardware


## Contenu du kit
*Insérer ici une photo du contenu du kit*

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

## Schéma d'implantation

![Schéma implantation]

*Nota : Il est bien utile d'avoir un exemplaire imprimé du [schéma d'implatation](https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/Mat%C3%A9riel/1.3/ProgrammateurFilPilote_1.3_Schematic.png) près de soi lorsque l'on fait les soudures*

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

Soudure de D1, D3, D5, D7, D9, D11 et D13 (**Attention à la polarité !**)

----------

Soudure de OP1, OP3, OP5, OP7, OP9, OP11 et OP13 (**Attention à la polarité !**)

----------

Soudure de R3, R5, R7, R9, R11, R13 et R15 (Pas de polarité pour les résistances)

----------

De la même manière, procédez aux soudures suivantes :

Soudure de R18

Soudure de R17

Soudure de D15 (**Attention à la polarité !**)

Soudure de R22

Soudure de T2

Soudure de R2

Soudure de R1

Soudure de R21

Soudure de de C1 et C2

Soudure de R19 et R20

Soudure de OK1 (**Attention à la polarité !**)

### Face arrière

Soudure de D2, D4, D6, D8, D10, D12, D14 (**Attention à la polarité !**)

Soudure de OP2, OP4, OP6, OP8, OP10, OP12, OP14  (**Attention à la polarité !**)

Soudure de R4, R6, R8, R10, R12, R14, R16

Soudure de T1 (**Attention à la polarité !**)

Soudure du support pour IC1

Soudure des supports pour le Spark Core/Photon/NodeMCU

Soudure du connecteur OLED

Soudure du connecteur Grove

Soudure du relais

Soudure des borniers

Soudure des LEDs


### Astuces de soudure CMS

Le plus compliqué est de maintenir le composant en place pendant que l'on soude.
Une astuce, le troisième bras et une pointe.  
*Insérer une photo ici*

## Câblage et tests

### Test des soudures

### Montage dans le tableau électrique

### Test de de bon fonctionnement sans le NodeMCU

## Programmation du NodeMCU

## Test du module complet

### Via l'API