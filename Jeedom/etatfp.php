#!/usr/bin/php
<?php
// =========================================================================
//  Theju / 15/04/2015 / création du script
// =========================================================================

// Script permettant de récuperer les infos des fils pilotes de la carte de Tibault
// https://github.com/thibdct/programmateur-fil-pilote-wifi
// Le script prend un argument, simplement le numéro du fil pilote.
// Il est fourni tel quel, attention, aucun verification n'est faite sur les variable ou sur l'argument passé.
 
// =========================================================================
// Utilisation dans Jeedom : 
// =========================================================================

// Creer 2 variables (General / Scenarios / Voir Variables / Ajouter)
// SPARK_ID : L'id de votre spark
// SPARK_TOKEN : Votre token 
//
// Créer un script puis une commande d'info, type script, sous type numerique. 
// Cliquer sur nouveau
// Copier / Coller ce script
// Enregistrer
// Le chemin du script apparait dans le champ requete, a jouter a la fin le numéro du fil pilote a cheker : 
// ex Fil pilote 1 : /chemin vers le script/etatFP.php 1
// =========================================================================

// Plus rien a modifier en dessous de cette ligne.

require_once dirname(__FILE__) . '/../../../../core/php/core.inc.php';  

// Declaration des variables :
$core = scenario::getData("SPARK_ID");
$token = scenario::getData("SPARK_TOKEN");

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

// Compatibilité native Jeedom avec retour numerique :
switch ($etatfp) {
    case 'C':
     echo "0";
     break;
    case 'H':
     echo "1";
     break;
    case 'A':
     echo "2";
     break;
    case 'E':
     echo "3";
     break;
}
//echo $etatfp;
?>

