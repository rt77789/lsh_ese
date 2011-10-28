#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <pthread.h>
#include <algorithm>
#include "weps.h"
#include "utils.h"
#include "sac_prep.h"

using namespace std;

vector<double> qsin[4];
vector<WSSimilar> qsout[4];
WaveletEps we[4];


void
deal_data() {

	vector<string> sacs;

	getSacPath("../earth_quake_search/real_data/", sacs);

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
	WSSimilar wss = we[i].find(qsin[i]);

	vector<WSSimilar> &vwss = we[i].getWSSimilar();
	// cout << "vwss.size(): " << vwss.size() << endl;

	for(size_t j = 0; j < vwss.size() && j < K; ++j)
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
		print_now();
		istringstream iss(line);
		vector<double> sin;
		string m;

		while(iss >> m) {
			sin.push_back(sci2double(m));
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

		for(size_t j = 0; j < ans.size() && j < K; ++j) {
			fout << "[" << j << "]: " << endl;
			vector<double> &sig = ans[j].ws.wsig[ans[j].ws.wsig.size()-1].sig;
			for(size_t i = 0; i < sig.size(); ++i) {
				if(i > 0)
					fout << "\t";
				fout << sig[i];
			}
			fout << endl;
		}

		
		print_now();
	}
	fout.close();
	fin.close();
}

int
main(int argc, char **args) {

	if(argc < 2) {
		perror("usage: -pre or -test");
		exit(0);
	}

	if(strcmp(args[1], "-pre") == 0) {
		print_now();
		loadConfig("config");
		deal_data();
	}
	else if(strcmp(args[1], "-test") == 0)	{
		print_now();
		loadConfig("config");
		test();
	}
	else {
		perror("ill option");
		exit(0);
	}
	// 1111 0000 0000 0000

	return 0;
}
