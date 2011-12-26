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
#ifdef L2NORM
		std::cout << "L2 norm distance computing..." << std::endl;
#else
#ifdef T0XCORR
		cout << "T0 cross-correlation computing..." << endl;
#else
		std::cout << "cross correlation computing..." << std::endl;
#endif
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

int
main(int argc , char **args) {
	Configer::init("all.config");

	if(argc >= 2 && strcmp(args[1], "-build") == 0) {
		test_build_index();
	}
	else if(argc >= 2  && strcmp(args[1], "-lsh") == 0) {
		try {
		test_by_type("lsh");
		} catch(exception e) {
			std::cout << "exception: " << e.what() << std::endl;
		}
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
	else if(argc >= 2 && strcmp(args[1], "-kdmpl") == 0) {
		test_by_type("kdmpl");
			}
	else if(argc >= 2 && strcmp(args[1], "-kdlsh") == 0) {
		test_by_type("kdlsh");
	}
	else {
		perror("usage: \n\
				./test (-build | -lsh | -mpl | -flann | -wavelet | -fft) \n\
				\t-build dataset.file index.file\n\
				\t-lsh\n\
				\t-mpl\n\
				\t-flann\n\
				\t-fft\n\
				\t-wavelet\n\
				\t-kdmpl\n\
				\t-kdlsh\n");
		exit(0);
	}
	//test_lshese(args[1], queryNum);
	return 0;
}
