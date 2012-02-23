#include <iostream>
#include <cstdlib>
#include <cstring>
#include "lsh_ese.h"
#include "structs/candidate.h"
#include "utils/config.h"

using namespace std;

void test() {
	Configer::init("all.config");
	vector<u_int> eid;
	eid.push_back(1310903);
	vector<Point> res;
	res.swap(Candidate::get(eid));

	for(size_t i = 0; i < res.size(); ++i) {
		for(int j = 0; j < DIMS; ++j) {
		cout << res[i].d[j] << " ";
		}
		cout << endl;
	}
}

int
main(int argc, char **args) {
	//test();
	if(argc < 4) {
		perror("usage: \n\t./main (-rand | -trans | -itrans)\n\t\t-rand #size_of_dataset output_file\n\t\t-trans input_file output_file\n\t\t-itrans input_file output_file");
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
	else if(strcmp(args[1], "-itrans") == 0) {
		LShashESE::iTransformDataSet(args[2], args[3]);
	}
	else {
		perror("usage: \n\t./main (-rand | -trans | -itrans)\n\t\t-rand #size_of_dataset output_file\n\t\t-trans input_file output_file\n\t\t-itrans input_file output_file");
		exit(0);
	}
	return 0;
}
