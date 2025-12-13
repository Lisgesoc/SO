#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit 1
fi

DIR="$1"

if [ -d "$DIR" ];then
    rm -rf "$DIR"/*
else
    mkdir -p "$DIR"
fi

cd "$DIR" 

mkdir "subdir"
touch "fichero1"
echo 0123456789 > "fichero2"
ln -s "fichero1" "enlaceS"
ln "fichero2" "enlaceH"

stat .
ls 
stat "subdir"
stat "fichero1"
stat "fichero2"
stat "enlaceS"
stat "enlaceH"




