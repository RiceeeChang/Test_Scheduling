#include "schedule.h"
#include "test.h"

using namespace std;

void write_output(const string& fname, const System& sys, const Schedule& sch){

	ofstream fout(fname.c_str());

	fout << "Schedule\nbegin\n\n";
	fout << "\tTest_time " << sch.optimal_test_time << endl << endl;
	
	//TAM_assignment
	for(size_t i = 0; i < sys.core_name.size(); i++){
		fout << "\tTAM_assignment " << sys.core_name[i]
			<< " [" << sch.TAM_assignment[i].first
			<< ":" << sch.TAM_assignment[i].second << "]\n";
	}
	fout << endl;
	
	//test
	int c = 0; 
	for(size_t i = 0; i < sys.test.size(); i++){
		if(sys.test[i]->core != c){
			fout << endl;
			c++;
		}
		
		if(sys.test[i]->category == BIST){
			fout << "\tBIST ";
		}else{
			fout << "\tExternal ";
		}
		fout << sys.core_name[sys.test[i]->core] << " "
			<< sys.test[i]->name << " ";
		//partition
		for(size_t j = 0; j < sch.timestamps[i].size(); j++){
			fout << "(" << sch.timestamps[i][j].first << ", "
			<< sch.timestamps[i][j].second << ")";
		}
		fout << endl;
	}
	fout << endl << "end" << endl;

	fout.close();
}
