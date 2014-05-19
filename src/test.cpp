#include "test.h"

Test::Test(const string& n){

	name = n;
}

System::System(){}

System::~System(){}

void System::init(){}

Test * System::getTestByName(const string& name){}

void System::read_input(const string& fname){

	ifstream fin(fname.c_str());
	char buf[200];
	char *delim = " \t";
	char *p;

	//parse system
	fin.getline(buf, 200);//System
	while(strcmp(buf, "System") != 0){
		fin.getline(buf, 200);
	}

	fin.getline(buf, 200);//begin
	fin.getline(buf, 200);//TAM
	while(strcmp(buf, "end") != 0){
		p = strtok(buf, delim);

		if(strcmp(p, "TAM_width") == 0){
			p = strtok(NULL, delim);
			sys_TAM_width = atoi(p);
		}
		else if(strcmp(p, "Power") == 0){
			p = strtok(NULL, delim);
			sys_power = atoi(p);
		}
		else if(strcmp(p, "Precedence") == 0){
			vector<string> v;
			int l = 0;
			while(p = strtok(NULL, delim)){
				if(l++ % 2 == 0){
					string str = p;
					v.push_back(str);
				}
			}
			precedence.push_back(v);

		}
		else if(strcmp(p, "Resource") == 0){
			while(p = strtok(NULL, delim)){
				string str = p;
				sys_resource.push_back(str);
			}
		}

		fin.getline(buf, 200);
	}
	fin.getline(buf, 200);

	//parse core
	int c = 0;
	string tmp;

	while(fin.getline(buf, 200)){ //Core

		int tam;
		tmp = buf;
		if(tmp.find("Core") == -1){
			continue;
		}
		c++;
		fin.getline(buf, 200);//begin
		fin.getline(buf, 200);//TAM
		while(strcmp(buf, "end") != 0){
			p = strtok(buf, delim);

			if(strcmp(p, "TAM_width") == 0){
				p = strtok(NULL, delim);
				tam = atoi(p);
			}
			else if(strcmp(p, "External") == 0){ //External
				p = strtok(NULL, delim);
				tmp = p;
				Test *t = new Test(tmp);
				t->category = External;
				t->core = c;
				t->TAM_width = tam;
				t->partition = 1;
				while(p = strtok(NULL, delim)){
					if(strcmp(p, "length") == 0){
						p = strtok(NULL, delim);
						t->length = atoi(p);
					}
					else if(strcmp(p, "power") == 0){
						p = strtok(NULL, delim);
						t->power = atoi(p);
					}
					else if(strcmp(p, "partition") == 0){
						p = strtok(NULL, delim);
						t->partition = atoi(p);
					}
				}
				test.push_back(t);
			}
			else if(strcmp(p, "BIST") == 0){ //BIST
				p = strtok(NULL, delim);
				tmp = p;
				Test *t = new Test(tmp);
				t->category = BIST;
				t->core = c;
				t->TAM_width = 0;
				while(p = strtok(NULL, delim)){
					if(strcmp(p, "length") == 0){
						p = strtok(NULL, delim);
						t->length = atoi(p);
					}
					else if(strcmp(p, "power") == 0){
						p = strtok(NULL, delim);
						t->power = atoi(p);
					}
					else if(strcmp(p, "resource") == 0){
						p = strtok(NULL, delim);
						t->resource = p;
					}
				}
				test.push_back(t);
			}
			fin.getline(buf, 200);
		}
	}
	fin.close();
}

void System::print(){

	int i;
	cout << "System" << endl;
	cout << "TAM_width = " << sys_TAM_width << endl;
	cout << "Power = " << sys_power << endl;
	cout << "Resource = ";
	for(i = 0; i < sys_resource.size(); i++)
		cout << sys_resource[i] << " ";
	cout << endl << endl;

	for(i = 0; i < test.size(); i++){
		cout << test[i]->name << endl;
		cout << "category " << test[i]->category 
			<< " core " << test[i]->core
			<< " TAM " << test[i]->TAM_width
			<< " length " << test[i]->length
			<< " power " << test[i]->power
			<< endl << endl;
	}

	cout << "Total number of tests = " << test.size() << endl;
}

