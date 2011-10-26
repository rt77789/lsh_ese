#include "lsh_ese.h"
#include "lshash/ghash.h"
#include "lshash/util.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void
test_lshese(const char *dataset, u_int queryNum) {
	cout << "lsese constructing... | ";
	Util::print_now();

	LShashESE lsese(dataset);

	cout << "readDataSet-#queryNum=" << queryNum << " | ";
	Util::print_now();

	vector<Point> p;
	LShashESE::readDataSet(dataset, p, queryNum);

	cout << "start querying... | ";
	Util::print_now();

	for(u_int i = 0; i < p.size(); ++i) {
		vector<double> sin(p[i].d, p[i].d + DIMS);
		vector<u_int> index;

		lsese.findIndex(sin, index);

		for(u_int i = 0; i < index.size(); ++i) {
			cout << "id: " << index[i] << endl;
		}
	}
	cout << "end... | ";
	Util::print_now();
}

int
main(int argc , char **args) {
	if(argc < 3) {
		perror("usage: ./test dataset.file #query.");
		exit(0);
	}
	int queryNum = atoi(args[2]);
	assert(queryNum >= 0);

	test_lshese(args[1], queryNum);
	return 0;
}
