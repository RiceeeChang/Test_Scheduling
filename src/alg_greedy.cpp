#include <algorithm>
#include <map>
#include <set>

#include <iostream> //TODO

#include "alg_greedy.h"
#include "schedule.h"
#include "sequence_pair.h"
#include "test.h"

using namespace std;

namespace
{
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

	void addRect(const TAMSolution::CoreRect& rect)
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
	TAMSolution tam_sol = packCoreRectangle();

	vector<CoreBus> tam_assign = buildTAMAssignment(tam_sol);

	ConstraintTable cons_table = findPairWiseConstraints(tam_assign);

	MidSolution sol = packTestRectangle(cons_table);

	vector<TestSchedule> test_schedule = buildSchedule(sol);

	return Schedule(_system, _opt_time, tam_assign, test_schedule);
}

TAMSolution Alg_Greedy::packCoreRectangle()
{	// Collect Rectangles
	typedef TAMSolution::CoreRect CoreRect;
	vector<CoreRect> vec_rect;
	vec_rect.reserve( _system.getNumCores() );
	for(size_t i=0; i<_system.getNumCores(); ++i)
	{
		const TTime&  h = _system.getCoreExtTotalLength(i);
		const TWidth& w = _system.getCoreWidth(i);
		if(w > 0) // Don't add cores use zero width
			vec_rect.push_back( CoreRect(i, h, w) );
	}
	// Sort in decreasing order of height
	sort(vec_rect.begin(), vec_rect.end(), CoreRect::comp);

	// Initialize
	const TWidth& w_max = _system.getTAMWidth();
	TTime curr_h = 0;
	// Implement Best Fit Decreasing Height Algorithm
	typedef map< CoreRectBin, std::vector<size_t>, CoreRectBin::Comp > BinSet;
	BinSet bin_set;
	TWidth w_max_avail = 0;
	for(size_t i=0; i<vec_rect.size(); ++i)
	{
		vector<size_t> rect_in_bin;
		if( vec_rect[i].w() > w_max_avail )
		{	// Create a new bin
			// since vec_rect[i] doesn't fit into any bin
			CoreRectBin new_bin(curr_h, w_max);
			new_bin.addRect(vec_rect[i]);

			rect_in_bin.push_back( i );
			bin_set.insert(BinSet::value_type(new_bin, rect_in_bin));

			curr_h += vec_rect[i].h();
		}
		else
		{	// Find bin with least available width 
			// that can accommodate vec_rect[i]
			BinSet::iterator pos;
			pos = bin_set.lower_bound(CoreRectBin(curr_h, vec_rect[i].w()));
			assert( pos != bin_set.end() );
			CoreRectBin bin = pos->first;
			swap(rect_in_bin, pos->second);
			bin_set.erase(pos);

			bin.addRect(vec_rect[i]);
			rect_in_bin.push_back( i );
			bin_set.insert( BinSet::value_type(bin, rect_in_bin) );
		}
		// Find maximum of available width
		w_max_avail = (bin_set.rbegin())->first.availWidth();
	}

	// Construct sequence pair
	vector<size_t> loci_pos, loci_neg;
	loci_pos.reserve( vec_rect.size() );
	loci_neg.reserve( vec_rect.size() );
	for(BinSet::iterator it=bin_set.begin(); it!=bin_set.end(); ++it)
	{
		const vector<size_t>& rect_in_bin = it->second;
		for(vector<size_t>::const_reverse_iterator rit=rect_in_bin.rbegin();
			rit!=rect_in_bin.rend(); ++rit)
		{	loci_pos.push_back(*rit);}
		for(size_t i=0; i<rect_in_bin.size(); ++i)
		{	loci_neg.push_back(rect_in_bin[i]);}
	}
	assert(loci_pos.size() == vec_rect.size());
	assert(loci_neg.size() == vec_rect.size());

	return TAMSolution(vec_rect, SequencePair(loci_pos, loci_neg));
}

