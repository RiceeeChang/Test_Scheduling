#include <iostream>


#include "alg_greedy.h"
#include "test.h"

using namespace std;

int main(int argc, char* argv[]){
	if(argc != 2){
		cout << "usage: TestScheduler <soc_name>" << endl;
		return 0;
	}
	System system;
	system.read_input(argv[1]);

	Alg_Greedy alg(system);

	cout << alg.run();

	return 0;
}
