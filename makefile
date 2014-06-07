include makefile.include

TARGET = bin/TestScheduler 

all     : ${TARGET}

clean   :
	-$(RM) -f ${TARGET}
	$(MAKE) -C src/ clean

${TARGET}:
	$(MAKE) -C src/


