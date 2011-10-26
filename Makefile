CC=cc
CPP=g++
LSH=lshash
WAVE=wavelet

CFLAGS = -Wall -O -g


main: $(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o $(WAVE)/weps.o $(WAVE)/utils.o lsh_ese.h lsh_ese.o main.cpp
	$(CPP) $(CFLAGS) $(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o $(WAVE)/weps.o $(WAVE)/utils.o lsh_ese.o main.cpp -o main

test: $(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o $(WAVE)/weps.o $(WAVE)/utils.o lsh_ese.h lsh_ese.o test.cpp
	$(CPP) $(CFLAGS) $(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o $(WAVE)/weps.o $(WAVE)/utils.o lsh_ese.o test.cpp -o test 


lsh_ese.o:  lsh_ese.h lsh_ese.cpp
	$(CPP) $(CFLAGS) -c lsh_ese.cpp -o lsh_ese.o 

clean:
	rm -rf *.o main
