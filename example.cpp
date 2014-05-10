#include "test.h"

int main(int argc, char* argv[]){

        System *sys = new System();
        sys->read_input(argv[1]);
        sys->print();

        return 0;
}
