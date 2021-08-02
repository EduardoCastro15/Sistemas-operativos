#!/bin/bash

#autor: Castro Cruces Eduardo
#autor: Cortés Castaño Eduardo

Array=("$@")
Tam=${#Array[@]}
if [ -z $1 ]; then
	echo "Te falta especificar el parámetro"
else
	echo "El numero de argumentos introducidos es: $Tam"
fi