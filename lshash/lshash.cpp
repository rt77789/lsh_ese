#include "lshash.h"

#include "../utils/config.h"

#include <set>
#include <map>
#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

LShash::LShash() {
	}

LShash::~LShash() {
}

/* initializing. */
void LShash::init() {
/*K = 10;
	prob = 0.99;
	*/
	K = Configer::get("lsh_K").toInt();
	prob = Configer::get("lsh_prob").toDouble();

	M = estimateParaM(K, prob);

	cout << "M: " << M << endl;

	//int L = M*(M-1) / 2;

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
LShash::findNodes(const Point &q, std::vector<u_int> &eid) {
	Point p = q;
	//# p.d = q.d[] / R
	Ghash::preComputeFields(p);
	std::map<u_int, u_int> idMap;

	for(u_int i = 0; i < g.size(); ++i) {
		std::vector<u_int> tid;
		g[i].findNodes(p, tid);
		for(u_int j = 0; j < tid.size(); ++j) {
			++idMap[tid[j]];
			// cout << "identity: " << ptr->identity << endl;
		}
	}

	std::vector<pair<u_int, u_int> > vp;
	for(std::map<u_int, u_int>::iterator iter = idMap.begin(); iter != idMap.end(); ++iter) {
		//eid.push_back(*iter);
		vp.push_back(make_pair<u_int, u_int>(iter->second, iter->first));
	}
	std::sort(vp.begin(), vp.end(), greater<pair<u_int, u_int> >());

	for(u_int i = 0; i < vp.size(); ++i) {
	//	cout << "vp.first: " << vp[i].first << " | vp.second: " << vp[i].second << endl;
		eid.push_back(vp[i].second);
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

	cout << "begin store static fileds" << endl;
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
