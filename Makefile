## Macros.
CC=cc
CPP=/usr/bin/c++
GPP=g++
LSH=lshash
WAVE=wavelet
FFT=fft
MPLSH=mplshash
LIBLSHKIT=${MPLSH}

CFLAGS = -Wall -O -g 
INCLUDE=-I/home/rt777789/code/project/lshkit/lshkit-0.2.1/include -I/usr/local/include 
RDANAMIC = -rdynamic ${LIBLSHKIT}/liblshkit.a -Wl,-Bstatic -lboost_program_options-mt -Wl,-Bdynamic -L/usr/local/lib -lgsl -lgslcblas -lm


OBJ=$(LSH)/util.o $(LSH)/ghash.o $(LSH)/lshash.o $(WAVE)/weps.o\
	$(WAVE)/utils.o ${FFT}/fft.o ${FFT}/complex.o ${MPLSH}/mplshash.o lsh_ese.o

LSH_ESE_DEP=${LSH}/util.h ${LSH}/point.h ${LSH}/lshash.h ${WAVE}/weps.h ${WAVE}/utils.h ${MPLSH}/mplshash.h lsh_ese.h lsh_ese.cpp


all: main test

main: fft_make lshash_make wavelet_make mplsh_make ${OBJ} lsh_ese.h main.cpp
	$(GPP) ${INCLUDE} ${OBJ} main.cpp -o main ${RDANAMIC}

test: lshash_make wavelet_make mplsh_make ${OBJ} lsh_ese.h test.cpp
	$(GPP) ${INCLUDE} ${OBJ} test.cpp -o test ${RDANAMIC}

lsh_ese.o:  ${LSH_ESE_DEP}
	$(GPP) ${INCLUDE} -c lsh_ese.cpp -o lsh_ese.o 

fft_make:
	${MAKE} -C ${FFT}

lshash_make:
	${MAKE} -C ${LSH}

wavelet_make:
	${MAKE} -C ${WAVE}

mplsh_make:
	${MAKE} -C ${MPLSH}

clean:
	cd ${LSH}; make clean 
	cd ${WAVE}; make clean
	cd ${FFT}; make clean
	cd ${MPLSH}; make clean

	rm -rf *.o main test
