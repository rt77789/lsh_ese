#include <iostream>
#include <fstream>
#include <cassert>
#include "../utils/util.h"
#include "ghash.h"
#include "lshash.h"


using namespace std;
using namespace eoaix;
void
test_util() {
	double x1 = Util::randomByUniform(0, 1LL<<32);
	double x2 = Util::randomByGaussian();
	u64 x3 = Util::randomU64(0, 1LL<<63);
	cout << "x1: " << x1 << endl;
	cout << "x2: " << x2 << endl;
	cout << "x3: " << x3 << endl;
}

void
test_ghash() {
	/* M, K, W, R. */
	Ghash::init(256, 256, 4, 100);

	Point p;

	u_int *uIndex = new u_int[2];
	uIndex[0] = 0;
	uIndex[1] = 1;
	Ghash ghash(uIndex);

	print_now();
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
	print_now();

	for(int i = 0; i < 10; ++i) {
		Ghash::preComputeFields(p);
		Gnode *ptr = ghash.findNode(p);
		//# ptr->identity equals to the privous p.identity = 123.
		cout << ptr->identity << endl;
	}
	print_now();
}

void
test_lshash() {
	LShash lsh;
	Point p;
	print_now();

	u_int datasetSize = 100000;

	ofstream fout("dataset.rand");
	assert(fout.is_open());

	for(u_int i = 0; i < datasetSize; ++i) {
		Ghash::randomPoint(p);
		p.identity = i;
		lsh.addNode(p);
		fout << "[" << i << "]: ";
		for(u_int i = 0; i < DIMS; ++i) {
			fout << p.d[i] << ' ';
		}
		fout << endl;
	}

	print_now();

	ofstream ftest("query.rand");
	assert(ftest.is_open());

	//Ghash::randomPoint(p);	

	for(u_int i = 0; i < DIMS; ++i) {
		ftest << p.d[i] << ' ';
	}
	ftest << endl;

	vector<u_int> eid;
	lsh.find(p, eid);

	for(u_int i = 0; i < eid.size(); ++i) {
		ftest << eid[i] << ' ' ;
	}
	ftest << endl;
	ftest << "num: " << eid.size() << " - " << " total: " << datasetSize << " | " << (eid.size() * 1.0 / datasetSize) << endl;
	print_now();

	int maxlen = lsh.getMaxBuckLen();
	cout << "maxlen: " << maxlen << endl;

	ftest.close();
	fout.close();
}

int
main() {

	//	test_util();
	//	test_ghash();
	test_lshash();

	return 0;
}
