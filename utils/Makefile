## Macros.
CC=cc
CPP=g++

CFLAGS = -Wall -O -g 
INCLUDE=
CONFIG_TEST_OBJ=util.o config.o

.PHONY : test_case config.o util.o

all: test_case 

test_case: ${CONFIG_TEST_OBJ} test_case.cpp
	${CPP} ${INCLUDE} ${CFLAGS} ${CONFIG_TEST_OBJ} test_case.cpp -o test_case 

config.o: config.h config.cpp
	${CPP} ${INCLUDE} ${CFLAGS} -c config.cpp -o config.o

util.o: util.h util.cpp
	${CPP} ${INCLUDE} ${CFLAGS} -c util.cpp -o util.o

clean:
	rm -rf *.o test_case test.config

