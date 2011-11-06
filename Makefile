## Macros.
CC=cc
CPP=g++
LSH=lshash
WAVE=wavelet
FFT=fft

CFLAGS = -Wall -O -g

OBJ=$(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o $(WAVE)/weps.o\
	$(WAVE)/utils.o ${FFT}/fft.o ${FFT}/complex.o lsh_ese.o


all: main test

main: lshash_make wavelet_make fft_make ${OBJ} lsh_ese.h main.cpp
	$(CPP) $(CFLAGS) ${OBJ} main.cpp -o main

test: lshash_make wavelet_make ${OBJ} lsh_ese.h test.cpp
	$(CPP) $(CFLAGS) ${OBJ} test.cpp -o test 

lsh_ese.o:  lsh_ese.h lsh_ese.cpp
	$(CPP) $(CFLAGS) -c lsh_ese.cpp -o lsh_ese.o 

lshash_make:
	${MAKE} -C ${LSH}

wavelet_make:
	${MAKE} -C ${WAVE}

fft_make:
	${MAKE} -C ${FFT}

clean:
	cd ${LSH}; make clean 
	cd ${WAVE}; make clean
	cd ${FFT}; make clean

	rm -rf *.o main test
