#include <iostream>
#include <cstdlib>
#include "lsh_ese.h"

using namespace std;

int
main(int argc, char **args) {
	if(argc < 2) {
		perror("usage: ./main size_of_dataset");
		exit(0);
	}
	int num = atoi(args[1]);
	LShashESE::randomDataSet("dataset.rand", num);
	// LShashESE::readDataSet("dataset.rand");
	//LShashESE lsese("dataset.rand");

	return 0;
}
