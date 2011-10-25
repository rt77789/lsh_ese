#include "lshash.h"
#include <set>
#include <cassert>
#include <cmath>
#include <iostream>
using namespace std;

LShash::LShash() {
	u_int K = 16;
	int M = estimateParaM(K, 0.9);
	cout << "M: " << M << endl;

	int L = M*(M-1) / 2;

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
	int m = (1 - y / log(mu) - 1 / (1-mu) + 0.5);
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
LShash::findNodes(const Point &q, vector<u_int> &eid) {
	Ghash::preComputeFields(q);
	set<u_int> idSet;

	for(u_int i = 0; i < g.size(); ++i) {
		vector<u_int> tid;
		g[i].findNodes(q, tid);
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
	Ghash::preComputeFields(q);
	for(u_int i = 0; i < g.size(); ++i)
		g[i].addNode(q);
}
