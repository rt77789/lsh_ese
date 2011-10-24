#include <iostream>
#include "util.h"
#include "ghash.h"

using namespace std;

void
test_util() {
	Util::init();
	double x1 = Util::randomByUniform(0, 1LL<<32);
	double x2 = Util::randomByGaussian();
	u64 x3 = Util::randomU64(0, 1LL<<63);
	cout << "x1: " << x1 << endl;
	cout << "x2: " << x2 << endl;
	cout << "x3: " << x3 << endl;
}

void
test_ghash() {
	Util::init();
	Ghash::init(256, 256);

	Point p;

	u_int *uIndex = new u_int[2];
	uIndex[0] = 0;
	uIndex[1] = 1;
	Ghash ghash(uIndex);

	Util::print_now();
	for(int i = 0; i < (100); ++i) {
		Ghash::randomPoint(p);
		p.identity = Util::randomByUniform(0, 1<<20);
		Ghash::preComputeFields(p);
		ghash.addNode(p);
		cout << "insertint " << i << "th point." << "\r";
	}

	double mean = 0;
	double variance = 0;

	for(int i = 0; i < DIMS; ++i) {
		cout << p.d[i] << ' ';	
		mean += p.d[i];
	}

	cout << endl;
	mean /= DIMS;
	cout << mean << endl;
	for(int i = 0; i < DIMS; ++i) {
		variance += (p.d[i] - mean) * (p.d[i] - mean);
	}
	cout << variance / DIMS << endl;
	//# it's actully a normal(Gaussian) distribution.
	Util::print_now();

	for(int i = 0; i < 10; ++i) {
		Ghash::preComputeFields(p);
		Gnode *ptr = ghash.findNode(p);
		//# ptr->identity equals to the privous p.identity = 123.
		cout << ptr->identity << endl;
	}
	Util::print_now();
}

void
test_lshash() {
	LShash lsh;
	Point p;
	for(int i = 0; i < 100; ++i) {
		Ghash::randomPoint(p);
		p.identity = Ghash
		lsh.addNode(p);
	}
	
}

int
main() {

	//	test_util();
	//	test_ghash();
	return 0;
}
