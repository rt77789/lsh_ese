
#include "ghash.h"
#include "util.h"
#include <iostream>

//# Global u hash function points.
vector< vector<Point> > Ghash::uPoints;
//# h1 & h2 hash function times uPoints tables with respect to current query q.
vector< u64 > Ghash::h1TimesU[U_NUM_IN_G];
vector< u64 > Ghash::h2TimesU[U_NUM_IN_G];

vector< vector<u64> > Ghash::projectValue;
//# h1 & h2 random points, used for hashing.
vector<u64> Ghash::h1Points;
vector<u64> Ghash::h2Points;

u_int Ghash::M;
u_int Ghash::K;
double Ghash::b;
double Ghash::w;


//# initial Ghash static fields.
void
Ghash::init(u_int _M, u_int _K) {
	M = _M, K = _K;
	w = 4;
	b = Util::randomByUniform(0.0, w);

	uPoints.clear();
	projectValue.clear();
	h1Points.clear();
	h2Points.clear();

	for(int i = 0; i < U_NUM_IN_G; ++i)
		h1TimesU[i].clear(), h2TimesU[i].clear();

	for(u_int i = 0; i < M; ++i) {
		vector<Point> u;
		for(int j = (int)K / 2; j >= 0; --j) {
			// std::cout << i << ' ' << j << std::endl;
			Point p;
			randomPoint(p);
			u.push_back(p);
		}
		uPoints.push_back(u);
		//# set projectValue : M * K.
		projectValue.push_back(vector<u64>(K, 0));
	}

	for(u_int i = 0; i < K; ++i) {
		h1Points.push_back(Util::randomU64(0, (u64)1LL<<63));
		h2Points.push_back(Util::randomU64(0, (u64)1LL<<63));
	}
	
	for(u_int i = 0; i < U_NUM_IN_G; ++i) {
		h1TimesU[i].resize(M, 0);
		h2TimesU[i].resize(M, 0);
	}
}

//# random generate a point.
void
Ghash::randomPoint(Point &_p) {
	for(u_int i = 0; i < DIMS; ++i) {
		_p.d[i] = Util::randomByUniform(-5,5);
	}
}

//#
void
Ghash::preComputeFields(const Point &q) {
	//# Projection.
	for(u_int i = 0; i < uPoints.size(); ++i) {
		for(u_int j = 0; j < uPoints[i].size(); ++j) {
			projectValue[i][j] = (u64)(((uPoints[i][j] * q) + b)/w);
		}
	}
	
	for(u_int i = 0; i < U_NUM_IN_G; ++i) {
		for(u_int j = 0; j < M; ++j) {
			h1TimesU[i][j] = h2TimesU[i][j] = 0;
			for(u_int k = 0; k < (K/U_NUM_IN_G); ++k) {
				h1TimesU[i][j] = ((h1TimesU[i][j] + projectValue[j][i*(K/U_NUM_IN_G) + k] * h1Points[i *(K/U_NUM_IN_G) + k]) % HASH_PRIME ) % TABLE_PRIME;
				h2TimesU[i][j] = (h2TimesU[i][j] + projectValue[j][i*(K/U_NUM_IN_G) + k] * h2Points[i *(K/U_NUM_IN_G) + k]) % HASH_PRIME;
			}
		}
	}
}

Ghash::Ghash(u_int *_uIndex) {
	for(u_int i = 0; i < U_NUM_IN_G; ++i)
		uIndex[i] = _uIndex[i];
}

Ghash::~Ghash() {
}

u_int
Ghash::getMaxLen() {
	u_int res = 0;
	for(u_int i = 0; i < TABLE_PRIME; ++i) {
		res = res > counter[i] ? res : counter[i];
	}
	return res;
}

void
Ghash::addNode(const Point &q) {
	Gnode *ptr = new Gnode();
	
	pair<u64, u64> mask = calh1Andh2(q);

	ptr->h2value = mask.second;
	ptr->identity = q.identity;

	if(tables[mask.first] == NULL) {
		ptr->next = NULL;
		tables[mask.first] = ptr;
		counter[mask.first] = 1;
	}
	else {
		++counter[mask.first];
		Gnode *ans = tables[mask.first], *pre = ans;
		while(ans != NULL && ans->h2value < ptr->h2value) {
			pre = ans;
			ans = ans->next;
		}
		//ptr->next = tables[mask.first]->next;
		ptr->next = ans;
		if(pre == tables[mask.first]) {
			tables[mask.first] = ptr;
		}
		else {
			pre->next = ptr;
		}
	}
}

//# It's not right here. Replace this function with findNodes();
Gnode*
Ghash::findNode(const Point &q) {
	pair<u64, u64> mask = calh1Andh2(q);
	
	Gnode *ptr = tables[mask.first];
	while(ptr != NULL) {
		//# It's wrong here, because there're many node whose h2value == mask.second.
		if(ptr->h2value == mask.second)
			return ptr;
		ptr = ptr->next;
	}
	return NULL;
}

void
Ghash::findNodes(const Point &q, vector<u_int> &eid) {
	pair<u64, u64> mask = calh1Andh2(q);
	
	Gnode *ptr = tables[mask.first];
	while(ptr != NULL) {
		//# It's wrong here, because there're many node whose h2value == mask.second.
		if(ptr->h2value == mask.second)
			eid.push_back(ptr->identity);
		ptr = ptr->next;
	}
}

pair<u64, u64>
Ghash::calh1Andh2(const Point &q) {
	u64 h1mask = 0, h2mask = 0;

	for(u_int i = 0; i < U_NUM_IN_G; ++i) {
		h1mask = ((h1mask + h1TimesU[i][ uIndex[i] ]) % HASH_PRIME) % TABLE_PRIME;
		h2mask += (h2mask + h2TimesU[i][ uIndex[i] ]) % HASH_PRIME;
	}
	return make_pair<u64, u64>(h1mask, h2mask);
}
