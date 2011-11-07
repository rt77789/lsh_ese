#include "lsh_ese.h"
#include "lshash/util.h"
#include "lshash/point.h"
#include "lshash/ghash.h"
#include "fft/fft.h"
#include <sstream>
#include <fstream>
#include <algorithm>

bool comPair (pair<double, vector<double> > i, pair<double, vector<double> > j) { return (i.first > j.first); }

LShashESE::LShashESE(const char *file):indexFile(file) {
	fhandle = fopen(file, "rb");
	assert(fhandle != NULL);
	loadPoint();
}

LShashESE::LShashESE(const char *file, const char *_if):indexFile(file) {
	fhandle = fopen(file, "rb");
	assert(fhandle != NULL);
	restoreLShash(_if);
}


LShashESE::~LShashESE() {
	if(fhandle != NULL)
		fclose(fhandle);
}

//# Load point from external file and add into lsh object.
void
LShashESE::loadPoint() {
	assert(fhandle != NULL);
	Point p;
	for(u_int i = 0; readPoint(i, p) ; ++i) {
		//# sort p.d[].
		lsh.addNode(p);
	}
}

void
LShashESE::findByLSH(const vector<double> &sin, vector<u_int> &_index) {
	assert(sin.size() == DIMS);

	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	lsh.findNodes(q, _index);
}

void
LShashESE::findIndex(const vector<double> &sin, vector< vector<double> > &resig) {
	
	//# Find by LSHash.
	vector<u_int> eid;
	findByLSH(sin, eid);

	cout << "lsh.findNodes returns: eid.size() == " << eid.size() << endl;

	Point p;
	vector<pair<double, u_int> > xlist;

	//# sort eid, decreate disk move.
	sort(eid.begin(), eid.end());

	WaveletEps twe(sin);
	vector< vector<double> > vtin;
	vector<int> iden;

	for(u_int i = 0; i < eid.size(); ++i) {
		assert(true == readPoint(eid[i], p));
		vector<double> tin(p.d, p.d + DIMS);

		vtin.push_back(tin);
		iden.push_back(p.identity);

		if(vtin.size() >= IN_MEMORY_NUM) {
			try {
				twe.batch_push(vtin, iden);
			}catch(...) {
				cerr << "batch_push exception" << endl;
				throw;
			}
			vtin.clear();
			iden.clear();
		}
	}

	if(vtin.size() > 0) {
		twe.batch_push(vtin, iden);
	}

	vector<WSSimilar> &vwss = twe.find(sin);

	for(u_int i = 0; i < vwss.size() && i < K; ++i) {
		cout << "[" << i << "]: " << vwss[i].sim << " - index: " << vwss[i].index << endl;
		resig.push_back(vwss[i].ws.wsig[vwss[i].ws.wsig.size()-1].sig);
	}
}

void
LShashESE::naiveFFTConvFind(const vector<double> &sin, vector< vector<double> > &resig) {
	assert(sin.size() == DIMS);

	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	vector<u_int> eid;
	//cout << "lsh.findNodes returns: eid.size() == " << eid.size() << endl;

	assert(0 == fseek(fhandle, 0LL, SEEK_SET));
	vector<pair<double, vector<double> > > xlist, rlist;

	int tnum = 0;

	Point p[BATCH_READ_NUM];

	int cpnum = 0;	

	while((cpnum = fread(p, sizeof(Point), BATCH_READ_NUM, fhandle)) > 0) {
		for(int i = 0; i < cpnum; ++i) {
			vector<double> tin(p[i].d, p[i].d + DIMS);
			//cout << "p.identity: " << p.identity << endl;
			double sim = FFT::xcorr(sin, tin);
			xlist.push_back(make_pair<double, vector<double> >(sim, tin));
			if(xlist.size() >= IN_MEMORY_NUM) {
				//# merge
				rlist.insert(rlist.end(), xlist.begin(), xlist.end());
				sort(rlist.begin(), rlist.end(), comPair);
				if(rlist.size() > IN_MEMORY_NUM) {
					rlist.resize(IN_MEMORY_NUM);
				}
				xlist.clear();
			}
		}
		tnum += cpnum;
	}

	cout << "FFT total signals : " << tnum << endl;


	for(u_int i = 0; i < rlist.size() && i < K; ++i) {
		cout << "[" << i << "]: " << rlist[i].first << endl;
		resig.push_back(rlist[i].second);
	}
}

