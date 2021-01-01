#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "instance.hpp"
#include "utils.hpp"

#define BASE10 (10)
#define MICROSECONDS_PER_MILLISECOND (1000.L)

int main(int argc, char **argv){
    if(argc < 3){
        fprintf(stderr, "Faltan argumentos.\n");
        fprintf(stderr, "Ejecución:\n");
        fprintf(stderr, "\t./MCPwB.elf <ruta/instancia.txt> <maxIteraciones>\n");
        fprintf(stderr, "Saliendo...\n");
        return 0;
    }
    auto time_program_start = std::chrono::steady_clock::now();
    Utils::debugPrintingEnabled = false;

    char *filename = argv[1];
    long K = strtol(argv[2], nullptr, BASE10);
    if(argc > 3){
        Utils::debugPrintingEnabled = std::string(argv[3]) == "true"; 
    }

    printf("\n");

    Instance inst(filename);
    if(Utils::debugPrintingEnabled) inst.print(true);

    auto sol = inst.initialSolution();
    if(Utils::debugPrintingEnabled){
        for(unsigned long i = 0; i < sol.size(); ++i){
            printf("%li: ", i);
            const auto &route = sol.at(i);
            route.print(true);
        }
        printf("Value: %Lf\n\n", inst.evaluateSolution(sol));
    }

    auto optimal(inst.hillClimbing(sol, K));

    if(Utils::debugPrintingEnabled){
        for(unsigned long i = 0; i < optimal.size(); ++i){
            printf("%li: ", i);
            const auto &route = optimal.at(i);
            route.print(true);
        }
        printf("Value: %Lf\n\n", inst.evaluateSolution(optimal));
    }

    const auto *initial_node = inst.getInitialNode();

    auto total_benefits = inst.evaluateSolution(optimal);
    auto transport_costs = inst.calculateTransportCosts(optimal);
    auto milk_profits = inst.calculateMilkProfits(optimal);
    printf("%.2Lf %.2Lf %.2Lf\n\n", total_benefits, transport_costs, milk_profits);

    for(const auto &route: optimal){
        printf("%02ld-", (*initial_node).getId());
        for(const auto &node: route.getNodes()){
            printf("%02ld-", (*node).getId());
        }
        printf("%02ld %.2Lf %ld %c\n", (*initial_node).getId(), route.calculateTransportCosts(initial_node), route.getMilkAmount(), route.getMilkType());
    }

    auto time_program_end = std::chrono::steady_clock::now();
    auto time_program_duration = (time_program_end - time_program_start)/std::chrono::microseconds(1);
    long double time_milis = time_program_duration/MICROSECONDS_PER_MILLISECOND;
    printf("\nTiempo total de ejecución: %02.3Lf[ms]\n", time_milis);

    printf("\n");

    return 0;
}
