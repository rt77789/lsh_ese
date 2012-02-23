#

CC=cc
CPP=g++

CFLAGS=-Wall -O -g

OBJ=complex.o fft.o

.PHONY : all clean

all: test

test: complex.h fft.h ${OBJ} test.cpp 
	$(CPP) $(CFLAGS) ${OBJ} test.cpp -o test

${OBJ}:%.o:%.cpp
	${CPP} ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o test
