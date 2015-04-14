#!/usr/bin/php
<?php

// script de recuperation des info compteurs EDF depuis la carte de Thibault (https://github.com/thibdct).
// Le script est fourni tel quel, il n'est pas parfait et aucun contrôle n'est fait sur les données.
//
// Il est necessaire de modifer le code de thibdct et hallard dans le spark afin de recuperer toutes les valeurs dans une seul réponse json
// voir mon github : https://github.com/Th3ju/programmateur-fil-pilote-wifi
//
// Integration à jeedom : 
// Créer un nouveau script puis ajouter une commande d'info evenement
// Selectionner script, type Info / Autre.
// Cliquer sur nouveau
// Copier / Coller ce script puis enregistrer
// Modifier la valeur Programmation à */5 * * * * (execution auto du script toutes les 5 minutes)
// Vous n'avez plus qu'a verifier que les imports sont corrects dans les logs du plugin teleinfo
// Attention, la plage horaire ne fonctionne pas. Si quelqu'un trouve pourquoi faites moi signe.


// Il faut modifier les varibales suivantes suivant votre conf :
// L'ip de votre jeedom : 
$IP = "IP:80";
// votre clé API jeedom :
$API= "xxxxxxxxxxxxxxxxxxx";

// votre spark id : 
$core = "xxxxxxxxxxxxxxxxxxxxxxxx";
// votre token d'accès a votre spark core :
$token = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

// Plus rien à toucher en dessous de cette ligne.
// Let's go !!! 

//initialisation de la requete :
//On forge l'URL de notre sprk core :  
$url = "https://api.spark.io/v1/devices/$core/tinfo?access_token=$token";  
$ch = curl_init();  

// set URL and other appropriate options  
curl_setopt($ch, CURLOPT_URL, $url);  
curl_setopt($ch, CURLOPT_HEADER, 0);  
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);  

// grab URL and pass it to the browser  
$output = curl_exec($ch);  

// close curl resource, and free up system resources  
curl_close($ch);  

//On décode la trame JSON 
$obj = json_decode($output);

//Les infos que l'on cherche sont dans le champ recherche, on décode a nouveau
// TODO : voir comment rendre le json plus propre et ne pas avoir besoin de décoder en 2 passes 
$json = $obj->{'result'};

//On parse les infos utiles :
$tinfo = json_decode($json);

//recuperation de toutes les trames depuis la crte fils pilotes :
$isousc = $tinfo->{'isousc'};
$iinst  = $tinfo->{'iinst'};
$papp   = $tinfo->{'papp'};
$ptec   = $tinfo->{'ptec'};
$hchp   = $tinfo->{'indexHP'};
$hchc   = $tinfo->{'indexHC'};
$adco   = $tinfo->{'ADCO'};

# recuperation de la plage horaire et transformation : 
switch($ptec){
  case 0:
   $ptec = "HC";
   break;
  case 1:
   $ptec = "HP";
   break;
}

// Fabrication de l'url : 
$url ="http://$IP/jeedom/plugins/teleinfo/core/php/jeeTeleinfo.php?api=$API&ADCO=$adco&HCHP=$hchp&HCHC=$hchc&PAPP=$papp&IINST=$iinst&ISOUSC=$isousc&PTEC=$ptec"; 

$publish = file_get_contents($url);
?>

