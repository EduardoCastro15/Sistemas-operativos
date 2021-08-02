#!/bin/bash

#autor: Castro Cruces Eduardo
#autor: Cortés Castaño Eduardo


if [ -e $1 ];
then
	echo "$1 si existe."
else
	echo "$1 no existe."
fi
if [ -f $1 ];
then
	echo "$1 es un archivo regular."
	if [ -r $1 ];
	then
		echo "$1 tiene permisos de lectura."
	else
		echo "$1 no tiene permisos de lectura."
	fi
	if [ -w $1 ];
	then
		echo "$1 tiene permisos de escritura."
	else
		echo "$1 no tiene permisos de escritura."
	fi
	if [ -x $1 ];
	then
		echo "$1 tiene permisos de ejecucion."
	else
		echo "$1 no tiene permisos de ejecucion."
	fi
else
	echo "$1 es un directorio."
	if [ -r $1 ];
	then
		echo "$1 tiene permisos de lectura."
	else
		echo "$1 no tiene permisos de lectura."
	fi
	if [ -w $1 ];
	then
		echo "$1 tiene permisos de escritura."
	else
		echo "$1 no tiene permisos de escritura."
	fi
	if [ -x $1 ];
	then
		echo "$1 tiene permisos de busqueda."
	else
		echo "$1 no tiene permisos de busqueda."
	fi
fi




