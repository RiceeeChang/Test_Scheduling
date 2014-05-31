#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "alg.h"
#include "test.h"

using namespace std;

namespace
{

// Functional class for computing lower bound
template <class TRectH, class TRectW>
class LowerBoundFor2SP{
private:
	typedef TRectH TRectArea;
	typedef pair<TRectH, TRectW> Rect;
	static bool compW(const Rect& t1, const Rect& t2)
	{	return t1.second < t2.second;}

public:
	void addRect(const TRectH& h, const TRectW& w)
	{	_vec_rect.push_back(Rect(h, w));}

	TRectH compute(const TRectW& w_bound);

private:
	vector<Rect> _vec_rect;
};

template <class TRectH, class TRectW>
TRectH LowerBoundFor2SP<TRectH, TRectW>::compute(const TRectW& w_bound)
{
	if(_vec_rect.size() == 0) return 0;

	sort(_vec_rect.begin(), _vec_rect.end(), compW);
	assert( _vec_rect.front().second > 0 );
	assert( _vec_rect.back().second <= w_bound );

	TRectH partA = 0;
	TRectArea partB = 0, partC = 0;

    // Compute L(alpha) where alpha = 1
	size_t i = 0, begin_J3 = 0;
	size_t begin_J2, begin_J1;
	TRectW half_w_bound = w_bound / 2;
	// Part B is from the rectangles in J3
	for(; i<_vec_rect.size(); ++i)
	{
		const TRectW& w = _vec_rect[i].second;
		const TRectH& h = _vec_rect[i].first;
		if( half_w_bound >= w) // W/2 >=2 && w >= 1
			partB += w * h;
		else
			break;
	}
	begin_J2 = i;
	// Part C is from the rectangles in J2
	for(; i<_vec_rect.size(); ++i)
	{
		const TRectW& w = _vec_rect[i].second;
		const TRectH& h = _vec_rect[i].first;
		if( w_bound > w ) // W-1 >= w && w > W/2
		{
			partA += h; // Sum of height in J2
			partC += (w_bound-w) * h;
		}
		else
			break;
	}
	begin_J1 = i;
	// Part A is from the sum of rectangles in J1 & J2
	for(; i<_vec_rect.size(); ++i)
	{
		const TRectH& h = _vec_rect[i].first;
		partA += h; // Add sum of height in J1
	}

    // max(0, (partB - partC)/w)
	TRectH lower_bound = 0;
	if(partB >= partC + w_bound)
	{ // ceiling function is omitted
		lower_bound = ( (partB - partC) / w_bound );
	}

	// Compute other L(alpha)
	TRectW old_alpha = 1;
	for(i=0; i<begin_J2; ++i)
	{
		const TRectW& new_alpha = _vec_rect[i].second;
		if(new_alpha == old_alpha)
			continue;
		// else when alpha is changed
		for(; begin_J3 < begin_J2; ++begin_J3)
		{	// Update partB
			const TRectW& w = _vec_rect[begin_J3].second;
			const TRectH& h = _vec_rect[begin_J3].first;
			if(w < new_alpha)
				partB -= w*h;
			else
				break;
		}
		for(; begin_J1 > begin_J2; --begin_J1)
		{	// Update part C
			const TRectW& w = _vec_rect[begin_J1-1].second;
			const TRectH& h = _vec_rect[begin_J1-1].first;
			if( w > w_bound - new_alpha )
				partC -= (w_bound - w) * h;
			else
				break;
		}

		TRectH tmp_lower = 0;
		if(partB >= partC + w_bound)
		{ // ceiling function is omitted
			tmp_lower = ( (partB - partC) / w_bound );
			if( lower_bound < tmp_lower )
				lower_bound = tmp_lower;
		}
	}

	// Add partA at last
	lower_bound += partA;
	return lower_bound;
}

}

Algorithm::Algorithm(const System& s):
_system(s)
{
	_lower = computeLowerBound();
	_upper = computeUpperBound();
	assert(_lower <= _upper);
}

Algorithm::~Algorithm(){}

TTime
Algorithm::computeLowerBound() const
{
	TTime lower_bound;

	LowerBoundFor2SP<TTime, TWidth> lb_by_power;
	for(size_t i=0; i<_system.getNumTests(); ++i)
	{
		const Test& ti = _system.getTest(i);
		if(ti.power > 0)
			lb_by_power.addRect(ti.length, ti.power);
	}

	TTime tmp_lower = lb_by_power.compute(_system.sys_power);

	LowerBoundFor2SP<TTime, TWidth> lb_by_width;
	for(size_t i=0; i<_system.getNumCores(); ++i)
	{
		const TTime&  h = _system.getCoreExtTotalLength(i);
		const TWidth& w = _system.getCoreWidth(i);
		if( w > 0 )
			lb_by_width.addRect(h, w);
	}
	lower_bound = lb_by_width.compute(_system.sys_TAM_width);

	if(lower_bound < tmp_lower)
		lower_bound = tmp_lower;

	return lower_bound;
}

TTime
Algorithm::computeUpperBound() const
{
	TTime upper_bound = 0;
	for(size_t i=0; i<_system.getNumTests(); ++i)
	{
		const Test& ti = _system.getTest(i);
		upper_bound += ti.length;
	}
	return upper_bound;
}

