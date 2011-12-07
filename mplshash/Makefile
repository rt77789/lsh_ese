
#

CC=cc
CPP=/usr/bin/c++
GPP=g++


CFLAGS=-Wall -O -g 
INCLUDE=-I/home/rt777789/code/project/lshkit/lshkit-0.2.1/include -I/usr/local/include 

OBJ=mplsh.o

.PHONY : all

all: mplsh main

mplsh: mplshash.h mplshash.cpp
	${CPP}  ${INCLUDE} -o mplshash.o -c mplshash.cpp

main: mplsh main.cpp
	${CPP} ${INCLUDE} mplshash.o main.cpp -o main -rdynamic ./liblshkit.a -Wl,-Bstatic -lboost_program_options-mt -Wl,-Bdynamic -L/usr/local/lib -lgsl -lgslcblas -lm



clean:
	rm -rf *.o

