#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <string>
#include <utility>
#include <vector>

class System;

class Schedule{
	typedef std::pair<int, int> Interval;
	public:
		int optimal_test_time;

		std::vector<Interval> TAM_assignment;
		std::vector<std::vector<Interval> > timestamps;
};

void write_output(const std::string& fname, const System& sys, const Schedule& sch);

#endif
