#include <algorithm>
#include <set>

#include <iostream> //TODO

#include "alg_greedy.h"
#include "test.h"
#include "schedule.h"

using namespace std;

namespace
{
class CoreRect
{
public:
	static bool comp(const CoreRect& r1, const CoreRect& r2)
	{
		if(r1._height != r2._height)
			return r1._height > r2._height;
		if(r1._width != r2._width)
			return r1._width > r2._width;
		return false;
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
class CoreRectBin
{
friend class Comp;

public:
	class Comp
	{
	public:
		bool operator () (const CoreRectBin& b1, const CoreRectBin& b2)
		{
			if(b1._w_avail != b2._w_avail)
				return b1._w_avail < b2._w_avail;
			assert(b1._curr_h != b2._curr_h);
			return b1._curr_h > b2._curr_h;
		}
	};

public:
	CoreRectBin(const TTime& curr_h, const TWidth& w_avail):
	_curr_h(curr_h), _w_avail(w_avail){}
	~CoreRectBin(){}

	void addRect(const CoreRect& rect)
	{
		assert( _w_avail >= rect.w() );
		_w_avail -= rect.w();
	}
	const TWidth& availWidth() const { return _w_avail;}

private:
	TTime  _curr_h;
	TWidth _w_avail;
};

class TestRect
{
friend class Comp;
public:
	class Comp
	{
	public:
		bool operator () (const TestRect& r1, const TestRect& r2)
		{
			if(r1.canPartition() != r2.canPartition())
			{
				if(!r1.canPartition()) return true;
				else return false;
			}
			if(r1._begun != r2._begun)
			{
				if(r1._begun) return true;
				else return false;
			}
			if(r1.h() != r2.h())
				return r1.h() > r2.h();
			if(r1.w() != r2.w())
				return r1.w() > r2.w();
			assert(r1._test_id != r2._test_id);
			return r1._test_id < r2._test_id;
		}
	};
public:
	TestRect(
		const size_t& idx,
		const TTime& h, const TPower& w,
		const TPartition& p
	):
	_test_id(idx), _height(h), _width(w), _remain_partition(p),
	_begun(false)
	{}
	~TestRect(){}
	bool canPartition() const{return _remain_partition>1;}
	bool hasBegun() const {return _begun;}

	const size_t& id() const{return _test_id;}
	const TTime&  h() const {return _height;}
	const TPower& w() const {return _width;}

	void preempt(const TTime& h)
	{
		if(_height < h)
			_height = 0;
		else
		{
			_height = _height - h;
			assert(_remain_partition > 0);
			--_remain_partition;
			_begun = true;
		}
	}

private:
	size_t _test_id;
	TTime _height;
	TPower _width;
	TPartition _remain_partition;
	bool _begun;
};

}

Alg_Greedy::Alg_Greedy(const System& sys):
Algorithm(sys), _opt_time(0){}

Alg_Greedy::~Alg_Greedy(){}

Schedule Alg_Greedy::run()
{
	vector<CoreBus> tam_assign = computeBusAssignment();

	ConstraintTable cons_table = findPairWiseConstraints(tam_assign);

	vector<TestSchedule> test_schedule = computeSchedule(cons_table);

	return Schedule(_system, _opt_time, tam_assign, test_schedule);
}

vector<CoreBus>
Alg_Greedy::computeBusAssignment()
{
	vector<CoreBus> tam_assign;

	// Collect Rectangles
	vector<CoreRect> vec_rect;
	vec_rect.reserve( _system.getNumCores() );
	for(size_t i=0; i<_system.getNumCores(); ++i)
	{
		const TTime&  h = _system.getCoreExtTotalLength(i);
		const TWidth& w = _system.getCoreWidth(i);
		if(w > 0)
			vec_rect.push_back( CoreRect(i, h, w) );
	}
	sort(vec_rect.begin(), vec_rect.end(), CoreRect::comp);

	// Initialize
	const TWidth& w_max = _system.getTAMWidth();
	TTime curr_h = 0;
	// Implement Best Fit Decreasing Height Algorithm
	// TODO TAM Assignment and Test scheduling should be put together
	tam_assign.resize(_system.getNumCores());

	set< CoreRectBin, CoreRectBin::Comp > bins;
	TWidth w_max_avail = 0;
	for(size_t i=0; i<vec_rect.size(); ++i)
	{
		TWidth bit_begin = 0;
		if( vec_rect[i].w() > w_max_avail )
		{	// Create a new bin
			// since vec_rect[i] doesn't fit into any bin
			CoreRectBin new_bin(curr_h, w_max);
			new_bin.addRect(vec_rect[i]);
			bins.insert( new_bin );

			curr_h += vec_rect[i].h();
		}
		else
		{	// Find bin with min avail width 
			// that can accommodate vec_rect[i]
			set<CoreRectBin>::iterator pos;
			pos = bins.lower_bound(CoreRectBin(curr_h, vec_rect[i].w()));
			assert( pos != bins.end() );
			CoreRectBin tmp = *pos;
			bins.erase(pos);

			bit_begin = w_max - tmp.availWidth();
			tmp.addRect(vec_rect[i]);
			bins.insert( tmp );
		}
		// Find maximum of available width
		w_max_avail = (bins.rbegin())->availWidth();
		// Assign Bus for core
		const TCoreIndex& idx = vec_rect[i].coreIndex();
		TWidth len = vec_rect[i].w();
		tam_assign[idx].addBusBits(bit_begin, len);
	}

	return tam_assign;
}

vector<TestSchedule>
Alg_Greedy::computeSchedule(const ConstraintTable& cons_table)
{	// Collect Rectangles
	typedef set<TestRect, TestRect::Comp> RectSet;
	RectSet set_rect;
	for(size_t i=0; i<_system.getNumTests(); ++i)
	{
		const Test& t = _system.getTest(i);
		assert(t.partition > 0);
		TestRect new_rect(i, t.length, t.power, t.partition);
		set_rect.insert(new_rect);
	}

	// TODO TAM Assignment and Test scheduling should be put together
	vector<TestSchedule> test_schedule;
	test_schedule.resize(_system.getNumTests());

	vector<bool> completed;
	completed.resize(_system.getNumTests(), false);

	TPower w_avail;
	TTime curr_h = 0, elapsed = 0;
	while( !set_rect.empty() )
	{	// Create a new bin
		w_avail = _system.getMaxPower();
		curr_h += elapsed;

		// Find rectangles with top priority while meet width constraint
		vector<TestRect> scheduled;
		set<size_t> conflict_ids;
		RectSet::iterator it = set_rect.begin();
		while( it!= set_rect.end() )
		{
			RectSet::iterator pos = it;
			++it; // Increment before possible erase
			if(pos->w() <= w_avail
			&& conflict_ids.count(pos->id()) == 0) // Check conflict
			{
				bool is_prec_met = true;
				vector<size_t> prec = cons_table.getPreceedIds(pos->id()); 
				for(size_t i=0; i<prec.size(); ++i)
					if(! completed[ prec[i] ] )
					{	is_prec_met = false; break;}
				if(!is_prec_met) // Check precedence
					continue;
				w_avail -= it->w();
				// Add conflict constraint
				vector<size_t> tmp = cons_table.getConflictIds(pos->id()); 
				conflict_ids.insert(tmp.begin(), tmp.end());
				// Copy before erased
				scheduled.push_back( *pos );
				set_rect.erase( pos );
			}
		}
		assert( !scheduled.empty() );

		{	// Find height for this bin
			TTime must_elapse = 0;
			TTime may_elapse = scheduled[0].h();
			for(size_t i=0; i<scheduled.size(); ++i)
			{
				const TestRect& si = scheduled[i];
				if( !si.canPartition() && must_elapse < si.h() )
					must_elapse = si.h();
				if( si.h() < may_elapse)
					may_elapse = si.h();
			}
			elapsed = (must_elapse < may_elapse)? may_elapse:must_elapse;
			assert(elapsed > 0);
		}

		for(size_t i=0; i<scheduled.size(); ++i)
		{
			const size_t& tid = scheduled[i].id();
			test_schedule[tid].addTimeInterval(curr_h, elapsed);
			scheduled[i].preempt( elapsed );
			if( scheduled[i].h() > 0 )
				set_rect.insert( scheduled[i] );
			else
				completed[tid] = true;
		}
	}
	// Record found test time usage
	_opt_time = curr_h + elapsed;

	return test_schedule;
}

Alg_Greedy::ConstraintTable
Alg_Greedy::findPairWiseConstraints(const vector<CoreBus>& tam_assign)
{
	ConstraintTable cons_table(_system.getNumTests());

	for(size_t i=0; i<_system.getNumPrecedences(); ++i)
	{
		const System::Precedence& prec = _system.getPrecedence(i);
		for(size_t j=0; j+1<prec.size(); ++j)
			for(size_t k=j+1; k<prec.size(); ++k)
			{	cons_table.setPrecedence(prec[j], prec[k]);}
	}

	for(size_t i=1; i<_system.getNumTests(); ++i)
		for(size_t j=0; j<i; ++j)
		{
			const Test& t0 = _system.getTest(i);
			const Test& t1 = _system.getTest(j);
			const CoreBus& bus0 = tam_assign[t0.core];
			const CoreBus& bus1 = tam_assign[t1.core];

			// Find same resource constraint
			if( cons_table(i, j) != ConstraintTable::NONE)
			{	continue;}
			if( t0.core == t1.core
			|| ( t0.category==External
			  && t1.category==External
			  && CoreBus::isOverlap(bus0, bus1))
			|| (!t0.resource.empty()
			  && t0.resource==t1.resource) )
			{	cons_table.setSameResource(i, j);}
		}

	return cons_table;
}

