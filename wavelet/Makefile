CC=cc
CPP=g++

CFLAGS=-Wall -O -g -pthread

UTILS=../utils

OBJ=${UTILS}/util.o ${UTILS}/config.o weps.o sac_prep.o

.PHONY : clean

all: main test

main: sacio.o ${OBJ} main.cpp
	$(CPP) $(CFLAGS) sacio.o ../fft/complex.o ../fft/fft.o ${OBJ} main.cpp -o main

test: ${OBJ} wavelet.o test.cpp
	${CPP} ${CFLAGS} sacio.o ../fft/complex.o ../fft/fft.o ${OBJ} wavelet.o test.cpp -o test

${OBJ}: %.o:%.cpp
	${CPP} ${CFLAGS} -c $< -o $@

sacio.o: sac.h sacio.c
	$(CPP) $(CFLAGS) -c sacio.c -o sacio.o

wavelet.o: wavelet.h wavelet.cpp
	${CPP} ${CFLAGS} -c wavelet.cpp -o wavelet.o

clean:
	rm -rf *.o main 
