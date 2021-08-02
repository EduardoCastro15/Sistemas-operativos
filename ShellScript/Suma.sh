#!/bin/bash

#autor: Eduardo Castro Cruces

function Suma
{
	a=`expr $1 + $2`
	echo "La suma de $1 y $2 es $a"
	if [ $1 -gt $2 ] 
	then
		echo "$1 es mayor que $2"
	else
		echo "$1 es menor que $2"
	fi
}

echo "El numero de argumentos es $#"
echo "Los argumentos son: $@"
echo "El argumento 1 es: $1"

Suma $1 $2

for i in `ls`
do
	echo "Archivo: $i"
done

for i in `seq 0 5`
do
	echo "Valor de i: $i"
done
