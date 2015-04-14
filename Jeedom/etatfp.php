#!/usr/bin/php
<?php
// Theju / 15/04/2015 / création du script

// Script permettant de récuperer les infos des fils pilotes de la carte de Tibault
// https://github.com/thibdct/programmateur-fil-pilote-wifi
// Le script prend un argument, simplement le numéro du fil pilote.
// Il est fourni tel quel, attention, aucun verification n'est faite sur les variable ou sur l'argument passé.
 
// Utilisation dans Jeedom : 
// Créer un script puis une commande d'info, type script, sous type numerique. 
// Clisquer sur nouveau
// Copier / Coller ce script
// Modifier les variables $core et $token en fonction de votre spark core
// Enregistrer
// Le chemin du script apparait dans le champ requete, a jouter a la fin le numéro du fil pilote a cheker : 
// ex Fil pilote 1 : /chemin vers le script/etatFP.php 1

// Declaration des variables :
$core = "votre core id";
$token = "votre token";

// Plus rien a modifier en dessous de cette ligne.

// Let's Go !
//recuperation de l'argument du cFP
$fp = $argv[1];

//Attention tableu commence à 0 :
$fp = $fp-1;

//initialisation de la requete :
//URL of targeted site  
$url = "https://api.spark.io/v1/devices/$core/etatfp?access_token=$token";  
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

//Les infos que l'on cherche sont dans le champ result
$etatfp = $obj->{'result'};
$etatfp = substr($etatfp,$fp,1); 

echo $etatfp;
?>

