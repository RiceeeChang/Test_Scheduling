#include <cmath>
#include <cstdlib>
#include <ctime>

#include <iostream>

#include "test.h"
#include "alg_sa.h"

using namespace std;

namespace
{
size_t myRand()
{	return rand();}

bool checkSwapPositive(
	const vector<MidSolution::PartedTest>& rects,
	const SequencePair& seq,
	const ConstraintTable& cons_table,
	const size_t& r1, const size_t& r2
)
{
	const size_t& tid_r1 = rects[r1].getTestId();
	const size_t& tid_r2 = rects[r2].getTestId();

	if(seq.left(r1, r2) || seq.left(r2, r1))
		if(cons_table(tid_r1, tid_r2) != ConstraintTable::NONE)
			return false;
	// else Can swap
	return true;
}
bool checkSwapBoth(
	const vector<MidSolution::PartedTest>& rects,
	const SequencePair& seq,
	const ConstraintTable& cons_table,
	const size_t& r1, const size_t& r2
)
{
	const size_t& neg_loc1 = seq.findNegative(r1);
	const size_t& neg_loc2 = seq.findNegative(r2);
	const size_t& tid_r1 = rects[r1].getTestId();
	const size_t& tid_r2 = rects[r2].getTestId();

	const vector<size_t> neg_loci = seq.getNegativeLoci();
	if(seq.left(r1, r2))
	{
		if(cons_table(tid_r1, tid_r2) == ConstraintTable::FW_PREC)
			return false;
		for(size_t r_neg=neg_loc1+1; r_neg<neg_loc2; ++r_neg)
		{
			const size_t& r_id = neg_loci[r_neg];
			const size_t& r_tid = rects[r_id].getTestId();
			if( seq.left(r1, r_id) )
				if(cons_table(tid_r1, r_tid)!=ConstraintTable::NONE)
					return false;
			if( seq.left(r_id, r2) )
				if(cons_table(r_tid, tid_r2)!=ConstraintTable::NONE)
					return false;
		}
	}
	else
	{	assert(seq.below(r2, r1));
		for(size_t r_neg=neg_loc2+1; r_neg<neg_loc1; ++r_neg)
		{
			const size_t& r_id = neg_loci[r_neg];
			const size_t& r_tid = rects[r_id].getTestId();
			if( seq.left(r_id, r1) )
				if(cons_table(tid_r1, r_tid)!=ConstraintTable::NONE)
					return false;
			if( seq.left(r2, r_id) )
				if(cons_table(r_tid, tid_r2)!=ConstraintTable::NONE)
					return false;
		}
	}
	// else
	return true;
}

}

Alg_SA::Alg_SA(const System& sys):
Alg_Greedy(sys){}

Alg_SA::~Alg_SA(){}

TAMSolution Alg_SA::packCoreRectangle()
{	// TODO Reduce repeat code
	srand(time(NULL));

	TAMSolution tam_sol = Alg_Greedy::packCoreRectangle();

	SequencePair opt_sp = tam_sol.getSequencePair(); 

	double tpr=4000.00, thresh = 1.0;
	const double gamma = 0.98;
	size_t iter_max = 400 * opt_sp.size();

	TTime cur_cost = costCoreRect(tam_sol);
	TTime opt_cost = cur_cost;

	while(tpr>thresh)// Stopping critiria
	{
		for(size_t i=0; i<iter_max; ++i)
		{
			SequencePair nb = perturb(tam_sol);

			// Accept it first
			tam_sol.swapSequencePair(nb);

			TTime nb_cost = costCoreRect(tam_sol);
			if(opt_cost > nb_cost)
			{
				opt_sp = tam_sol.getSequencePair();
				opt_cost = nb_cost;
			}
			else if(cur_cost < nb_cost)
			{	// Reject by chance
				double q = ((double) myRand() / (RAND_MAX));
				double expnt = -((double)(nb_cost-cur_cost)) / tpr;
				if(q >= exp(expnt))
				{
					tam_sol.swapSequencePair(nb);// Swap back
				}
			}
		}

		tpr *= gamma;
	}

	// Return the optimal
	tam_sol.swapSequencePair(opt_sp);
	return tam_sol;
}

