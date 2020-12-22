#include <stdlib.h>
#include <stdio.h>

#include "instance.hpp"

int main(int argc, char **argv){
    if(argc < 3){
        fprintf(stderr, "Faltan argumentos.\n");
        fprintf(stderr, "Ejecución:\n");
        fprintf(stderr, "./MCPwB.elf <ruta/instancia.txt> <maxIteraciones>\n");
        fprintf(stderr, "Saliendo...\n");
        return 0;
    }

    char *filename = argv[1];
    long K = strtol(argv[2], nullptr, 10);

    Instance inst(filename);
    
    return 0;
}
