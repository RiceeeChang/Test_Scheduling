#ifndef TEST_H
#define TEST_H

#include <vector>
#include <map>
#include <string>

#include "type_defs.h"

enum{
	External,
	BIST
};

class Core{
    public:
        Core(const int& bits);
        bool occupied;
        std::vector<int> bus;
};

class Bist{
    public:
        Bist();
        bool occupied;
};

class Test{

	public:
		Test(const std::string& n);

		std::string name;
		int category;
		int core;
		int TAM_width;
		int length;
		int power;
		int partition;
		std::string resource;

	// Rice Add
	int begin;
	int end;
	bool scheduled;
	bool complete;
};

class System{
public:
	typedef std::vector<size_t> Precedence;

	public:
		System();
		~System();

	size_t getNumTests() const {return test.size();}
	const Test& getTest(const size_t& i) const
	{	return *test[i];}

	size_t getNumCores() const {return core_name.size();}
	const std::string& getCoreName(const size_t& i) const
	{	return core_name[i];}
	TWidth getCoreWidth(const TCoreIndex&) const;
	TTime  getCoreExtTotalLength(const TCoreIndex&) const;

	size_t getNumPrecedences() const
	{	return _vec_prec.size();}
	const Precedence& getPrecedence(const size_t& i) const
	{	return _vec_prec[i];}

	public:
		std::string sys_name;
		int sys_TAM_width;
		int sys_power;
		std::vector< std::vector<std::string> > precedence;
		std::vector<std::string> sys_resource;
		std::vector<std::string> core_name;

		std::vector<Test *> test;

		void init();
		Test * getTestByName(const std::string& name);

		void read_input(const std::string& fname);

		//for debugging
		void print();
        // Rice Add
        std::vector<Core*> cores;
        std::map<std::string, Bist*> bists;
        std::vector<bool> tam_bus;

	private:
		std::vector<Precedence> _vec_prec;

		void storePrecedence
		(const std::vector< std::vector<std::string> >&);
};

#endif
