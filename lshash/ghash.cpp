
#include "ghash.h"

#include "../utils/config.h"

#include <iostream>
#include <cstring>
#include <cmath>

using namespace eoaix;

//# Global u hash function points.
vector< vector<Point> > Ghash::uPoints;
//# h1 & h2 hash function times uPoints tables with respect to current query q.
vector< u64 > Ghash::h1TimesU[U_NUM_IN_G];
vector< u64 > Ghash::h2TimesU[U_NUM_IN_G];

vector< vector<u64> > Ghash::projectValue;
//# h1 & h2 random points, used for hashing.
vector<u64> Ghash::h1Points;
vector<u64> Ghash::h2Points;

u_int Ghash::mm;
u_int Ghash::kk;
double Ghash::b;
double Ghash::w;
double Ghash::R;

//# initial Ghash static fields.
void
Ghash::init(u_int _M, u_int _K) {
	mm = _M, kk = _K;

	//w = 2;
	//R = 0.1;
	w = Configer::get("lsh_W").toInt();
	R = Configer::get("lsh_R").toDouble();

	b = Util::randomByUniform(0.0, w);

	uPoints.clear();
	projectValue.clear();
	h1Points.clear();
	h2Points.clear();

	for(int i = 0; i < U_NUM_IN_G; ++i)
		h1TimesU[i].clear(), h2TimesU[i].clear();

	for(u_int i = 0; i < mm; ++i) {
		vector<Point> u;
		for(int j = (int)kk/ 2; j >= 0; --j) {
			// std::cout << i << ' ' << j << std::endl;
			Point p;
			randomPoint(p);
			u.push_back(p);
		}
		uPoints.push_back(u);
		//# set projectValue : M * K.
		projectValue.push_back(vector<u64>(kk, 0));
	}

	for(u_int i = 0; i < kk; ++i) {
		h1Points.push_back(Util::randomU64(0, (u64)1LL<<63));
		h2Points.push_back(Util::randomU64(0, (u64)1LL<<63));
	}
	
	for(u_int i = 0; i < U_NUM_IN_G; ++i) {
		h1TimesU[i].resize(mm, 0);
		h2TimesU[i].resize(mm, 0);
	}
}

//# Store all static fields into into external index file.
void
Ghash::storeStaticFields(FILE *fh) {
	assert(fh != NULL);

	//# uPoints info.
	//# uPoints.size()|uPoints[0].size()|uPoints[0][0].d[0]...uPoints[0][0].d[DIMS-1]|uPoints[1].size()|...
	u_int len = uPoints.size();
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
	for(u_int i = 0; i < uPoints.size(); ++i) {
		len = uPoints[i].size();
		assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
		for(u_int j = 0; j < uPoints[i].size(); ++j) {
			assert(1 == fwrite(&uPoints[i][j], sizeof(Point), 1, fh));
		}
	}
	//# h1TimesU info.
	len = U_NUM_IN_G;
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));

	for(int i = 0; i < U_NUM_IN_G; ++i) {
		len = h1TimesU[i].size();
		assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
		for(u_int j = 0; j < h1TimesU[i].size(); ++j) {
			assert(1 == fwrite(&h1TimesU[i][j], sizeof(u64), 1, fh));
		}
	}
	//# h2TimesU info.
	len = U_NUM_IN_G;
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));

	for(int i = 0; i < U_NUM_IN_G; ++i) {
		len = h2TimesU[i].size();
		assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
		for(u_int j = 0; j < h2TimesU[i].size(); ++j) {
			assert(1 == fwrite(&h2TimesU[i][j], sizeof(u64), 1, fh));
		}
	}

	//# projectValue info.
	len = projectValue.size();
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));

	for(u_int i = 0; i < projectValue.size(); ++i) {
		len = projectValue[i].size();
		assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
		for(u_int j = 0; j < projectValue[i].size(); ++j) {
			assert(1 == fwrite(&projectValue[i][j], sizeof(u64), 1, fh));	
		}
	}
	//# h1Points info.

	len = h1Points.size();
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
	for(u_int i = 0; i < h1Points.size(); ++i) {
		assert(1 == fwrite(&h1Points[i], sizeof(u64), 1, fh));
	}
	//# h2Points info.

	len = h2Points.size();
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
	for(u_int i = 0; i < h2Points.size(); ++i) {
		assert(1 == fwrite(&h2Points[i], sizeof(u64), 1, fh));
	}

	//# M, K, b, w;

	assert(1 == fwrite(&mm, sizeof(u_int), 1, fh));
	assert(1 == fwrite(&kk, sizeof(u_int), 1, fh));
	assert(1 == fwrite(&b, sizeof(double), 1, fh));
	assert(1 == fwrite(&w, sizeof(double), 1, fh));
	assert(1 == fwrite(&R, sizeof(double), 1, fh));
}

