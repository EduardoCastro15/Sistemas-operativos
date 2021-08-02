#!/bin/bash

#autor: Eduardo Castro Cruces

array[0]=15
array[1]=20
array[2]=40

echo "El tamanio del arreglo es : ${#array[*]}"
echo "El contenido del arreglo es : ${array[*]}"
echo "El primer valor del arreglo es : ${array[0]}"

for i in `seq 0 2`
do
	echo "El valor $i del arreglo es : ${array[i]}"
done
