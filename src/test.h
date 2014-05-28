#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>

enum{
	External,
	BIST
};

using namespace std;

class Core{
    public:
        Core(const int& bits);
        bool occupied;
        vector<int> bus;
};

class Bist{
    public:
        Bist();
        bool occupied;
};

class Test{

	public:
		Test(const string& n);		

		string name;
		int category;
		int core;
		int TAM_width;
		int length;
		int power;
		int partition;
		string resource;

        // Rice Add
        int begin;
        int end;
        bool scheduled;
        bool complete;
};

class System{

	public:
		System();
		~System();

		string sys_name;
		int sys_TAM_width;
		int sys_power;
		vector< vector<string> > precedence;
		vector<string> sys_resource;
		vector<string> core_name;

		vector<Test *> test;

		void init();
		Test * getTestByName(const string& name);

		void read_input(const string& fname);

		//for debugging
		void print();
        // Rice Add
        vector<Core*> cores;
        map<string, Bist*> bists;
        vector<bool> tam_bus;
};

class Schedule{

	public:
		int optimal_test_time;	
		
                vector<pair<int, int> > TAM_assignment;
		vector<vector<pair<int, int> > > timestamps;

};

void write_output(const string& fname, System& sys, Schedule& sch);

#endif
