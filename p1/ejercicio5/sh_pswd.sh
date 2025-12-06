#!/bin/bash

while IFS=':' read -r var1;
do

	IFS=':' read -a campos <<< $var1;
	
	home="${campos[5]}"
	dir_padre=$(dirname "$home")

	if [$dir_pdre = "/home"]; then

		for token in "${campos[@]}"; do
		echo -n '|'$token
		done
		echo '| '
	fi

done </etc/passwd

