#include <fstream>


#include "alg_sa.h"
#include "test.h"

using namespace std;

int main(int argc, char* argv[]){
	if(argc != 2){
		cout << "usage: TestScheduler <soc_name>" << endl;
		return 0;
	}
	System system;
	system.read_input(argv[1]);

	cout << "Cores: " << system.getNumCores() << endl;
	cout << "Tests: " << system.getNumTests() << endl;

	Alg_SA alg(system);
	//Alg_Greedy alg(system);

	ofstream fout("log.txt");
	fout << alg.run();
	fout.close();
	return 0;
}
