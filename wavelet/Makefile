CC=cc
CPP=g++

CFLAGS=-Wall -O -g -pthread

ese: weps.o sacio.o sac_prep.o utils.o
	$(CPP) $(CFLAGS) sacio.o weps.o sac_prep.o utils.o main.cxx -o ese

weps.o: weps.h weps.cxx
	$(CPP) $(CFLAGS) -c weps.cxx -o weps.o

utils.o: utils.h utils.cxx
	$(CPP) $(CFLAGS) -c utils.cxx -o utils.o

sac_prep.o: sac_prep.h sac_prep.cxx
	$(CPP) $(CFLAGS) -c sac_prep.cxx -o sac_prep.o

sacio.o: sac.h sacio.c
	$(CPP) $(CFLAGS) -c sacio.c -o sacio.o

clean:
	rm -rf *.o ese
