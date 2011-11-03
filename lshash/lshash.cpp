#include "lshash.h"
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
using namespace std;

struct LGI {
	double sim;
	int gid;
	int goff;
	LGI(double _sim, int _gid, int _goff):sim(_sim), gid(_gid), goff(_goff) {}
	bool operator<(const LGI &l) const {
		return sim > l.sim;
	}
};

LShash::LShash() {
	K = 10;
	prob = 0.99;
	M = estimateParaM(K, prob);
	cout << "M: " << M << endl;

	//int L = M*(M-1) / 2;

	Util::init();
	Ghash::init(M, K);
	
	for(int i = 0; i < M; ++i) {
		for(int j = 0; j < M; ++j) {
			if(i == j) continue;
			u_int *uIndex = new u_int[2];
			uIndex[0] = i, uIndex[1] = j;
			g.push_back(Ghash(uIndex));
		}
	}
}

LShash::~LShash() {
}

void
LShash::tuneParameter() {

}

int
LShash::estimateParaM(int k, double prob) {
	//# k should be a even number.
	assert((k & 1) == 0);
	double w = 4, c = 1;
	double x = w / c;
	double s = 1 - erfc(x / M_SQRT2) - M_2_SQRTPI / M_SQRT2 / x * (1 - exp((-x*x) / 2));
	double mu = 1 - pow(s, k / 2);
	double p = prob;
	double d = (1-mu)/(1-p)*1.0/log(1/mu) * pow(mu, -1/(1-mu));
	double y = log(d);
	int m = (int)(1 - y / log(mu) - 1 / (1-mu) + 0.5);
	while(pow(mu, m-1) * (1 + m*(1-mu)) > 1 - p)
		++m;
	return m;
}

int
LShash::getMaxBuckLen() {
	int res = 0;
	for(u_int i = 0; i < g.size(); ++i) {
		int tmp = g[i].getMaxLen();
		res = res > tmp ? res : tmp;
	}
	return res;
}

void
LShash::findMeanPoints(const Point &q, vector<u_int> &eid) {
	vector<LGI> lgi;

	for(u_int i = 0; i < g.size(); ++i) {
		Point **plist = g[i].getMeanPoints();
		for(int j = 0; j < TABLE_PRIME; ++j) {
			if(plist[j] != NULL) {
				//# xcorrelation plist[i] and q;
				double sim = xCorrelation(&q, plist[j]);
			//	cout << "sim : " << sim << endl;
				lgi.push_back(LGI(sim, i, j));		
			}
		}
	}
	sort(lgi.begin(), lgi.end());

	set<u_int> siden;
	cout << "lgi.size(): " << lgi.size() << endl;
	for(u_int i = 0; i < lgi.size() && i < TOP_BUCKET_NUM; ++i) {
		g[lgi[i].gid].fillFromChain(lgi[i].goff, siden);
	}
	for(set<u_int>::iterator iter = siden.begin(); iter != siden.end(); ++iter) {
		eid.push_back(*iter);
	}
}

double
LShash::xCorrelation(const Point *sa, const Point *sb) {
	double ma = 0, mb = 0;

	// cout << sa.sig.size() << endl << sb.sig.size() << endl;

	int len = DIMS;

	for(int i = 0; i < len; ++i) {
		ma += sa->d[i];
		mb += sb->d[i];
	}

	ma /= len;
	mb /= len;

	double res = -INF;
	int offset = -INF;

	double deta = 0;
	double detb = 0;
	for(int i = 0; i < len; ++i) {
		deta += (sa->d[i] - ma) * (sa->d[i] - ma);
		detb += (sb->d[i] - mb) * (sb->d[i] - mb);
	}

	deta = sqrt(deta * detb);
	//# Make sure, it's fair for all resolutions.
	int eps = len;
	for(int dp = -eps; dp < eps; ++dp) {
		// cout << "test" << endl;
		double num = 0;
		for(int i = 0; i < len; ++i) {
			num += (sa->d[i] - ma) * (sb->d[((i + dp) % len + len) %len] - mb);
		}

		double rd = num / deta;
		if(rd > res) {
			res = rd;
			offset = dp;
		}
	}
	// cout << "res: " << res << endl;
	return res;
	//return make_pair<double, int>(res, offset);

}

void
LShash::findNodes(const Point &q, vector<u_int> &eid) {
	Point p = q;
	//# p.d = q.d[] / R
	Ghash::preComputeFields(p);
	set<u_int> idSet;

	for(u_int i = 0; i < g.size(); ++i) {
		vector<u_int> tid;
		g[i].findNodes(p, tid);
		for(u_int j = 0; j < tid.size(); ++j) {
			if(idSet.find(tid[j]) == idSet.end()) {
				idSet.insert(tid[j]);
			}
			// cout << "identity: " << ptr->identity << endl;
		}
	}

	for(set<u_int>::iterator iter = idSet.begin(); iter != idSet.end(); ++iter) {
		eid.push_back(*iter);
	}
}

void
LShash::addNode(const Point &q) {
	Point p = q;
	//# p.d = q.d[] / R
	Ghash::preComputeFields(p);
	for(u_int i = 0; i < g.size(); ++i)
		g[i].addNode(p);
}

void
LShash::storeGhash(const char *_file) {
	FILE *fh = fopen(_file, "wb");
	assert(fh != NULL);
	
	//# 
	assert(1 == fwrite(&K, sizeof(u_int), 1, fh));
	assert(1 == fwrite(&M, sizeof(int), 1, fh));
	assert(1 == fwrite(&prob, sizeof(double), 1, fh));

	Ghash::storeStaticFields(fh);

	for(u_int i = 0; i < g.size(); ++i) {
		g[i].storeObjectFields(fh);
	}

	fclose(fh);
}

//
void
LShash::restoreGhash(const char *_file) {
	FILE *fh = fopen(_file, "rb");
	assert(fh != NULL);

	assert(1 == fread(&K, sizeof(u_int), 1, fh));
	assert(1 == fread(&M, sizeof(int), 1, fh));
	assert(1 == fread(&prob, sizeof(double), 1, fh));

	Ghash::restoreStaticFields(fh);

	g.clear();
	int L = M*(M-1) / 2;
	u_int *uIndex = new u_int[U_NUM_IN_G];

	//# resize(c, obj), obj is NULL then throws exception.
	g.resize(L, Ghash(uIndex));

	for(int i = 0; i < L; ++i) {
		g[i].restoreObjectFields(fh);
	}

	fclose(fh);
}
