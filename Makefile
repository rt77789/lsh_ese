CC=cc
CPP=g++
LSH=lshash

CFLAGS = -Wall -O -g


main: $(LSH)/ghash.h $(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o main.cpp
	$(CPP) $(CFLAGS) $(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o main.cpp -o main


clean:
	rm -rf *.o main
