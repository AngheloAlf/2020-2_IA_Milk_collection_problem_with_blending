#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "instance.hpp"
#include "utils.hpp"

#define BASE10 (10)
#define MICROSECONDS_PER_MILLISECOND (1000.L)


void printResults(const Instance &inst, Solution &optimal){
    const auto *initial_node = inst.getInitialNode();

    auto total_benefits = optimal.evaluateSolution();
    auto transport_costs = optimal.calculateTransportCosts();
    auto milk_profits = optimal.calculateMilkProfits();
    if(!optimal.isFeasible()) Utils::fgRed();
    printf("%.2Lf", total_benefits);
    Utils::resetColors();
    printf(" %.2Lf %.2Lf\n\n", transport_costs, milk_profits);

    int size_longest_route = 0;
    for(const auto &route: optimal.getRoutes()){
        int route_size = static_cast<int>(route.getNodes().size());
        if(route_size > size_longest_route){
            size_longest_route = route_size;
        }
    }

    for(const auto &route: optimal.getRoutes()){
        printf("%02ld-", (*initial_node).getId());
        for(const auto &node: route.getNodes()){
            Utils::fgColor(2+((*node).getQuality()-'A'));
            printf("%02ld", (*node).getId());
            Utils::resetColors();
            printf("-");
        }
        int route_size = static_cast<int>(route.getNodes().size());
        printf("%02ld", (*initial_node).getId());
        // Se encarga de poner el espacio faltante para que todo se vea cuadradito.
        printf("%*c", (size_longest_route-route_size)*3+1, ' ');

        printf("%7.2Lf ", route.calculateTransportCosts(initial_node));
        if(!route.doesFulfilQuota()) Utils::fgRed();
        printf("%5ld", route.getMilkAmount());
        Utils::resetColors();
        printf(" ");
        if(!route.isFeasible()) Utils::fgRed();
        printf("%c", route.getMilkType());
        Utils::resetColors();
        printf("\n");
    }
}


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
        printf("Initial solution:\n");
        sol.print(true);
        printf("\n");
    }

    auto optimal(inst.hillClimbing(sol, K));

    if(Utils::debugPrintingEnabled){
        printf("Final solution:\n");
        sol.print(true);
        printf("\n");
    }

    printResults(inst, optimal);

    auto time_program_end = std::chrono::steady_clock::now();
    auto time_program_duration = (time_program_end - time_program_start)/std::chrono::microseconds(1);
    long double time_milis = time_program_duration/MICROSECONDS_PER_MILLISECOND;
    printf("\nTiempo total de ejecución: %02.3Lf[ms]\n", time_milis);

    printf("\n");

    return 0;
}
