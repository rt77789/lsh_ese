#include "lshash.h"

LShash::LShash() {
	u_int K = 2;
	int M = 32;

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
LShash::findNodes(const Point &q, vector<identity> &eid) {
	Ghash::preComputeFields(q);
	set<u_int> idSet;

	for(u_int i = 0; i < g.size(); ++i) {
		Gnode *ptr = g.findNode(q);
		if(ptr != NULL) {
			if(idSet.find(ptr->identity) == idSet.end()) {
				idSet.insert(ptr->identity);
			}
		}
	}

	for(set<u_int>::iterator iter = idSet.begin(); iter != idSet.end(); ++iter) {
		eid.push_back(*iter);
	}
}

void
LShash::addNode(const Point &q) {
	for(u_int i = 0; i < g.size(); ++i)
		g[i].addNode(q);
}
