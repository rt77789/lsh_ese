CC=cc
CPP=g++

CFLAGS=-Wall -O -g -pthread

OBJ=weps.o utils.o sac_prep.o

.PHONY : clean

all: ese test

ese: sacio.o ${OBJ} main.cxx
	$(CPP) $(CFLAGS) sacio.o ../fft/complex.o ../fft/fft.o ${OBJ} main.cxx -o ese

test: ${OBJ} wavelet.o test.cpp
	${CPP} ${CFLAGS} sacio.o ../fft/complex.o ../fft/fft.o ${OBJ} wavelet.o test.cpp -o test

${OBJ}: %.o:%.cxx
	${CPP} ${CFLAGS} -c $< -o $@

sacio.o: sac.h sacio.c
	$(CPP) $(CFLAGS) -c sacio.c -o sacio.o

wavelet.o: wavelet.h wavelet.cpp
	${CPP} ${CFLAGS} -c wavelet.cpp -o wavelet.o

clean:
	rm -rf *.o ese