MidSolution Alg_Greedy::packTestRectangle(const ConstraintTable& cons_table)
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

	vector<MidSolution::PartedTest> vec_part_rect;

	// Vector to record if a test is completed
	vector<bool> completed;
	completed.resize(_system.getNumTests(), false);

	TTime curr_h = 0;
	vector<size_t> size_of_bins;
	size_of_bins.push_back(0);
	while( !set_rect.empty() )
	{	// Create a new bin
		TPower w_avail = _system.getMaxPower();

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
		size_of_bins.push_back( scheduled.size() + size_of_bins.back() );

		TTime elapsed;
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
			// Record test
			if( scheduled[i].h() > elapsed )
			{
				MidSolution::PartedTest tmp(tid, curr_h, elapsed, scheduled[i].w());
				vec_part_rect.push_back(tmp);
				scheduled[i].preempt( elapsed );
				set_rect.insert( scheduled[i] );
			}
			else
			{
				MidSolution::PartedTest tmp(tid, curr_h, scheduled[i].h(), scheduled[i].w());
				vec_part_rect.push_back(tmp);
				scheduled[i].preempt( elapsed );
				completed[tid] = true;
			}
		}
		curr_h += elapsed; // Increase time stamp
	}
	// Record found test time usage
	_opt_time = curr_h;

	// Construct sequence pair
	vector<size_t> loci_pos, loci_neg;
	loci_pos.reserve( vec_part_rect.size() );
	loci_neg.reserve( vec_part_rect.size() );
	size_t j = 1;
	for(size_t i=0; i<vec_part_rect.size(); ++i)
	{
		size_t pos_id = (size_of_bins[j] - 1) - i + size_of_bins[j-1];
		loci_pos.push_back( pos_id );
		loci_neg.push_back( i );
		if( i == (size_of_bins[j]-1) )
			++j;
	}
	assert( j==size_of_bins.size() );
	assert(loci_pos.size() == vec_part_rect.size());
	assert(loci_neg.size() == vec_part_rect.size());

	return MidSolution(vec_part_rect, SequencePair(loci_pos, loci_neg));
}

vector<CoreBus>
Alg_Greedy::buildTAMAssignment(const TAMSolution& tam_sol)
{	// TODO Reduce repeat code
	const SequencePair& seq_pair = tam_sol.getSequencePair();
	const vector<TAMSolution::CoreRect> rects =
		tam_sol.getCoreRects();

	vector<TWidth> core_y, max_y;
	core_y.resize(seq_pair.size());
	max_y.resize(seq_pair.size(), 0);

	// Consturct Weighted Longest Common Subsequence
	const vector<size_t>& pos_loci = seq_pair.getPositiveLoci();
	for(vector<size_t>::const_reverse_iterator it = pos_loci.rbegin();
		it != pos_loci.rend(); ++it)
	{	// In reverse order
		const size_t& r_i = *it;
		const size_t& i_neg = seq_pair.findNegative(r_i);

		core_y[ r_i ] = max_y[ i_neg ];
		TWidth new_max_y = max_y[ i_neg ] + rects[r_i].w();

		for(size_t j=i_neg; j<seq_pair.size(); ++j)
			if(new_max_y > max_y[j]) max_y[j] = new_max_y;
			else break;
	}

	vector<CoreBus> tam_assign;
	tam_assign.resize(_system.getNumCores());
	for(size_t i=0; i<core_y.size(); ++i)
	{
		const TCoreIndex& cid = rects[i].coreIndex();
		tam_assign[cid].addBusBits(core_y[i], rects[i].w());
	}
	return tam_assign;
}

vector<TestSchedule>
Alg_Greedy::buildSchedule(const MidSolution& sol)
{	// TODO Reduce repeat code
	const SequencePair& seq_pair = sol.getSequencePair();
	const vector<MidSolution::PartedTest> rects =
		sol.getPartedTestRects();

	vector<TTime> max_x;
	vector<TTime> test_x;
	max_x.resize(rects.size(), 0);
	test_x.resize(rects.size());

	// Consturct Weighted Longest Common Subsequence
	const vector<size_t>& pos_loci = seq_pair.getPositiveLoci();
	for(vector<size_t>::const_iterator it = pos_loci.begin();
		it != pos_loci.end(); ++it)
	{	// In normal order
		const size_t& r_i = *it;
		const size_t& i_neg = seq_pair.findNegative(r_i);

		test_x[r_i] = max_x[ i_neg ];
		TTime new_max_x = max_x[ i_neg ] + rects[r_i].h();

		for(size_t j=i_neg; j<seq_pair.size(); ++j)
			if(new_max_x > max_x[j]) max_x[j] = new_max_x;
			else break;
	}

	_opt_time = max_x.back();

	vector<TestSchedule> t_sch;
	t_sch.resize(_system.getNumTests());
	for(size_t i=0; i<sol.size(); ++i)
	{
		const size_t& tid = rects[i].getTestId();
		t_sch[tid].addTimeInterval(test_x[i], rects[i].h());
	}
	return t_sch;
}

ConstraintTable
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