//# Just the restore the static fields info.
void 
Ghash::restoreStaticFields(FILE *fh) {
	assert(fh != NULL);

	//# uPoints info.
	//# uPoints.size()|uPoints[0].size()|uPoints[0][0].d[0]...uPoints[0][0].d[DIMS-1]|uPoints[1].size()|...
	uPoints.clear();
	u_int len = 0;
	assert(1 == fread(&len, sizeof(u_int), 1, fh));

	for(u_int i = 0; i < len; ++i) {
		u_int lenj = 0;
		assert(1 == fread(&lenj, sizeof(u_int), 1, fh));
		vector<Point> vps;
		for(u_int j = 0; j < lenj; ++j) {
			Point p;
			assert(1 == fread(&p, sizeof(Point), 1, fh));
			vps.push_back(p);
		}
		uPoints.push_back(vps);
	}
	//# h1TimesU info.
	assert(1 == fread(&len, sizeof(u_int), 1, fh));
	assert(len == U_NUM_IN_G);

	for(int i = 0; i < U_NUM_IN_G; ++i) {
		u_int lenj = 0;
		assert(1 == fread(&lenj, sizeof(u_int), 1, fh));
		h1TimesU[i].clear();
		for(u_int j = 0; j < lenj; ++j) {
			u64 tu = 0;
			assert(1 == fread(&tu, sizeof(u64), 1, fh));
			h1TimesU[i].push_back(tu);
		}
	}
	//# h2TimesU info.
	assert(1 == fread(&len, sizeof(u_int), 1, fh));
	assert(len == U_NUM_IN_G);

	for(int i = 0; i < U_NUM_IN_G; ++i) {
		u_int lenj = 0;
		assert(1 == fread(&lenj, sizeof(u_int), 1, fh));
		h2TimesU[i].clear();
		for(u_int j = 0; j < lenj; ++j) {
			u64 tu = 0;
			assert(1 == fread(&tu, sizeof(u64), 1, fh));
			h2TimesU[i].push_back(tu);
		}
	}
	

	//# projectValue info.
	projectValue.clear();
	assert(1 == fread(&len, sizeof(u_int), 1, fh));

	for(u_int i = 0; i < len; ++i) {
		u_int lenj = 0;
		assert(1 == fread(&lenj, sizeof(u_int), 1, fh));
		vector<u64> vu;
		for(u_int j = 0; j < lenj; ++j) {
			u64 tu = 0;
			assert(1 == fread(&tu, sizeof(u64), 1, fh));	
			vu.push_back(tu);
		}
		projectValue.push_back(vu);
	}
	//# h1Points info.
	h1Points.clear();
	assert(1 == fread(&len, sizeof(u_int), 1, fh));
	for(u_int i = 0; i < len; ++i) {
		u64 tu = 0;
		assert(1 == fread(&tu, sizeof(u64), 1, fh));
		h1Points.push_back(tu);
	}
	//# h2Points info.
	h2Points.clear();
	assert(1 == fread(&len, sizeof(u_int), 1, fh));
	for(u_int i = 0; i < len; ++i) {
		u64 tu = 0;
		assert(1 == fread(&tu, sizeof(u64), 1, fh));
		h2Points.push_back(tu);
	}
	//# M, K, b, w;

	assert(1 == fread(&mm, sizeof(u_int), 1, fh));
	assert(1 == fread(&kk, sizeof(u_int), 1, fh));
	assert(1 == fread(&b, sizeof(double), 1, fh));
	assert(1 == fread(&w, sizeof(double), 1, fh));
	assert(1 == fread(&R, sizeof(double), 1, fh));
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
Ghash::preComputeFields(Point &q) {
	//# normalize.
	double maxd = 0;
	const double eps = 1e-32;
	//# Normalize the initial signal, point[] / max{ point[] }.
	//for(int i = 0; i < DIMS; ++i)
	//	maxd = maxd > q.d[i] ? maxd : q.d[i];
	for(int i = 0; i < DIMS; ++i)
		maxd += q.d[i];
	maxd /= DIMS;

	for(int i = 0; i < DIMS; ++i)
		q.d[i] -= maxd;
	double ts = 0;
	for(int i = 0; i < DIMS; ++i)
		ts += q.d[i] * q.d[i];
	ts = sqrt(ts);

	//# ??
	//assert(fabs(maxd) >= eps);

	for(int i = 0; i < DIMS; ++i)
		q.d[i] = q.d[i] / ts / R;
		//q.d[i] = q.d[i] / maxd / R;

	//# Projection.
	for(u_int i = 0; i < uPoints.size(); ++i) {
		for(u_int j = 0; j < uPoints[i].size(); ++j) {
			projectValue[i][j] = (u64)(((uPoints[i][j] * q) + b)/w);
		}
	}
	
	for(u_int i = 0; i < U_NUM_IN_G; ++i) {
		for(u_int j = 0; j < mm; ++j) {
			h1TimesU[i][j] = h2TimesU[i][j] = 0;
			for(u_int k = 0; k < (kk/U_NUM_IN_G); ++k) {
				h1TimesU[i][j] = ((h1TimesU[i][j] + projectValue[j][i*(kk/U_NUM_IN_G) + k] * h1Points[i *(kk/U_NUM_IN_G) + k]) % HASH_PRIME ) % TABLE_PRIME;
				h2TimesU[i][j] = (h2TimesU[i][j] + projectValue[j][i*(kk/U_NUM_IN_G) + k] * h2Points[i *(kk/U_NUM_IN_G) + k]) % HASH_PRIME;
			}
		}
	}
}

Ghash::Ghash(u_int *_uIndex) {
	for(u_int i = 0; i < U_NUM_IN_G; ++i)
		uIndex[i] = _uIndex[i];
	memset(counter, 0, sizeof(u_int) * TABLE_PRIME);
	//# inital tables.
	memset(tables, 0, sizeof(Gnode*) * TABLE_PRIME);
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
		Gnode *ans = tables[mask.first], *pre = NULL;
		while(ans != NULL && ans->h2value < ptr->h2value) {
			pre = ans;
			ans = ans->next;
		}
		//ptr->next = tables[mask.first]->next;
		ptr->next = ans;
		if(pre == NULL) {
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

//# Discard.
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

//# store ojbect fileds.
void
Ghash::storeObjectFields(FILE *fh) {
	//# uIndex[U_NUM_IN_G] info.
	u_int len = U_NUM_IN_G;
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
	for(int i = 0; i < U_NUM_IN_G; ++i) {
		assert(1 == fwrite(&uIndex[i], sizeof(u_int), 1, fh));
	}
	//# counter & tables.
	len = TABLE_PRIME;
	assert(1 == fwrite(&len, sizeof(u_int), 1, fh));
	for(int i = 0; i < TABLE_PRIME; ++i) {
		assert(1 == fwrite(&counter[i], sizeof(u_int), 1, fh));
		Gnode *ptr = tables[i];
		/*
		cout << counter[i] << endl;
		int tnum = 0;
		while(ptr != NULL) {
			ptr = ptr->next;
			++tnum;
		}
		cout << "tnum: " << tnum << " - counter[i]:" << counter[i] << endl;
		assert(tnum == counter[i]);
		ptr = tables[i];
		*/
		for(u_int j = 0; j < counter[i]; ++j) {
			assert(ptr != NULL);
			assert(1 == fwrite(ptr, sizeof(Gnode), 1, fh));
			ptr = ptr->next;
		}
	}
}

//# Restore object fields.
void
Ghash::restoreObjectFields(FILE *fh) {
	//# uIndex[U_NUM_IN_G] info.
	u_int len = 0;
	assert(1 == fread(&len, sizeof(u_int), 1, fh));
	assert(len == U_NUM_IN_G);

	for(int i = 0; i < U_NUM_IN_G; ++i) {
		assert(1 == fread(&uIndex[i], sizeof(u_int), 1, fh));
	}
	//# counter & tables.
	len = 0;
	assert(1 == fread(&len, sizeof(u_int), 1, fh));
	assert(len == TABLE_PRIME);

	for(int i = 0; i < TABLE_PRIME; ++i) {
		assert(1 == fread(&counter[i], sizeof(u_int), 1, fh));
		Gnode *pre = NULL;

		for(u_int j = 0; j < counter[i]; ++j) {
			Gnode *ptr = new Gnode();
			assert(ptr != NULL);
			assert(1 == fread(ptr, sizeof(Gnode), 1, fh));
			ptr->next = NULL;

			if(j == 0) {
				pre = tables[i] = ptr;
			}
			else {
				pre->next = ptr;
				pre = ptr;
			}
		}
	}
}
