#ifndef ALG_GREEDY_H
#define ALG_GREEDY_H

#include <cassert>

#include "alg.h"
#include "sequence_pair.h"

class ConstraintTable;
class TAMSolution;
class MidSolution;

class Alg_Greedy : public Algorithm
{
public:
	Alg_Greedy(const System&);
	virtual ~Alg_Greedy();

	virtual Schedule run();

protected:
	virtual TAMSolution packCoreRectangle();
	virtual MidSolution packTestRectangle(const ConstraintTable&);
	virtual std::vector<CoreBus> buildTAMAssignment(const TAMSolution&);
	virtual std::vector<TestSchedule>
		buildSchedule(const MidSolution&);
	virtual ConstraintTable findPairWiseConstraints(const std::vector<CoreBus>&);

protected:
	TTime _opt_time;
};

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

class TAMSolution
{
public:
	class CoreRect
	{
	public:
		static bool comp(const CoreRect& r1, const CoreRect& r2)
		{
			if(r1._height != r2._height)
				return r1._height > r2._height;
			if(r1._width != r2._width)
				return r1._width > r2._width;
			assert(r1._idx != r2._idx);
			return r1._idx < r2._idx;
		}
	public:
		CoreRect(const TCoreIndex& i,const TTime& h, const TWidth& w):
		_idx(i), _height(h), _width(w){}
		~CoreRect(){}

		const TCoreIndex& coreIndex() const {return _idx;}
		const TTime&  h() const {return _height;}
		const TWidth& w() const {return _width;}
	private:
		TCoreIndex _idx;
		TTime  _height;
		TWidth _width;
	};
public:
	TAMSolution(
		const std::vector<CoreRect>& r,
		const SequencePair& sp
	): _rects(r), _seq_pair(sp){}
	~TAMSolution(){}

	const std::vector<CoreRect>& getCoreRects() const
	{	return _rects;}
	const SequencePair& getSequencePair() const
	{	return _seq_pair;}
	void swapSequencePair(SequencePair& seq)
	{	SequencePair::swap(_seq_pair, seq);}
private:
	std::vector<CoreRect> _rects;
	SequencePair _seq_pair;
};

class MidSolution
{
public:
	typedef struct PartedTest
	{
		PartedTest(const size_t& tid, const TTime& b, const TTime& l)
		: test_id(tid), x(b), length(l){}
		size_t test_id;
		mutable TTime  x;
		TTime  length;
	}PartedTest;
public:
	MidSolution(
		const std::vector<PartedTest>& r,
		const SequencePair& sp
	): _rects(r), _seq_pair(sp){}
	~MidSolution(){}

	size_t size() const {return _rects.size();}
	const PartedTest& operator [] (const size_t& i) const
	{	return _rects[i];}
	const SequencePair& getSequencePair() const
	{	return _seq_pair;}

	std::vector<PartedTest> _rects;
	SequencePair _seq_pair; 
};

#endif//ALG_GREEDY_H

