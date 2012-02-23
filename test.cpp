#include "lsh_ese.h"
#include "lshash/ghash.h"

#include "utils/util.h"
#include "utils/config.h"
#include "wavelet/sac_prep.h"

#include "structs/bench.h"

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
	eoaix::print_now();

	LShashESE lsese;
	cout << "begin store index..." << endl;
	lsese.init("lsh");
	cout << "end store index..." << endl;
}

void
test_by_type(const string &type) {
	cout << "restore from index file... | " << std::endl;
	eoaix::print_now();

	LShashESE lsese;
	cout << "before lshese.init()" << std::endl;
	lsese.init(type);

	eoaix::print_now();

	vector<Point> p;

	string testset = Configer::get("project_dir").toString() + Configer::get("testset_path").toString();
	u_int queryNum = Configer::get("testset_query_num").toInt();

	cout << "readDataSet-#queryNum=" << queryNum << " | " << std::endl;
	

	LShashESE::readDataSet(testset.c_str(), p, queryNum);

	cout << "start querying... | ";
	eoaix::print_now();

	double cost = 0;
	double totalRows = Configer::get("rows").toDouble();
	bool doBenchmark = Configer::get("do_benchmark").toBool();
	vector< vector<u_int> > apro(p.size());
	vector< vector<double> > epro(p.size());
	double time = 0;

	for(u_int x = 0; x < p.size(); ++x) {
		vector<double> sin(p[x].d, p[x].d + DIMS);
		
		vector<SearchRes> resig;
		eoaix::Timer t;
		int candi = lsese.findIndex(sin, resig, type);
		cost += candi * 1.0 / totalRows;
		time += t.elapsed();

		for(size_t i = 0; i < resig.size(); ++i) {
			apro[x].push_back(resig[i].getID());
			epro[x].push_back(resig[i].getSim());
		}

		for(u_int i = 0; i < resig.size(); ++i) {
			// pair<int, double> fres = FFT::shift(sin, resig[i].getSignal());
			pair<int, double> fres = FFT::xcorr(sin, resig[i].getSignal());

			//assert((fres.first % sin.size() + sin.size()) % sin.size() == xres.first);

			int offset = fres.first;
			double sim = fres.second;
			
			/**/
			resig[i].setSim(sim);
			/**/
			resig[i].setOffset(offset);
		}

		sort(resig.begin(), resig.end());

		for(size_t i = 0; i*2 < resig.size(); ++i) {
			SearchRes tmp = resig[i];
			resig[i] = resig[resig.size() - 1 - i];
			resig[resig.size() - 1 - i] = tmp;
		}

		{
			int offset = 0;
			cout << "[input]: " << 1 << " | -1 | " << offset << endl;

			vector<double> sig;
			vector<double> &rs = sin;
			sig.assign(rs.begin(), rs.end());
			FFT::normalize(sig);

			int tlen = sig.size();

			cerr << -1 << " ";

			for(size_t j = 0; j < sig.size(); ++j) {
				offset = ((offset % tlen) + tlen ) % tlen;
				cerr << sig[offset++] << " ";
			}
			cerr << endl;

		}
		for(size_t i = 0; i < resig.size(); ++i) {
			int offset = resig[i].getOffset();
			cout << "[" << i << "]: " << resig[i].getSim() << " | " << resig[i].getID() <<  " | " << offset << endl;

			vector<double> sig;
			vector<double> &rs = resig[i].getSignal();
			sig.assign(rs.begin(), rs.end());
			FFT::normalize(sig);

			int tlen = sig.size();

			cerr << resig[i].getID() << " ";

			for(size_t j = 0; j < sig.size(); ++j) {
				offset = ((offset % tlen) + tlen ) % tlen;
				cerr << sig[offset++] << " ";
			}
			cerr << endl;
		}
	}
	if(doBenchmark) {
		Bench bench;
		bench.init();
		double recall = bench.recall(apro);
		double e = bench.recall(epro);
		cout << "recall: " << recall << endl;
		cout << "cost: " << cost / p.size() << endl;
		cout << "E: " << e << endl;
		cout << "time: " << time << endl;
	}
	//fout.close();
	cout << "end... | ";
	eoaix::print_now();
}


void
test_by_type_old(const string &type) {
	cout << "restore from index file... | ";
	eoaix::print_now();

	LShashESE lsese;
	lsese.init(type);

	eoaix::print_now();

	vector<Point> p;

	string testset = Configer::get("project_dir").toString() + Configer::get("testset_path").toString();
	u_int queryNum = Configer::get("testset_query_num").toInt();

	cout << "readDataSet-#queryNum=" << queryNum << " | ";
	

	LShashESE::readDataSet(testset.c_str(), p, queryNum);

	cout << "start querying... | ";
	eoaix::print_now();

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
	eoaix::print_now();
}

void test_demo(const char *path, const string &type) {
	/* sac file to raw file. */
	vector<double> sin;

	PSac ps(path);
	ps.data2vector(sin);

	LShashESE lsese;
	lsese.init(type);
	eoaix::print_now();
	vector<SearchRes> resig;

	ifstream in("sacname.map");
	assert(in.is_open());

	map<int, string> sacname;
	int id;
	string name;
	while(in >> id >> name) {
		sacname[id] = name;
	}

	int cansize = lsese.findIndex(sin, resig, type);
	std::cout << "candidate size: " << cansize << std::endl;

	u_int topk = Configer::get("project_top_k").toInt();

	for(size_t i = 0; i < sin.size(); ++i) {
		cerr << sin[i] << " ";
	}
	cerr << endl;

	for(u_int i = 0; i < resig.size() && i < topk; ++i) {
		assert(sacname.find(resig[i].getID()) != sacname.end());
		string sac = sacname.find(resig[i].getID())->second;

		cout << "[" << i << "]: " <<
				" dis = " << resig[i].getSim() <<
				" | id = " << resig[i].getID() <<
				" | sac = <font color=\"red\">" << sac << "</font>" << 
				std::endl;

		for(size_t j = 0; j < resig[i].getSignal().size(); ++j) {
			cerr << resig[i].getSignal()[j] << " ";

					}
		cerr << endl;
	}
	eoaix::print_now();
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
	else if(argc >= 2 && strcmp(args[1], "-fft") == 0) {
		test_by_type("fft");
	}
	else if(argc >= 2 && strcmp(args[1], "-kdmpl") == 0) {
		test_by_type("kdmpl");
	}
	else if(argc >= 2 && strcmp(args[1], "-kdlsh") == 0) {
		test_by_type("kdlsh");
	}
	else if(argc >= 3 && strcmp(args[1], "-dfft") == 0) {
		/* set testset file to upload file name. */
		test_demo(args[2], "fft");
	}
	else if(argc >= 2 && strcmp(args[1], "-dlsh") == 0) {
		/* set testset file to upload file name. */
		test_demo(args[2], "lsh");
	}
	else if(argc >= 2 && strcmp(args[1], "-dkdtree") == 0) {
		/* set testset file to upload file name. */
		test_demo(args[2], "flann");
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
				\t-kdlsh\n\
				\t-dfft\n\
				\t-dlsh\n\
				\t-dkdtree\n");
		exit(0);
	}
	//test_lshese(args[1], queryNum);
	return 0;
}
