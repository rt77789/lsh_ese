#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <pthread.h>
#include <algorithm>
/* Current directory include files. */
#include "weps.h"
#include "sac_prep.h"
/* Other directory include files. */
#include "../utils/util.h"
#include "../utils/config.h"

using namespace std;

vector<double> qsin[4];
vector<WSSimilar> qsout[4];
WaveletEps we[4];

void read_from_sac(const char *path) {
	PSac ps(path);

	vector<double> sin;
	ps.data2vector(sin);

	Signal sig(sin);
	sig.calPPos();

	int base = sig.ppos;
	sin.clear();
	for(int i = 0; i < DIMS; ++i) {
		if(base >= DIMS) {
			base %= DIMS;
		}
		sin.push_back(sig.sig[base++]);	
	}
	//cout << "[";
	for(size_t i = 0; i < sin.size(); ++i) {
		cout << sin[i];
		if(i + 1 < sin.size()) cout << " ";
	}

	cout << endl;
	//cout << "]\n";
	cerr << "dims: " << sin.size() << endl;
}

void extract_sac_sample(const char *path, const char *opath) {

	vector<string> sacs;
	eoaix::getSacPath(path, sacs);

	// Load Sac file one by one.

	FILE *fout = fopen(opath, "wb");
	assert(fout != NULL);

	for(size_t i = 0; i < sacs.size(); ++i) {
		PSac ps(sacs[i].c_str());

		vector<double> sin;
		ps.data2vector(sin);
		string fn = sacs[i].substr(sacs[i].find_last_of("/") + 1);
		cerr << i << " " << fn << endl;
		printf("total: 1350000 - now: %d\r", i);

		size_t jj = 0;
		/*
		for(; jj < 250; ++jj) {
			if(jj > 0) fprintf(fout, " "); 
			fprintf(fout, "%lf", 0.0);
		}
		*/

		for(size_t j = 0; j < sin.size() && jj < 4096; ++j, ++jj) {
			if(jj > 0) fprintf(fout, " "); 
			fprintf(fout, "%.20lf", sin[j]);
		}
		//for(;jj < 4096; ++jj) 
	//		fprintf(fout, " %lf", 0.0);

		fprintf(fout, "\n");
	}
	fclose(fout);
}
void extract_sac_sample(const string& path) {

	vector<string> sacs;
	sacs.push_back(path);

	// Load Sac file one by one.

	for(size_t i = 0; i < sacs.size(); ++i) {
		PSac ps(sacs[i].c_str());

		vector<double> sin;
		ps.data2vector(sin);
		string fn = sacs[i].substr(sacs[i].find_last_of("/") + 1);
		//cerr << i << " " << fn << endl;

		/* normalize. */
		double max = 0;
		for(size_t j = 0; j < sin.size(); ++j) {
			if(fabs(sin[j]) > max) {
				max = fabs(sin[j]);
			}
		}

		size_t jj = 0;
		for(size_t j = 0; j < sin.size(); ++j, ++jj) {
			if(jj > 0) printf(" "); 
			printf("%.20lf", sin[j] / max);
		}
		printf("\n");
	}
}



void
deal_data() {

	vector<string> sacs;

	eoaix::getSacPath("../earth_quake_search/real_data/", sacs);

	// Load Sac file one by one.

	WaveletEps we;
	ofstream fout("filt3.wavelet.3");

	for(size_t i = 0; i < sacs.size(); ++i) {
		PSac ps(sacs[i].c_str());
		fout << sacs[i] << endl;

		vector<double> sin;
		ps.data2vector(sin);

		WaveletSignal ws;

		{
			Signal ts;
			ts.sig.assign(sin.begin(), sin.end());
			// Calculate the ppos of Signal.
			ts.calPPos();
			sin.clear();
			int len = ts.sig.size();

			for(int i = 0; i < DIMS; ++i) {
				// 2 seconds before first P-wave arrival.
				int index = ts.ppos + i - 2 * FREQUENT;
				index = ((index % len) + len) % len;
				sin.push_back(ts.sig[index]);
			}
			// 
			//for(size_t i = sin.size(); i < DIMS; ++i) {
			//	sin.push_back(0);
			//}
		}

		cout << "dims: " << sin.size() << endl;
		assert(sin.size() == DIMS);

		for(int i = 0; i < LEVEL; ++i) {
			Signal ts;
			ts.sig.assign(sin.begin(), sin.begin() + ((int)sin.size() >> i));
			// Calculate the ppos of Signal.
			ts.calPPos();
			cout << "level: " << i << " ppos: " << ts.ppos << endl;
			ws.wsig.push_back(ts);
			we.waveletTransform(sin);
		}
		// Write ws file.
		for(int i = ws.wsig.size()-1; i >= 0; --i) {
			fout << ws.wsig[i].ppos;
			for(size_t j = 0; j < ws.wsig[i].sig.size(); ++j) {
				fout << "\t" << ws.wsig[i].sig[j];
			}
			fout << endl;
		}
	}
}
void*
find_similar_signal(void *ptr) {
	long i = (long)ptr;
	// cout << "i - " << i << endl;

	vector<WSSimilar> &vwss = we[i].find(qsin[i]);
	// cout << "vwss.size(): " << vwss.size() << endl;
	size_t top_k = Configer::get("project_top_k").toInt();

	for(size_t j = 0; j < vwss.size() && j < top_k; ++j)
		qsout[i].push_back(vwss[j]);
	// cout << "qsout: "<< i << " - " << qsout[i].size() << endl;
	return (void*)0;
}


