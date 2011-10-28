CC=cc
CPP=g++

CFLAGS=-Wall -O -g

OBJ=util.o ghash.o lshash.o

.PHONY : all clean

all: lsh test

lsh: ghash.h ${OBJ} main.cpp
	$(CPP) $(CFLAGS) ${OBJ} main.cpp -o lsh

test: ghash.h ${OBJ} test.cpp 
	$(CPP) $(CFLAGS) ${OBJ} test.cpp -o lsh

${OBJ}:%.o:%.cpp
	${CPP} ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o lsh
