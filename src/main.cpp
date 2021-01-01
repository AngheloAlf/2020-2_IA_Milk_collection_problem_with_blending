#include <cstdlib>
#include <cstdio>

#include "instance.hpp"
#include "utils.hpp"

int main(int argc, char **argv){
    if(argc < 3){
        fprintf(stderr, "Faltan argumentos.\n");
        fprintf(stderr, "Ejecución:\n");
        fprintf(stderr, "\t./MCPwB.elf <ruta/instancia.txt> <maxIteraciones>\n");
        fprintf(stderr, "Saliendo...\n");
        return 0;
    }

    char *filename = argv[1];
    long K = strtol(argv[2], nullptr, 10);

    Utils::debugPrintingEnabled = true;
    printf("\n");

    Instance inst(filename);
    //inst.print(true);

    auto sol = inst.initialSolution();
    for(unsigned long i = 0; i < sol.size(); ++i){
        printf("%li: ", i);
        Route route = sol.at(i);
        route.print(true);
    }

    printf("Value: %lf\n\n", inst.evaluateSolution(sol));

    auto newsol(inst.hillClimbing(sol, K));
    for(unsigned long i = 0; i < newsol.size(); ++i){
        printf("%li: ", i);
        Route route = newsol.at(i);
        route.print(true);
    }
    printf("Value: %lf\n\n", inst.evaluateSolution(newsol));

    return 0;
}
