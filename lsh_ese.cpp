#include "lsh_ese.h"
#include "lshash/util.h"

LShashESE::LShashESE(const char *file):indexFile(file) {
	fhandle = fopen(file, "rb");
	assert(fhandle != NULL);
}

LShashESE::~LShashESE() {
	if(fhandle != NULL)
	fclose(fhandle);
}

void
LShashESE::findIndex(const vector<double> &sin, vector<int> &index) {
	assert(sin.size() == DIMS);
	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	vector<u_int> eid;
	lsh.findNodes(q, eid);

	wavelet.clear();

	Point p;
	for(u_int i = 0; i < eid.size(); ++i) {
		readPoint(eid[i], p);
		wavelet.addSignal();
	}

	wavelet.fin
}

//# Read a point from external index file.
void
LShashESE::readPoint(u_int index, Point &p) {
	int offset = sizeof(u_int) + DIMS * sizeof(double);
	//# todo...
	fseek();
}
