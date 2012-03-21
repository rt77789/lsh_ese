## Macros.
CC=cc
CPP=g++

## lshash directory.
LSH=lshash
## wavelet directory.
WAVE=wavelet
## fft directory.
FFT=fft
## mplshash directory.
MPLSH=mplshash
##
FLANN=flann

## self-definition structs.
STRUCTS=structs
## self-definition utils.
UTILS=utils

## liblshkit directory.
LIBLSHKIT=${MPLSH}

## compile flags.
CFLAGS = -Wall -O -g 

## FLANN MACROS.
prefix=/usr/local
libdir=${prefix}/lib64
includedir=${prefix}/include
libs=/usr/lib64
## FLANN INCLUDE DIR & LIB DIR.
flann_includedir=${FLANN}/src/cpp/
flann_libdir=${FLANN}/lib/

## compile include path.
INCLUDE=-I${flann_includedir} -I${MPLSH}/include -I/usr/local/include 
## FLANN option
FLANN_LIB_OPTION=-L${libdir} -L${flann_libdir} -L${libs} -lflann -lhdf5

## dynamic lib option.
RDANAMIC = -rdynamic ${LIBLSHKIT}/liblshkit.a -Wl,-Bstatic -lboost_program_options-mt -Wl,-Bdynamic -L/usr/local/lib -lgsl -lgslcblas -lm

OBJ=${FLANN}/flann_interface.o ${UTILS}/util.o ${UTILS}/config.o $(LSH)/ghash.o $(LSH)/lshash.o $(WAVE)/weps.o\
	${FFT}/fft.o ${FFT}/complex.o ${MPLSH}/mplshash.o lsh_ese.o ${WAVE}/sacio.o ${WAVE}/sac_prep.o

LSH_ESE_DEP=${FLANN}/flann_interface.h ${STRUCTS}/point.h ${LSH}/lshash.h ${WAVE}/weps.h ${UTILS}/util.h ${MPLSH}/mplshash.h lsh_ese.h lsh_ese.cpp


all: main test

main: fft_make lshash_make wavelet_make mplsh_make flann_make structs_make utils_make ${OBJ} main.cpp
#main: ${OBJ} main.cpp
	$(CPP) ${INCLUDE} ${FLANN_LIB_OPTION} ${OBJ} main.cpp -o main ${RDANAMIC}

test: lshash_make wavelet_make mplsh_make flann_make structs_make utils_make ${OBJ} lsh_ese.h test.cpp
#test: ${OBJ} test.cpp
	$(CPP) ${INCLUDE} ${FLANN_LIB_OPTION} ${OBJ} test.cpp -o test ${RDANAMIC}

lsh_ese.o:  ${LSH_ESE_DEP}
	$(CPP) ${INCLUDE} -c lsh_ese.cpp -o lsh_ese.o 

fft_make:
	${MAKE} -C ${FFT}

lshash_make:
	${MAKE} -C ${LSH}

wavelet_make:
	${MAKE} -C ${WAVE}

mplsh_make:
	${MAKE} -C ${MPLSH}

flann_make:
	${MAKE} -C ${FLANN}

structs_make:
	${MAKE} -C ${STRUCTS}

utils_make:
	${MAKE} -C ${UTILS}

clean:
	cd ${LSH}; make clean 
	cd ${WAVE}; make clean
	cd ${FFT}; make clean
	cd ${MPLSH}; make clean
	cd ${FLANN}; make clean
	cd ${STRUCTS}; make clean
	cd ${UTILS}; make clean

	rm -rf *.o main test
