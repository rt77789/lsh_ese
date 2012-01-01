#include "lshash.h"

#include "../utils/config.h"

#include <set>
#include <map>
#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

LShash::LShash() {
	}

LShash::~LShash() {
}

void LShash::init() {
	u_int K = Configer::get("lsh_K").toInt();
	double prob = Configer::get("lsh_prob").toDouble();
	double W = Configer::get("lsh_W").toDouble();
	double R = Configer::get("lsh_R").toDouble();

	init(K, prob, W, R);
}

void LShash::init(u_int K, double prob, double W, double R) {
	int M = Configer::get("lsh_M").toInt();
	if(M == 0 && Configer::get("lsh_use_uhash").toBool()) {
		M = estimateParaM(K, prob, W);
	}
	printf("K: %u - prob: %lf - W: %lf - R: %lf - M: %d\n", K, prob, W, R, M);
	init(K, M, prob, W, R);
}

/* initializing. */
void LShash::init(u_int K, int M, double prob, double W, double R) {
	bool load_index = Configer::get("lsh_load_index").toBool();

	_K = K, _prob = prob, _M = M, _W = W,_R = R;

	std::cout << "load_index before" << std::endl;
	if(!load_index) {

		Ghash::init(_M, _K, _W, _R);

		u_int *uIndex = new u_int[2];
		if(Configer::get("lsh_use_uhash").toBool()) {
			std::cout << "using u hash. " << std::endl;
			/* Using u hash. */
			for(int i = 0; i < _M; ++i) {
				for(int j = i+1; j < _M; ++j) {
					uIndex[0] = i, uIndex[1] = j;
					_g.push_back(Ghash(uIndex));
				}
			}
		}
		else {
			/* Using L random vectors. */
			uIndex[0] = uIndex[1] = 0;
			int L = Configer::get("lsh_L").toInt();
			std::cout << "L: " << L << std::endl;
			for(int i = L; i > 0; --i) {
				_g.push_back(Ghash(uIndex));
			}
		}
		if(uIndex == NULL) {
			delete[] uIndex;
		}

		std::cout << "before buildIndex" << std::endl;
		buildIndex();
		cout << "load Point over." << endl;
		bool doSave = Configer::get("lsh_do_save").toBool();
		if(doSave) {
			string indexPath = Configer::get("project_dir").toString() + Configer::get("lsh_index_path").toString();
			storeGhash(indexPath.c_str());
		}
	}
	else {
		string indexPath = Configer::get("project_dir").toString() + Configer::get("lsh_index_path").toString();
		restoreGhash(indexPath.c_str());
	}
}
/* Build index. */
void LShash::buildIndex() {
	std::string path = Configer::get("project_dir").toString() + Configer::get("naive_dataset_path").toString();

	std::ifstream in(path.c_str(), ios_base::binary);

	if(!in.is_open()) {
		std::clog << path + " open failed..." << std::endl;
		throw path + " open failed...";
	}

	Point p;
	int cur_row = 0;
	int rows = Configer::get("rows").toInt();
	//std::cout << "buildIndex(lsh), rows: " << rows << std::endl;

	while(cur_row < rows && in.read((char*)&p, sizeof(Point))) {
		addNode(p);
		++cur_row;
	}

	in.close();
}
void
LShash::tuneParameter() {

}

int
LShash::estimateParaM(int K, double prob, double W) {
	//# K should be a even number.
	assert((K & 1) == 0);
	double w = 4;
	double c = 1;
	double x = w / c;
	double s = 1 - erfc(x / M_SQRT2) - M_2_SQRTPI / M_SQRT2 / x * (1 - exp((-x*x) / 2));
	double mu = 1 - pow(s, K / 2);
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
	for(u_int i = 0; i < _g.size(); ++i) {
		int tmp = _g[i].getMaxBucketLen();
		res = res > tmp ? res : tmp;
	}
	return res;
}

void LShash::showStat() {
	for(size_t i = 0; i < _g.size(); ++i) {
		cout << "Table[" << i << "]:" << _g[i].showStat();	
	}
}

void
LShash::find(const Point &q, std::vector<u_int> &eid) {
	Point p = q;
	//# p.d = q.d[] / R
	Ghash::preComputeFields(p);
	std::map<u_int, u_int> idMap;

	u_int lsh_max_candidate = Configer::get("lsh_max_candidate").toInt();
	for(u_int i = 0; i < _g.size(); ++i) {
		std::vector<u_int> tid;
		_g[i].findNodes(p, tid);
		for(u_int j = 0; j < tid.size(); ++j) {
			++idMap[tid[j]];
			// cout << "identity: " << ptr->identity << endl;
		}
		/* If the current map size is bigger than lsh_max_candidate, then exit the loop. */
		if(idMap.size() >= lsh_max_candidate) break;
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
	for(u_int i = 0; i < _g.size(); ++i)
		_g[i].addNode(p);
}

void
LShash::storeGhash(const char *_file) {
	FILE *fh = fopen(_file, "wb");
	assert(fh != NULL);
	
	//# 
	assert(1 == fwrite(&_K, sizeof(u_int), 1, fh));
	assert(1 == fwrite(&_M, sizeof(int), 1, fh));
	assert(1 == fwrite(&_prob, sizeof(double), 1, fh));

	cout << "begin store static fileds" << endl;
	Ghash::storeStaticFields(fh);

	for(u_int i = 0; i < _g.size(); ++i) {
		_g[i].storeObjectFields(fh);
	}

	fclose(fh);
}

//
void
LShash::restoreGhash(const char *_file) {
	FILE *fh = fopen(_file, "rb");
	assert(fh != NULL);

	assert(1 == fread(&_K, sizeof(u_int), 1, fh));
	assert(1 == fread(&_M, sizeof(int), 1, fh));
	assert(1 == fread(&_prob, sizeof(double), 1, fh));

	Ghash::restoreStaticFields(fh);

	_g.clear();
	int L = _M*(_M-1) / 2;
	u_int *uIndex = new u_int[U_NUM_IN_G];

	//# resize(c, obj), obj is NULL then throws exception.
	_g.resize(L, Ghash(uIndex));

	for(int i = 0; i < L; ++i) {
		_g[i].restoreObjectFields(fh);
	}
	if(uIndex == NULL) {
		delete[] uIndex;
	}

	fclose(fh);
}
