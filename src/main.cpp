#include <cstdio>
#include <cstdlib>
#include "test.h"
#include "lowerbound.h"
#include "greedy.h"
int main(int argc, char* argv[]){
    if(argc != 2){
        printf("usage: TestScheduler <soc_name>");
        return 0;
    }
    System system;
    system.init();
    system.read_input(argv[1]);
    
    printf("well read!!! \n");
    printf("System: TAM = %d, Power = %d\n", system.sys_TAM_width, system.sys_power);
    printf("cores = %d, bists = %d\n", system.cores.size(), system.bists.size());
    if(!system.cores[1]->occupied)
        printf("Not Occupied\n");
    //long int lb = lowerbound(system.test, system.sys_TAM_width, system.sys_power);
    //printf("Lowerbound2 = %ld\n", lb);
    
    int length = greedy(system);
    printf("Length = %d\n", length);

    return 0;
}
