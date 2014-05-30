#ifndef ALG_H
#define ALG_H

#include "schedule.h"

class System;

class Algorithm
{
public:
	Algorithm(const System&);
	virtual ~Algorithm() = 0;

	virtual Schedule run() = 0;

protected:
	const System& _system;
};

#endif//ALG_H

