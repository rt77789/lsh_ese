## Macros.
CC=cc
CPP=g++


## self-definition structs.
STRUCTS=../structs
## self-definition utils.
UTILS=../utils

FFT=../fft

prefix=/usr/local
libdir=${prefix}/lib64
includedir=${prefix}/include
libs=/usr/lib64

flann_includedir=./src/cpp/
flann_libdir=./lib/


##
LIB_OPTION=-L${libdir} -L${flann_libdir} -L${libs} -lflann -lhdf5
## compile flags.
CFLAGS= -Wall -O -g -I${includedir} -I${flann_includedir}
#

INCLUDE=-I${includedir} -I${flann_includedir}


OBJ=${UTILS}/util.o ${UTILS}/config.o flann_interface.o ${FFT}/fft.o ${FFT}/complex.o 
DEPEND=${STRUCTS}/searcher.h ${STRUCTS}/candidate.h ${STRUCTS}/bench.h ${UTILS}/util.h

all: test_case main

main: ${OBJ} ${DEPEND} main.cpp
	${CPP} ${CFLAGS} ${INCLUDE} ${LIB_OPTION} ${OBJ} main.cpp -o main

test_case: ${OBJ} ${DEPEND} test_case.cpp
	${CPP} ${CFLAGS} ${INCLUDE} ${LIB_OPTION} ${OBJ} test_case.cpp -o test_case

flann_interface.o: flann_interface.h flann_interface.cpp
	${CPP} ${CFLAGS} ${INCLUDE} -c flann_interface.cpp -o flann_interface.o


clean:
	rm -rf *.o test_case main
