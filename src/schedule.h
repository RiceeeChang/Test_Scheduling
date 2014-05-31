#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <string>
#include <utility>
#include <vector>

#include "type_defs.h"

class System;

class Schedule{
	typedef std::pair<TWidth, TWidth> BusBits;
	typedef std::vector<BusBits> CoreAssignment;

	typedef std::pair<TTime, TTime> Interval;
	typedef std::vector<Interval> TestSchedule;

	public:
		TTime optimal_test_time;

		std::vector<BusBits> TAM_assignment;
		std::vector<TestSchedule> timestamps;
};

void write_output(const std::string& fname, const System& sys, const Schedule& sch);

#endif
