#include <iostream>
#include <cstdlib>
#include <cstring>
#include "lsh_ese.h"

using namespace std;

int
main(int argc, char **args) {
	if(argc < 4) {
		perror("usage: ./main (-rand #size_of_dataset output_file | -trans input_file output_file)");
		exit(0);
	}
	if(strcmp(args[1], "-rand") == 0) {

		char outfile[1024];
		strcpy(outfile, args[3]);
		strcat(outfile, ".");
		strcat(outfile, args[2]);

		int num = atoi(args[2]);
		LShashESE::randomDataSet(outfile, num);
	}
	else if(strcmp(args[1], "-trans") == 0) {
		LShashESE::transformDataSet(args[2], args[3]);	
	}
	else {
		perror("usage: ./main (-rand #size_of_dataset output_file | -trans input_file output_file)");
		exit(0);
	}
	return 0;
}
