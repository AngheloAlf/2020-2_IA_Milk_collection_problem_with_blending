#!/bin/bash

## Ejecuta el programa con todas las instancias de prueba.
## Requiere un parámetro. La cantidad de iteraciones que se le pasará al programa.

bold=`tput bold`
green=`tput setaf 2`
reset=`tput sgr0`

# https://stackoverflow.com/a/20796617/6292472
for filename in instances/*.txt; do
    echo
    echo "${bold}${green}$filename${reset}"
    ./MCPwB.elf "$filename" "$1"
done

tput bel
