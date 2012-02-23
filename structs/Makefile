
CPP=g++

## compile flags.
CFLAGS = -Wall -O -g 

OBJ=../utils/config.o ../fft/fft.o ../fft/complex.o

## *.h files, not need compile.

all: test_case
#	echo "structs directory make operation"

test_case: test_case.cpp point.h searcher.h search_res.h candidate.h ${OBJ}
	${CPP} ${CFLAGS} ${OBJ} test_case.cpp -o test_case	

clean:
	rm -rf test_case
	#echo "structs directory make clean operation"
