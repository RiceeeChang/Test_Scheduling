#ifndef ALG_SA_H
#define ALG_SA_H

#include "alg_greedy.h"
#include "solution.h"

class Alg_SA : public Alg_Greedy
{
public:
	Alg_SA(const System&);
	virtual ~Alg_SA();

	// run() is the same as Greedy approach
	// virtual Schedule run();
protected:
	virtual TAMSolution packCoreRectangle();
	virtual MidSolution packTestRectangle(const ConstraintTable&);
	virtual TTime costCoreRect(const TAMSolution&);
	virtual TTime costTestRect(const MidSolution&);

	virtual SequencePair perturb(const TAMSolution&);
	virtual SequencePair
	perturb(const MidSolution&, const ConstraintTable&);
};

#endif//ALG_SA_H
