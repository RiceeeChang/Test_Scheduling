#CC and CFLAGS are variables
CC = g++
CFLAGS = -c
AR = ar
ARFLAGS = rcv
DBGFLAGS = -g -D_DEBUG_ON_
OPTFLAGS = -O2

all     : bin/TestScheduler
	@echo -n ""

clean   :
	rm *.o ../bin/TestScheduler

# optimized version
bin/TestScheduler : main.o test.o lowerbound.o greedy.o
		$(CC) $^ -o $@

main.o        : src/main.cpp src/lowerbound.h src/test.h
		$(CC) $(CFLAGS) -c $< -o $@

test.o            : src/test.cpp src/test.h
		$(CC) $(CFLAGS) -c $< -o $@
lowerbound.o      : src/lowerbound.cpp src/lowerbound.h src/test.h
		$(CC) $(CFLAGS) -c $< -o $@

greedy.o          : src/greedy.cpp src/lowerbound.h src/test.h
		$(CC) $(CFLAGS) -c $< -o $@

