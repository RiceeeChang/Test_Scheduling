#include <fstream>


#include "alg_sa.h"
#include "test.h"

using namespace std;

string getOutputName(const string& fname)
{
	const string old_suf = ".info";
	const string new_suf = ".schedule";

	size_t pos = fname.size() - old_suf.size();
	if(fname.compare(pos, string::npos, old_suf) == 0)
		return fname.substr(0, pos) + new_suf;
	// else
	return fname + new_suf;
}

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

	ofstream fout(getOutputName(argv[1]).c_str());
	fout << alg.run();
	fout.close();
	return 0;
}
