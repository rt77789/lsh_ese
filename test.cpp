#include "lsh_ese.h"
#include "lshash/ghash.h"
#include "lshash/util.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;

const char *tmpOut = "tmp_out";

void
test_build_index(const char *dataset, const char *_if) {
	cout << "lsese constructing... | ";
	Util::print_now();

	LShashESE lsese(dataset);
	lsese.storeLShash(_if);
}

void
test_restore_index(const char *dataset, const char *_if, const char *_query_file, u_int queryNum) {
	cout << "restore from index file... | ";
	Util::print_now();

	LShashESE lsese(dataset, _if);

	cout << "readDataSet-#queryNum=" << queryNum << " | ";
	Util::print_now();

	vector<Point> p;
	LShashESE::readDataSet(_query_file, p, queryNum);

	cout << "start querying... | ";
	Util::print_now();

	ofstream fout(tmpOut);
	assert(fout.is_open());

	for(u_int i = 0; i < p.size(); ++i) {
		vector<double> sin(p[i].d, p[i].d + DIMS);
		vector< vector<double> > resig;

		lsese.findIndex(sin, resig);

		for(u_int i = 0; i < resig.size(); ++i) {
			for(size_t j = 0; j < resig[i].size(); ++j) {
				fout << resig[i][j] << " ";
			}
			fout << endl;
		}
	}
	fout.close();
	cout << "end... | ";
	Util::print_now();
}

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

	ofstream fout(tmpOut);
	assert(fout.is_open());

	for(u_int i = 0; i < p.size(); ++i) {
		vector<double> sin(p[i].d, p[i].d + DIMS);
		vector< vector<double> > resig;

		lsese.findIndex(sin, resig);

		for(u_int i = 0; i < resig.size(); ++i) {
			for(size_t j = 0; j < resig[i].size(); ++j) {
				fout << resig[i][j] << " ";
			}
			fout << endl;
		}
	}
	fout.close();

	cout << "end... | ";
	Util::print_now();
}

void
test_naive_wavelet(const char *dataset, const char *_query_file, u_int queryNum, const char *which) {

	LShashESE lsese(dataset);

	cout << "readDataSet-#queryNum=" << queryNum << " | ";
	Util::print_now();

	vector<Point> p;
	LShashESE::readDataSet(_query_file, p, queryNum);

	cout << "start querying... | ";
	Util::print_now();

	ofstream fout(tmpOut);
	assert(fout.is_open());

	for(u_int i = 0; i < p.size(); ++i) {
		vector<double> sin(p[i].d, p[i].d + DIMS);
		vector< vector<double> > resig;

		if(strcmp(which, "FFT") == 0) {
			lsese.naiveFFTConvFind(sin, resig);
		}
		else {
			lsese.naiveWaveletFind(sin, resig);
		}

		for(u_int i = 0; i < resig.size(); ++i) {
			for(size_t j = 0; j < resig[i].size(); ++j) {
				fout << resig[i][j] << " ";
			}
			fout << endl;
		}
	}
	fout.close();
	cout << "end... | ";
	Util::print_now();
}

int
main(int argc , char **args) {
	if(argc >= 4 && strcmp(args[1], "-build") == 0) {
		test_build_index(args[2], args[3]);
	}
	else if(argc >= 6 && strcmp(args[1], "-load") == 0) {
		int queryNum = atoi(args[5]);
		assert(queryNum >= 0);
		test_restore_index(args[2], args[3], args[4], queryNum);
	}
	else if(argc >= 5 && strcmp(args[1], "-nw") == 0) {
		int queryNum = atoi(args[4]);
		assert(queryNum >= 0);
		test_naive_wavelet(args[2], args[3], queryNum, "nw");
	}
	else if(argc >= 5 && strcmp(args[1], "-nf") == 0) {
		int queryNum = atoi(args[4]);
		assert(queryNum >= 0);
		test_naive_wavelet(args[2], args[3], queryNum, "FFT");
	}
	else {
		perror("usage: \n\
				./test (-build | -load | -nf | -nw) \n\
				\t-build dataset.file index.file\n\
				\t-load dataset.file index.file query.file #query_number\n\
				\t-nf dataset.file query.file #query_number\n\
				\t-nw dataset.file query.file #query_number\n");
		exit(0);
	}
	//test_lshese(args[1], queryNum);
	return 0;
}
