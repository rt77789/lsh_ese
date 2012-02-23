CC=cc
CPP=g++

CFLAGS=-Wall -O -g

UTILS=../utils
FFT=../fft
STRUCTS=../structs

OBJ=${UTILS}/util.o ${UTILS}/config.o ${FFT}/fft.o ${FFT}/complex.o ghash.o lshash.o

DEPEND=${STRUCTS}/searcher.h ${STRUCTS}/candidate.h ${STRUCTS}/bench.h ${UTILS}/util.h

.PHONY : all clean

all: main test 

main: ghash.h ${OBJ} main.cpp ${DEPEND}
	$(CPP) $(CFLAGS) ${OBJ} main.cpp -o main 
test: ghash.h ${OBJ} test.cpp ${DEPEND}
	$(CPP) $(CFLAGS) ${OBJ} test.cpp -o test 

${OBJ}:%.o:%.cpp
	${CPP} ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o main test
