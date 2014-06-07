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

class Test{
	public:
		Test(const std::string& n);

		const std::string name;
		int category;
		int core;
		TWidth TAM_width;
		TTime  length;
		TPower power;
		TPartition partition;
		std::string resource;
};

class System{
public:
	typedef std::vector<size_t> Precedence;

public:
	System();
	~System();

	void read_input(const std::string& fname);

	const std::string& getName() const
	{	return sys_name;}
	const TWidth& getTAMWidth() const
	{	return sys_TAM_width;}
	const TPower getMaxPower() const
	{	return sys_power;}

	size_t getNumTests() const
	{	return test.size();}
	const Test& getTest(const size_t& i) const
	{	return *test[i];}

	size_t getNumCores() const
	{	return core_name.size();}
	const std::string& getCoreName(const size_t& i) const
	{	return core_name[i];}
	TWidth getCoreWidth(const TCoreIndex&) const;
	TTime  getCoreExtTotalLength(const TCoreIndex&) const;

	size_t getNumPrecedences() const
	{	return _vec_prec.size();}
	const Precedence& getPrecedence(const size_t& i) const
	{	return _vec_prec[i];}

	//for debugging
	void print();

private:
	std::string sys_name;
	TWidth sys_TAM_width;
	TPower sys_power;
	std::vector<std::string> sys_resource;
	std::vector<std::string> core_name;
	std::vector<Test *> test;
	std::vector<Precedence> _vec_prec;

private:
	void storePrecedence
	(const std::vector< std::vector<std::string> >&);
};

#endif
