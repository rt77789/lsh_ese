## Macros.
CC=cc
CPP=g++


## self-definition structs.
STRUCTS=../structs
## self-definition utils.
UTILS=../utils

FLANN=../flann

FFT=../fft

##
## compile flags.
CFLAGS= -Wall -O -g
#

INCLUDE=


OBJ=${UTILS}/util.o ${UTILS}/config.o ${FFT}/fft.o ${FFT}/complex.o 
DEPEND=${STRUCTS}/searcher.h ${STRUCTS}/candidate.h ${STRUCTS}/bench.h ${UTILS}/util.h

all: main

main: ${OBJ} ${DEPEND} main.cpp
	${CPP} ${CFLAGS} ${INCLUDE} ${OBJ} main.cpp -o main

clean:
	rm -rf *.o main
