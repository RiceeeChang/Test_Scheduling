#ifndef ALG_GREEDY_H
#define ALG_GREEDY_H

#include <cassert>

#include "alg.h"

class Alg_Greedy : public Algorithm
{
protected:
	class ConstraintTable
	{
	public:
		enum ConstraintType
		{NONE=0, FW_PREC=1, SAME_RSRC=2, BW_PREC=3};
	public:
		ConstraintTable(const size_t& n):
		_num_elems(n)
		{	_cons.resize(n*(n-1)/2, NONE);}
		~ConstraintTable(){}

		void setPrecedence(const size_t& a, const size_t& b)
		{
			assert(a != b);
			if(a<b) _cons[getID(a, b)] = FW_PREC;
			else _cons[getID(a, b)] = BW_PREC;
		}	
		void setSameResource(const size_t& a, const size_t& b)
		{	_cons[getID(a, b)] = SAME_RSRC;}
		enum ConstraintType
		operator () (const size_t& a, const size_t& b) const
		{
			enum ConstraintType ret = _cons[getID(a, b)];
			switch(ret)
			{
				case NONE:
				case SAME_RSRC:
					break;
				case FW_PREC:
					ret = (a<b)? FW_PREC:BW_PREC; break;
				case BW_PREC:
					ret = (a<b)? BW_PREC:FW_PREC; break;
				default: assert(false);
			}
			return ret;
		}
		std::vector<size_t> getPreceedIds(const size_t& a) const
		{
			std::vector<size_t> ret;
			for(size_t i=0; i<a; ++i)
				if( _cons[getID(i,a)] == FW_PREC )
					ret.push_back(i);
			for(size_t i=a+1; i<_num_elems; ++i)
				if( _cons[getID(i,a)] == BW_PREC )
					ret.push_back(i);
			return ret;
		}
		std::vector<size_t> getConflictIds(const size_t& a) const
		{
			std::vector<size_t> ret;
			for(size_t i=0; i<a; ++i)
				if( _cons[getID(i,a)] != NONE )
					ret.push_back(i);
			for(size_t i=a+1; i<_num_elems; ++i)
				if( _cons[getID(i,a)] != NONE )
					ret.push_back(i);
			return ret;
		}
	private:
		const size_t _num_elems;
		std::vector<ConstraintType> _cons;

		static size_t getID(const size_t& i, const size_t& j)
		{
			assert(i != j);
			size_t c_min = (i<j)? i: j; 
			size_t c_max = (i>j)? i: j; 
			return c_max*(c_max-1)/2 + c_min;
		}
		
	};
public:
	Alg_Greedy(const System&);
	virtual ~Alg_Greedy();

	virtual Schedule run();

protected:
	virtual std::vector<CoreBus> computeBusAssignment();
	virtual std::vector<TestSchedule>
		computeSchedule(const ConstraintTable&);
	virtual ConstraintTable findPairWiseConstraints(const std::vector<CoreBus>&);

protected:
	TTime _opt_time;
};

#endif//ALG_GREEDY_H

