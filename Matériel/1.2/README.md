Version matérielle 1.2c
=======================

Modifications
-------------
Cette version reprend toutes les caractéristiques de la version [initiale][1] en y ajoutant les fonctionnalités suivantes :

- Ajout d'un contrôleur d'entrées sortie I2C [MCP23017][10] pour libérer les I/O du Spark Core
- Possibilité de mettre deux types de relais dont un [avec contact NO + NF][5]
- Possibilité de mettre un emetteur récepteur sans fil de type [RFM12B][6], [RFM69CW][7], [RFM69HW][8] ou [RFM69CW][9] 433Mhz ou 868MHz
- Possibilité de mettre un [récepteur 433MHz][4] pour récupérer par exemple les trames des capteurs de température Orégon Scientific
- Ajout d'un connecteur I2C ET SPI pour des afficheurs OLED de type [I2C][2] ou [SPI][3]
- Ajout d'un connecteur de type Grove I2C
- Ajout d'une LED 3mm (déportable ou non) sur le boitier 
- Ajout de pin out disponibles sur toutes les pattes du Spark Core

Changements apportés dans la révision 1.2c 
- Correction informations du silk au niveau du connecteur grove
- Déplacement connecteur grove pour ne pas géner en cas d'insertion un module grove
- Déplacement de la LED pour qu'elle ne soit plus située sous le LCD
- Agrandissement des PAD des optos pour une soudure plus aisée
- Inversion des plots 3.3V et GND afin de pouvoir souder un double plot uniquement (5V+GND), les triples étant moins facile à trouver et je suis pas certain que le 3.3V soit utilisé (mais il reste disponible)

Attention, deux erreurs étaient présentes sur le PCB 1.2 original (corrigées dans la revision 1.2c) :
- le dessin sur le PCB du connecteur grove est inversé, donc si vous souhaitez le souder, il faut le faire dans le sens inversé par rapport au dessin du PCB (mais toujours à souder sur le dessus).
- la diode D14 est inversée, il faut donc la souder dans le sens inverse de ce qui est indiqué sur le PCB


Description Détaillée
=====================

**Schematic**  
![schematic](https://raw.github.com/thibdct/programmateur-fil-pilote-wifi/master/Mat%C3%A9riel/1.2/ProgrammateurFilPilote_1.2_Schematic.png)


**Boards**  
<img src="https://raw.github.com/thibdct/programmateur-fil-pilote-wifi/master/Mat%C3%A9riel/1.2/ProgrammateurFilPilote_1.2_Board.png" alt="board" width="30%" height="30%">&nbsp;
<img src="https://raw.github.com/thibdct/programmateur-fil-pilote-wifi/master/Mat%C3%A9riel/1.2/ProgrammateurFilPilote_1.2_Recto.png" alt="top" width="30%" height="30%">&nbsp;
<img src="https://raw.github.com/thibdct/programmateur-fil-pilote-wifi/master/Mat%C3%A9riel/1.2/ProgrammateurFilPilote_1.2_Verso.png" alt="bottom" width="30%" height="30%">


[1]: https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/README.md
[2]: http://www.ebay.com/itm/291216700457
[3]: http://www.ebay.com/itm/141371873602
[4]: http://www.ebay.com/itm/290935235157
[5]: http://www.seeedstudio.com/depot/index.php?main_page=opl_info&opl_id=136

[6]: http://www.anarduino.com/details.jsp?pid=142
[7]: http://www.anarduino.com/details.jsp?pid=145
[8]: http://www.anarduino.com/details.jsp?pid=136
[9]: http://www.anarduino.com/details.jsp?pid=122
[10]: http://www.adafruit.com/product/732

