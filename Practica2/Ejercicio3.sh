#!/bin/bash

#autor: Eduardo Cortés Castaño
function ordenar {
    #Agregar números a un arreglo.
    array=("$@")
    numArg=`expr $# - 1`

    for i in `seq 0 $numArg`
    do
        for j in `seq 0 $numArg`
        do
            if [ ${array[$i]} -lt ${array[$j]} ]
            then
                temp=${array[$i]}
                array[$i]=${array[$j]}
                array[$j]=${temp}
            fi
        done
    done

    for i in `seq 0 $numArg`
    do
        echo "Valor $i es: ${array[$i]}"
    done
}


if [ $# -gt 1 ]
then
ordenar $*
else
echo "Inserte al menos dos argumentos."
fi
