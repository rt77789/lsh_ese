#include "lsh_ese.h"
#include "lshash/util.h"
#include "lshash/point.h"
#include "lshash/ghash.h"
#include <sstream>
#include <fstream>
#include <algorithm>

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
		lsh.addNode(p);
	}
}

void
LShashESE::findIndex(const vector<double> &sin, vector<u_int> &_index) {
	assert(sin.size() == DIMS);

	vector<WSSimilar> res;

	vector<double> nsin;
	Point q;
	u_int teids = 0;
	set<int> seid;

	for(int h = 0; h < DIMS; ++h) {
		nsin.clear();
		for(u_int i = 0; i < sin.size(); ++i) {
			q.d[i] = sin[(i + h) % DIMS];
			nsin.push_back(q.d[i]);
		}

		vector<u_int> eid;

		try {
			//# Here always throw segment fault.
			lsh.findNodes(q, eid);
		} catch(exception) {
			cerr << "just fuck it...\n";
			throw;
		}

		cout << "offset: " << h << " | lsh.findNodes returns: eid.size() == " << eid.size() << endl;
		teids += eid.size();
		for(u_int i = 0; i < eid.size(); ++i)
			seid.insert(eid[i]);
	}

	wavelet.clear();

	Point p;
	for(set<int>::iterator iter = seid.begin(); iter != seid.end(); ++iter) {
		assert(true == readPoint(*iter, p));
		vector<double> tin(p.d, p.d + DIMS);
		wavelet.addSignal(tin, p.identity);
	}

	vector<WSSimilar> &vwss = wavelet.find(nsin);

	cout << "tedis: " << teids << " | seid: " << seid.size() << endl;

	for(u_int i = 0; i < vwss.size() && _index.size() < K; ++i) {
		cout << "[" << i << "]: " << vwss[i].sim << " - index: " << vwss[i].index << endl;
		_index.push_back(vwss[i].index);
	}
}

void
LShashESE::naiveWaveletFind(const vector<double> &sin, vector<u_int> &_index) {
	assert(sin.size() == DIMS);
	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	vector<u_int> eid;
	//lsh.findNodes(q, eid);

	//cout << "lsh.findNodes returns: eid.size() == " << eid.size() << endl;

	wavelet.clear();

	Point p;
	assert(0 == fseek(fhandle, 0LL, SEEK_SET));

	int tnum = 0;
	while(fread(&p, sizeof(Point), 1, fhandle) == 1) {
		vector<double> tin(p.d, p.d + DIMS);
		//cout << "p.identity: " << p.identity << endl;
		wavelet.addSignal(tin, p.identity);
		++tnum;
	}

	cout << "total signals : " << tnum << endl;

	vector<WSSimilar> &vwss = wavelet.find(sin);

	for(u_int i = 0; i < vwss.size() && i < K; ++i) {
		cout << "[" << i << "]: " << vwss[i].sim << " - index: " << vwss[i].index << endl;
		_index.push_back(vwss[i].index);
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
		while(iss >> d) {
			p.d[j++] = d;	
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


//#
int
LShashESE::getMaxBuckLen() {
	return lsh.getMaxBuckLen();
}

//#
int
LShashESE::getAveBuckLen() {
	return lsh.getAveBuckLen();
}
