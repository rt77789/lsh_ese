#include "lsh_ese.h"
#include "lshash/ghash.h"

#include "utils/util.h"
#include "utils/config.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;

const char *tmpOut = "tmp_out";

/* Test benchmark, return the match number. */
int
test_bench(const vector<SearchRes> &bench, const vector<SearchRes> &diff) {
	int mn = 0;
	for(u_int i = 0; i < bench.size(); ++i) {
		for(u_int j = 0; j < diff.size(); ++j) {
			if(bench[i].getID() == diff[j].getID()) {
				++mn;
				break;
			}
		}
	}
	return mn;
}

void
test_build_index() {
	cout << "lsese constructing... | ";
	print_now();

	LShashESE lsese;
	cout << "begin store index..." << endl;
	lsese.init("lsh");
	cout << "end store index..." << endl;
}

void
test_by_type(const string &type) {
	cout << "restore from index file... | ";
	print_now();

	LShashESE lsese;
	lsese.init(type);

	print_now();

	vector<Point> p;

	string testset = Configer::get("testset_path").toString();
	u_int queryNum = Configer::get("testset_query_num").toInt();

	cout << "readDataSet-#queryNum=" << queryNum << " | ";
	

	LShashESE::readDataSet(testset.c_str(), p, queryNum);

	cout << "start querying... | ";
	print_now();

	//ofstream fout(tmpOut);
	//assert(fout.is_open());
	double recall = 0;
	double cost = 0;
	bool doBenchmark = Configer::get("do_benchmark").toBool();

	for(u_int i = 0; i < p.size(); ++i) {
		vector<double> sin(p[i].d, p[i].d + DIMS);
		vector<SearchRes> resig, bench;

#ifdef T0XCORR
		cout << "T0 cross-correlation computing..." << endl;
#endif
		int candi = lsese.findIndex(sin, resig, type);

		if(doBenchmark) {
			int total = lsese.naiveFFTConvFind(sin, bench);
			cost += 1.0 * candi / total;
			recall += test_bench(bench, resig) * 1.0 / bench.size();
		}

		for(u_int i = 0; i < resig.size(); ++i) {
			for(size_t j = 0; j < resig[i].getSignal().size(); ++j) {
				//fout << resig[i][j] << " ";
				cerr << resig[i].getSignal()[j] << " ";
			}
			//fout << endl;
			cerr << endl;
		}
	}
	if(doBenchmark) {
		cout << "recall: " << recall / p.size() << endl;
		cout << "cost: " << cost / p.size() << endl;
	}
	//fout.close();
	cout << "end... | ";
	print_now();
}
/*
void
test_mplsh(const char *dataset, const char *_if, const char *_query_file, u_int queryNum) {
	cout << "mplsh test ... | ";
	print_now();

	LShashESE lsese();
	lsese.init("mpl");

	cout << "readDataSet-#queryNum=" << queryNum << " | ";
	print_now();

	vector<Point> p;
	LShashESE::readDataSet(_query_file, p, queryNum);

	cout << "mplsh start querying... | ";
	print_now();

	//ofstream fout(tmpOut);
	//assert(fout.is_open());

	double recall = 0;
	double cost = 0;
	bool doBenchmark = Configer::get("do_benchmark").toBool();
	for(u_int i = 0; i < p.size(); ++i) {
		vector<double> sin(p[i].d, p[i].d + DIMS);
		vector<SearchRes> resig, bench;
#ifdef T0XCORR
		cout << "T0 cross-correlation computing..." << endl;
#endif
		int candi = lsese.findIndex(sin, resig, "mpl");
		if(doBenchmark) {
			int total = lsese.naiveFFTConvFind(sin, bench);
			cost += 1.0 * candi / total;
			recall += test_bench(bench, resig) * 1.0 / bench.size();
		}

		for(u_int i = 0; i < resig.size(); ++i) {
			for(size_t j = 0; j < resig[i].getSignal().size(); ++j) {
				//fout << resig[i][j] << " ";
				cerr << resig[i].getSignal()[j] << " ";
			}
			//fout << endl;
			cerr << endl;
		}
	}
	if(doBenchmark) {
		cout << "recall: " << recall / p.size() << endl;
		cout << "cost: " << cost / p.size() << endl;
	}
	//fout.close();
	cout << "end... | ";
	print_now();
}
*/
/*
void test_naive_wavelet(const string &type) {

	LShashESE lsese;
	lsese.init("naive");

	cout << "readDataSet-#queryNum=" << queryNum << " | ";
	print_now();

	vector<Point> p;
	string dataset = Configer::get("naive_dataset_path").toString();
	u_int queryNum= Configer::get("query_num").toInt();
	LShashESE::readDataSet(dataset.c_str(), p, queryNum);

	cout << "start querying... | ";
	print_now();

	//ofstream fout(tmpOut);
	//assert(fout.is_open());
	double recall = 0;
	double cost = 0;
	bool doBenchmark = Configer::get("do_benchmark").toBool();

	for(u_int i = 0; i < p.size(); ++i) {
		vector<double> sin(p[i].d, p[i].d + DIMS);
		vector<SearchRes> resig, bench;
#ifdef T0XCORR
		cout << "T0 cross-correlation computing..." << endl;
#endif
		//resig.push_back(sin);

		int candi = 0;

		if(strcmp(type, "fft") == 0) {
			candi = lsese.naiveFFTConvFind(sin, resig);
		}
		else {
			candi = lsese.naiveWaveletFind(sin, resig);
		}

		if(doBenchmark) {
			int total = lsese.naiveFFTConvFind(sin, bench);
			cost += 1.0 * candi / total;
			recall += test_bench(bench, resig) * 1.0 / bench.size();
		}

		for(u_int i = 0; i < resig.size(); ++i) {
			for(size_t j = 0; j < resig[i].getSignal().size(); ++j) {
				//fout << resig[i][j] << " ";
				cerr << resig[i].getSignal()[j] << " ";
			}
			//fout << endl;
			cerr << endl;
		}
	}
	if(doBenchmark) {
		cout << "recall: " << recall / p.size() << endl;
		cout << "cost: " << cost / p.size() << endl;
	}
	//fout.close();
	cout << "end... | ";
	print_now();
}
*/

int
main(int argc , char **args) {
	Configer::init("all.config");

	if(argc >= 2 && strcmp(args[1], "-build") == 0) {
		test_build_index();
	}
	else if(argc >= 2  && strcmp(args[1], "-lsh") == 0) {
		test_by_type("lsh");
	}
	else if(argc >= 2 && strcmp(args[1], "-flann") == 0) {
		test_by_type("flann");
	}
	else if(argc >= 2 && strcmp(args[1], "-mpl") == 0) {
		test_by_type("mpl");
	}
	else if(argc >= 2 && strcmp(args[1], "-wavelet") == 0) {
		test_by_type("wavelet");
	}
	else if(argc >= 2 && strcmp(args[1], "-fft") == 0) {
		test_by_type("fft");
	}
	else {
		perror("usage: \n\
				./test (-build | -lsh | -mpl | -flann | -wavelet | -fft) \n\
				\t-build dataset.file index.file\n\
				\t-lsh\n\
				\t-mpl\n\
				\t-flann\n\
				\t-fft\n\
				\t-wavelet\n");
		exit(0);
	}
	//test_lshese(args[1], queryNum);
	return 0;
}