void
LShashESE::naiveWaveletFind(const vector<double> &sin, vector< vector<double> > &resig) {
	assert(sin.size() == DIMS);
	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	WaveletEps twe(sin);

	assert(0 == fseek(fhandle, 0LL, SEEK_SET));

	int tnum = 0;

	vector< vector<double> > vtin;
	vector<int> iden;
	Point p[BATCH_READ_NUM];
	int cpnum = 0;

	while((cpnum = fread(&p, sizeof(Point), 1, fhandle)) > 0) {
		for(int i = 0; i < cpnum; ++i) {
			vector<double> tin(p[i].d, p[i].d + DIMS);
			//# Can't load all dataset in memory.
			vtin.push_back(tin);
			iden.push_back(p[i].identity);
			if(vtin.size() >= IN_MEMORY_NUM) {
				twe.batch_push(vtin, iden);
				vtin.clear();
				iden.clear();
			}
		}
		tnum += cpnum;
	}
	if(vtin.size() > 0) {
		twe.batch_push(vtin, iden);
	}

	cout << "Wavelet FFT total signals : " << tnum << endl;

	vector<WSSimilar> &vwss = twe.find(sin);

	for(u_int i = 0; i < vwss.size() && i < K; ++i) {
		cout << "[" << i << "]: " << vwss[i].sim << " - index: " << vwss[i].index << endl;
		resig.push_back(vwss[i].ws.wsig[vwss[i].ws.wsig.size()-1].sig);
	}
}


//# Read a point from external index file and insert into waveletEps obejct.
bool
LShashESE::readPoint(u_int index, Point &p) {
	//int offset = sizeof(u_int) + DIMS * sizeof(double);
	u64 offset = (u64)sizeof(Point) * index;

	assert(0 == fseek(fhandle, offset, SEEK_SET));

	int rv = fread(&p, sizeof(Point), 1, fhandle);
	//assert(rv == 1);
	return rv == 1;
}


//# Transform dataset into binary format.
void
LShashESE::transformDataSet(const char *_fin, const char *_fout) {
	ifstream in(_fin);
	assert(in.is_open());
	FILE *fh = fopen(_fout, "wb");
	assert(fh != NULL);

	string line;
	int i = 0;

	while(getline(in, line)) {
		istringstream iss(line);
		double d;
		Point p;
		p.identity = i++;
		u_int j = 0;
		string sd;
		while(iss >> sd) {
			try {
				p.d[j++] = sci2double(sd);	
			}catch(...) {
				cerr << "sci2double exception." << endl;
				throw;
			}
		}
		assert(j == DIMS);

		assert(fwrite(&p, sizeof(Point), 1, fh) == 1);
	}

	fclose(fh);
	in.close();
}

//#
void
LShashESE::iTransformDataSet(const char *_fin, const char *_fout) {
	FILE *fh = fopen(_fin, "rb");
	assert(fh != NULL);

	ofstream out(_fout);
	assert(out.is_open());

	Point p;
	while(fread(&p, sizeof(Point), 1, fh) == 1) {
		for(int i = 0; i < DIMS; ++i) {
			out << p.d[i];		
			if(i + 1 != DIMS)
				out << " ";
		}
		out << endl;
	}

	out.close();
	fclose(fh);
}


//# Random a dataset of size = _size, and write into the 'file'.
void
LShashESE::randomDataSet(const char *file, u_int _size) {
	FILE *fh = fopen(file, "wb");

	assert(fh != NULL);
	for(u_int i = 0; i < _size; ++i) {
		Point p;
		p.identity = i;
		Ghash::randomPoint(p);

		assert(fwrite(&p, sizeof(Point), 1, fh) == 1);
	}
	fclose(fh);
}

//#
void
LShashESE::readDataSet(const char *file) {
	FILE *fh = fopen(file, "rb");
	assert(fh != NULL);
	Point p;
	int num = 0;
	while(fread(&p, sizeof(Point), 1, fh) == 1) {
		++num;
		cout << "[" << p.identity << "]: ";
		for(int i = 0; i < DIMS; ++i) {
			cout << p.d[i] << ' ';
		}
		cout << endl;
	}
	cout << num << endl;
	fclose(fh);
}

//#
void
LShashESE::readDataSet(const char *file, vector<Point> &p, u_int _size) {
	FILE *fh = fopen(file, "rb");
	assert(fh != NULL);
	Point tp;
	u_int num = 0;
	while(fread(&tp, sizeof(Point), 1, fh) == 1 && num < _size) {
		++num;
		p.push_back(tp);
	}
	fclose(fh);
}

void
LShashESE::storeLShash(const char *_if) {
	lsh.storeGhash(_if);
}

void
LShashESE::restoreLShash(const char *_if) {
	lsh.restoreGhash(_if);
}
