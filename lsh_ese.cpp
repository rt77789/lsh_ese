#include "lsh_ese.h"
#include "lshash/util.h"
#include "lshash/point.h"
#include "lshash/ghash.h"

LShashESE::LShashESE(const char *file):indexFile(file) {
	fhandle = fopen(file, "rb");
	assert(fhandle != NULL);
	loadPoint();
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
	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	vector<u_int> eid;
	lsh.findNodes(q, eid);

	cout << "lsh.findNodes returns: eid.size() == " << eid.size() << endl;

	wavelet.clear();

	Point p;
	for(u_int i = 0; i < eid.size(); ++i) {
		assert(true == readPoint(eid[i], p));

		vector<double> tin(p.d, p.d + DIMS);
		wavelet.addSignal(tin, p.identity);

	}

	WSSimilar wss = wavelet.find(sin);
	vector<WSSimilar> &vwss = wavelet.getWSSimilar();

	for(u_int i = 0; i < vwss.size() && i < K; ++i) {
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
