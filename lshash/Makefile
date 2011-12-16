CC=cc
CPP=g++

CFLAGS=-Wall -O -g

UTILS=../utils

OBJ=${UTILS}/util.o ${UTILS}/config.o ghash.o lshash.o

.PHONY : all clean

all: main test 

main: ghash.h ${OBJ} main.cpp
	$(CPP) $(CFLAGS) ${OBJ} main.cpp -o main 
test: ghash.h ${OBJ} test.cpp 
	$(CPP) $(CFLAGS) ${OBJ} test.cpp -o test 

${OBJ}:%.o:%.cpp
	${CPP} ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o lsh test
