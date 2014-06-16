#include <cassert>

#include "schedule.h"
#include "test.h"

using namespace std;

bool CoreBus::isOverlap(const CoreBus& b0, const CoreBus& b1)
{
	// TODO Speed up for multiple intervals
	for(size_t i=0; i<b0._bit_itvl.size(); ++i)
	{
		const BusBits& itvl0 = b0._bit_itvl[i];
		for(size_t j=0; j<b1._bit_itvl.size(); ++j)
		{
			const BusBits& itvl1 = b1._bit_itvl[j];
			if(itvl0.first <= itvl1.second 
			&& itvl1.first <= itvl0.second)
				return true;
		}
	}
	return false;
}

ostream& operator << (ostream& out, const CoreBus& a)
{	// Note: Print in decreasing order
	vector<CoreBus::BusBits>::const_reverse_iterator it;
	it = a._bit_itvl.rbegin();
	for(; it!=a._bit_itvl.rend(); ++it)
		out	<< "[" << it->second << ":" << it->first << "]";
	return out;
}

void
TestSchedule::addTimeInterval(
	const TTime& begin, const TTime& length
)
{
	vector<Interval>::iterator it = _itvls.begin();
	for(; it!=_itvls.end(); ++it)
	{
		if(it->first > begin)
			break;
		assert(it->second < begin);
	}

	TTime end = begin + length - 1;
	_itvls.insert( it, Interval(begin, end) );
}

ostream& operator << (ostream& out, const TestSchedule& s)
{
	for(size_t i=0; i<s._itvls.size(); ++i)
		out << "(" << s._itvls[i].first
		    << "," << s._itvls[i].second << ")";
	return out;
}

ostream& operator << (ostream& out, const Schedule& sch)
{
	const System& sys = sch._system;
	const vector<CoreBus>& tam_assign = sch._TAM_assignment;
	const vector<TestSchedule>& test_schedule = sch._test_schedule;
	

	out << "Schedule" << endl
	    << "begin" << endl
	    << endl
	    << "Test_time " << sch._optimal_test_time << endl
	    << endl;
	for(size_t i=0; i<tam_assign.size(); ++i)
	{
		out << "TAM_assignment " << sys.getCoreName(i) << " "
		    << tam_assign[i] << endl;
	}
	out << endl;

	for(size_t i=0; i<test_schedule.size(); ++i)
	{
		const Test& ti = sys.getTest(i);
		if(ti.category == BIST)
			out << "BIST";
		else
			out << "External";
		out << ' ' << ti.name << ' ' << test_schedule[i] << endl;
	}
	out << endl
	    << "end" << endl;
	return out;
}

