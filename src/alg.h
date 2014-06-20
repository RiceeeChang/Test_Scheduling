#ifndef ALG_H
#define ALG_H

#include "type_defs.h"
#include "schedule.h"

class System;

class Algorithm
{
public:
	Algorithm(const System&);
	virtual ~Algorithm() = 0;

	virtual Schedule run() = 0;
	
	const TTime& getLowerBound() const {return _lower;}
	const TTime& getUpperBound() const {return _upper;}
private:
	TTime computeLowerBound() const;
	TTime computeUpperBound() const;

protected:
	const System& _system;
private:
	TTime _lower, _upper;
};

#endif//ALG_H

