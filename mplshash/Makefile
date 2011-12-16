
#

CC=cc
CPP=g++

UTILS=../utils/
LSHKIT_INCLUDE=./include/

CFLAGS=-Wall -O -g 
INCLUDE=-I${LSHKIT_INCLUDE} -I/usr/local/include/
LIBLSH=./liblshkit.a
RDYNAMIC=-rdynamic ${LIBLSH} -Wl,-Bstatic -lboost_program_options-mt -Wl,-Bdynamic -L/usr/local/lib -lgsl -lgslcblas -lm


OBJ=${UTILS}/config.o mplshash.o 

.PHONY : all

all: main

mplshash.o: mplshash.h mplshash.cpp
	${CPP}  ${INCLUDE} -o mplshash.o -c mplshash.cpp

main: ${OBJ}
	${CPP} ${INCLUDE} ${OBJ} main.cpp -o main ${RDYNAMIC}

clean:
	rm -rf *.o

