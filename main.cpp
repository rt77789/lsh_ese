#include <iostream>
#include <cstdlib>
#include <cstring>
#include "lsh_ese.h"

using namespace std;

int
main(int argc, char **args) {
	if(argc < 2) {
		perror("usage: ./main size_of_dataset");
		exit(0);
	}

	char outfile[1024];
	strcpy(outfile, "dataset.rand.");
	strcat(outfile, args[1]);

	int num = atoi(args[1]);
	LShashESE::randomDataSet(outfile, num);
	// LShashESE::readDataSet("dataset.rand");
	//LShashESE lsese("dataset.rand");

	return 0;
}
