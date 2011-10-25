CC=cc
CPP=g++

CFLAGS=-Wall -O -g

lsh: ghash.h main.cpp util.o ghash.o lshash.o 
	$(CPP) $(CFLAGS) util.o ghash.o lshash.o main.cpp -o lsh

test: ghash.h test.cpp util.o ghash.o lshash.o
	$(CPP) $(CFLAGS) util.o ghash.o lshash.o test.cpp -o lsh
util.o: util.h util.cpp
	$(CPP) $(CFLAGS) -c util.cpp -o util.o

ghash.o: ghash.h ghash.cpp
	$(CPP) $(CFLAGS) -c ghash.cpp -o ghash.o

lshash.o: lshash.h lshash.cpp
	$(CPP) $(CFLAGS) -c lshash.cpp -o lshash.o


clean:
	rm -rf *.o lsh
