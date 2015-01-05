#!/bin/bash
# Script permettant de commander le module programmateur fil pilote
# thibault.chauffage@gmail.com - v0.1 05/01/2015

DEVICE_ID="<A remplacer par l'identifiant de votre Spark Core>"
SPARK_TOKEN="<A remplacer par votre jeton Spark Core>"
FUNCTION="setfp"
PARAM=$1 # Paramètre d'entrée [ZONE = 1 à 7][ORDRE = A ou C ou E ou H], ex : 3C (zone 3 en mode confort)

curl https://api.spark.io/v1/devices/$DEVICE_ID/$FUNCTION -d access_token=$SPARK_TOKEN -d params=$PARAM
