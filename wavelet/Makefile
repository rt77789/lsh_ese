CC=cc
CPP=g++

CFLAGS=-Wall -O -g -pthread

OBJ=weps.o utils.o sac_prep.o

.PHONY : clean

ese: sacio.o ${OBJ} main.cxx
	$(CPP) $(CFLAGS) sacio.o ${OBJ} main.cxx -o ese

${OBJ}: %.o:%.cxx
	${CPP} ${CFLAGS} -c $< -o $@

sacio.o: sac.h sacio.c
	$(CPP) $(CFLAGS) -c sacio.c -o sacio.o

clean:
	rm -rf *.o ese
