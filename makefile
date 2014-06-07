include makefile.include

TARGET = bin/TestScheduler 

.PHONY: ${TARGET}

all     : ${TARGET}

clean   :
	-$(RM) -f ${TARGET}
	$(MAKE) -C src/ clean

${TARGET}:
	$(MAKE) -C src/


