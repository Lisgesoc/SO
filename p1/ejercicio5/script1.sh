#!/bin/bash

while read nombre;do

	echo "Hola, $nombre!" >> salu2.txt

done <nombres.txt

#!Fin script de prueba
