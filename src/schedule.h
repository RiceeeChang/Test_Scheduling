#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "type_defs.h"

class Test;
class System;

class CoreBus
{
friend std::ostream& operator << (std::ostream&, const CoreBus&);
public:
	typedef std::pair<TWidth, TWidth> BusBits;
	static bool isOverlap(const CoreBus&, const CoreBus&);
public:
	void addBusBits(const TWidth& begin, const TWidth& n)
	{
		TWidth end = begin + n - 1;
		_bit_itvl.push_back( BusBits(begin, end) );
	}
private:
	std::vector<BusBits> _bit_itvl;
};
std::ostream& operator << (std::ostream&, const CoreBus&);

class TestSchedule
{
friend std::ostream& operator << (std::ostream&, const TestSchedule&);
public:
	typedef std::pair<TTime, TTime> Interval;
public:
	void addTimeInterval(const TTime&, const TTime&);
private:
	std::vector<Interval> _itvls;
};
std::ostream& operator << (std::ostream&, const TestSchedule&);

class Schedule{
friend std::ostream& operator << (std::ostream&, const Schedule&);
public:
	Schedule(
		const System& sys,
		const TTime& opt_time,
		const std::vector<CoreBus>& assign,
		const std::vector<TestSchedule>& t_sch
	): _system(sys), _optimal_test_time(opt_time),
	_TAM_assignment(assign), _test_schedule(t_sch)
	{}

	~Schedule(){}

private:
	const System& _system;
	TTime _optimal_test_time;

	std::vector<CoreBus> _TAM_assignment;
	std::vector<TestSchedule> _test_schedule;
};
std::ostream& operator << (std::ostream&, const Schedule&);

#endif