MidSolution Alg_SA::packTestRectangle(const ConstraintTable&cons_table)
{	// TODO Reduce repeat code
	srand(time(NULL));

	MidSolution sol = Alg_Greedy::packTestRectangle(cons_table);

	SequencePair opt_sp = sol.getSequencePair(); 

	double tpr=4000.00, thresh = 1.0;
	const double gamma = 0.90;
	size_t iter_max = 200*opt_sp.size();

	TTime cur_cost = costTestRect(sol);
	TTime opt_cost = cur_cost;

	while(tpr>thresh)// Stopping critiria
	{
		for(size_t i=0; i<iter_max; ++i)
		{
			SequencePair nb = perturb(sol, cons_table);

			// Accept it first
			sol.swapSequencePair(nb);

			TTime nb_cost = costTestRect(sol);
			if(opt_cost > nb_cost)
			{
				opt_sp = sol.getSequencePair();
				opt_cost = nb_cost;
			}
			else if(cur_cost < nb_cost)
			{	// Reject by chance
				double q = ((double) myRand() / (RAND_MAX));
				double expnt = -((double)(nb_cost-cur_cost)) / tpr;
				if(q >= exp(expnt))
				{
					sol.swapSequencePair(nb);// Swap back
				}
			}
		}

		tpr *= gamma;
	}

	// Return the optimal
	sol.swapSequencePair(opt_sp);
	return sol;
}

TTime Alg_SA::costCoreRect(const TAMSolution& tam_sol)
{	return tam_sol.getRequiredTime();}

TTime Alg_SA::costTestRect(const MidSolution& sol)
{	return sol.getRequiredTime();}

SequencePair Alg_SA::perturb(const TAMSolution& tam_sol)
{	// TODO Reduce repeat code
	SequencePair ret = tam_sol.getSequencePair();

	// Randomly pick an element
	size_t a = myRand() % ret.size();

	for(size_t i=0; i<ret.size(); ++i)
	{
		size_t e1 = (a+i) % ret.size();
		size_t e2 = (a+i+1) % ret.size();

		size_t choice = myRand() % 2;
		size_t c = choice;
		do
		{
			if(c == 0)
			{	// Swap in postive loci only
				ret.swapPositive(e1, e2);
				if(tam_sol.checkTAMWidth(_system.getTAMWidth(), ret))
					return ret; // A valid move
				ret.swapPositive(e1, e2); // Back to original
			}
			else
			{	// Swap both loci
				ret.swapBoth(e1, e2);
				if(tam_sol.checkTAMWidth(_system.getTAMWidth(), ret))
					return ret; // A valid move
				ret.swapBoth(e1, e2); // Back to original
			}
			c = (c+1) % 2;
		}
		while(c != choice);
		// Both move is rejected
	}

	return ret;
}

SequencePair
Alg_SA::perturb(const MidSolution& sol, const ConstraintTable& cons_table)
{	// TODO Reduce repeat code
	const vector<MidSolution::PartedTest>& rects = sol.getPartedTestRects();
	SequencePair ret = sol.getSequencePair();

	// Randomly pick an element and swap with its next
	size_t a = myRand() % ret.size();

	for(size_t i=0; i<ret.size(); ++i)
	{
		size_t loc = (a+i) % ret.size();
		if(loc + 1 == ret.size()) continue; // Skip boundry case

		size_t choice = (myRand() % 2);
		for(size_t j=0; j<2; ++j)
		{
			size_t c = (choice+j) % 2;
			if(c == 0)
			{	// Swap in postive loci only
				const vector<size_t>& pos_loci = ret.getPositiveLoci();
				const size_t& r1 = pos_loci[ loc ];
				const size_t& r2 = pos_loci[ loc+1 ];
				if(!checkSwapPositive(rects, ret, cons_table, r1, r2))
					continue;
				ret.swapPositive(r1, r2);
				if(sol.checkPower(_system.getMaxPower(), ret))
					return ret; // A valid move
				ret.swapPositive(r1, r2); // swap back
			}
			else if (c == 1)
			{	// Swap both
				const vector<size_t>& pos_loci = ret.getPositiveLoci();
				const size_t& r1 = pos_loci[ loc ];
				const size_t& r2 = pos_loci[ loc+1 ];
				if(!checkSwapBoth(rects, ret, cons_table, r1, r2))
					continue;
				ret.swapBoth(r1, r2);
				if(sol.checkPower(_system.getMaxPower(), ret))
					return ret; // A valid move
				ret.swapBoth(r1, r2); // swap back
			}
		}
		// All kinds of move are rejected
	}

	// All elements are rejected which should be impossible
	return ret;
}

