include ../makefile.include

SRC = $(wildcard *.cpp)
TARGET = ../bin/TestScheduler 

all: ${TARGET}

clean:
	rm -rf *.o *.d ${TARGET}

${TARGET} : $(SRC:%.cpp=%.o)
	$(LD) -o $@ $^

-include $(SRC:%.cpp=%.d)

