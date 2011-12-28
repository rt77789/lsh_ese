
#

CC=cc
CPP=g++

UTILS=../utils/
FFT=../fft/
LSHKIT_INCLUDE=./include/
STRUCTS=../structs/

CFLAGS=-Wall -O -g 
INCLUDE=-I${LSHKIT_INCLUDE} -I/usr/local/include/
LIBLSH=./liblshkit.a
RDYNAMIC=-rdynamic ${LIBLSH} -Wl,-Bstatic -lboost_program_options-mt -Wl,-Bdynamic -L/usr/local/lib -lgsl -lgslcblas -lm


OBJ=${UTILS}/util.o ${UTILS}/config.o mplshash.o ${FFT}/fft.o ${FFT}/complex.o

DEPEND=${STRUCTS}/searcher.h ${STRUCTS}/candidate.h ${STRUCTS}/bench.h ${UTILS}/util.h

.PHONY : all

all: main mplshash.o

mplshash.o: mplshash.h mplshash.cpp
	${CPP}  ${INCLUDE} -o mplshash.o -c mplshash.cpp

main: ${OBJ} ${DEPEND} main.cpp
	${CPP} ${INCLUDE} ${OBJ} main.cpp -o main ${RDYNAMIC}

clean:
	rm -rf *.o main

