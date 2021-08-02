#!/bin/bash

#autor: Eduardo Castro Cruces

Saludo="Bienvenido usuario: "
us=$(whoami)
dia=$(date +%d-%m-%Y)

echo "Hola Mundo"
echo "$Saludo $us"
echo "La fecha de hoy es: $dia"
echo "La ruta actual es: "
pwd