void
test() {

	char f3[128] = "filt3.wavelet.3.";
	int f3len = strlen(f3);
	f3[f3len+1] = 0;
	for(int i = 0; i < 4; ++i) {
		f3[f3len] = '0' + i;
		cout << "--" << f3 << "--" << endl;
		we[i] = WaveletEps(f3);
	}
	ifstream fin("simulate_data");
	ofstream fout("fout");

	string line;

	while(getline(fin, line)) {
		eoaix::print_now();
		istringstream iss(line);
		vector<double> sin;
		string m;

		while(iss >> m) {
			sin.push_back(eoaix::sci2double(m));
		}
		cout << "sin.size(): " << sin.size() << endl;
		for(int i = 0; i < 4; ++i) {
			qsin[i].assign(sin.begin(), sin.end());
		}

		pthread_t threads[4];
		int iret[4];

		const int THREAD_NUM = 4;
		for(int i = 0; i < THREAD_NUM; ++i)
			iret[i] = pthread_create( &threads[i], NULL, &find_similar_signal, (void*) i);
		for(int i = 0; i < THREAD_NUM; ++i)
			pthread_join(threads[i], NULL);
		// getchar();

		// merge all thread results.
		
		vector<WSSimilar> ans;
		for(int i = 0; i < THREAD_NUM; ++i) {
			// ans.insert(ans.begin(), qsout[i].begin(), qsout[i].end());	
			// cout << "qsout[" << i << " ].size(): " << qsout[i].size() << endl;
			for(size_t j = 0; j < qsout[i].size(); ++j)
				ans.push_back(qsout[i][j]);
		}
		sort(ans.begin(), ans.end());

		//		cout << "sim: " << wss.sim << endl;

		fout << "res: ";

		size_t top_k = Configer::get("project_top_k").toInt();
		for(size_t j = 0; j < ans.size() && j < top_k; ++j) {
			fout << "[" << j << "]: " << endl;
			vector<double> &sig = ans[j].ws.wsig[ans[j].ws.wsig.size()-1].sig;
			for(size_t i = 0; i < sig.size(); ++i) {
				if(i > 0)
					fout << "\t";
				fout << sig[i];
			}
			fout << endl;
		}

		
		eoaix::print_now();
	}
	fout.close();
	fin.close();
}

int
main(int argc, char **args) {

	if(argc < 2) {
		perror("usage: -pre\n\
				-test\n\
				-trans sacfile.sac\n\
				-sample\n");
		exit(0);
	}

	if(strcmp(args[1], "-pre") == 0) {
		eoaix::print_now();
		//loadConfig("config");
		deal_data();
	}
	else if(strcmp(args[1], "-test") == 0)	{
		eoaix::print_now();
		//loadConfig("config");
		test();
	}
	else if(argc >= 3 && strcmp(args[1], "-trans") == 0) {
		read_from_sac(args[2]);
	}
	else if(argc >= 4 && strcmp(args[1], "-sample") == 0) {
		extract_sac_sample(args[2], args[3]);
	}
	else if(argc >= 3 && strcmp(args[1], "-sample2") == 0) {
		extract_sac_sample(args[2]);
	}
	else {
		perror("ill option");
		exit(0);
	}
	// 1111 0000 0000 0000

	return 0;
}
