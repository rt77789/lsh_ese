## Macros.
CC=cc
CPP=g++

CFLAGS = -Wall -O -g 
INCLUDE=
CONFIG_TEST_OBJ=indexmapper.o util.o config.o sacio.o sac_prep.o

.PHONY : test_case config.o util.o indexmapper.o

all: test_case 

test_case: ${CONFIG_TEST_OBJ} test_case.cpp
	${CPP} ${INCLUDE} ${CFLAGS} ${CONFIG_TEST_OBJ} test_case.cpp -o test_case 

config.o: config.h config.cpp
	${CPP} ${INCLUDE} ${CFLAGS} -c config.cpp -o config.o

util.o: util.h util.cpp
	${CPP} ${INCLUDE} ${CFLAGS} -c util.cpp -o util.o

indexmapper.o: indexmapper.h indexmapper.cpp
	${CPP} ${INCLUDE} ${CFLAGS} -c indexmapper.cpp -o indexmapper.o

sac_prep.o: sac_prep.h sac_prep.cpp
	${CPP} ${INCLUDE} ${CFLAGS} -c sac_prep.cpp -o sac_prep.o

sacio.o: sac.h sacio.c
	${CPP} ${INCLUDE} ${CFLAGS} -c sacio.c -o sacio.o

clean:
	rm -rf *.o test_case test.config

