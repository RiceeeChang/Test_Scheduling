#include "test.h"


int main(int argc, char* argv[]){

        System sys;
        sys.read_input(argv[1]);
        sys.print();

	Schedule sch;
	sch.optimal_test_time = 700;
	int x = rand() % 100;
	int y = rand() % 150;
	int i, j;
	pair<int, int> tmp;

	for(i = 0; i < sys.core_name.size(); i++){
		tmp.first = x++;
		tmp.second = y++;
		sch.TAM_assignment.push_back(tmp);
	}
	
	for(i = 0; i < sys.test.size(); i++){
		
		vector<pair<int, int> > v;
		tmp.first = x++;
                tmp.second = y++;
		v.push_back(tmp);
		if(sys.test[i]->partition > 1){
			tmp.first = x++;
                	tmp.second = y++;
			v.push_back(tmp);
		}
		sch.timestamps.push_back(v);
	}
	write_output(argv[2], sys, sch);

        return 0;
}
