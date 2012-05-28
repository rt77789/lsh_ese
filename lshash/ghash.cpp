
#include "ghash.h"

#include "../utils/config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cmath>

using namespace eoaix;

//# Global u hash function points.
vector<Ghash::PointVector> Ghash::uPoints;
/* Random vector whose element is selected from Gaussian distribution. */
//vector<Ghash::PointVector> Ghash::_randVector;
//# h1 & h2 hash function times uPoints tables with respect to current query q.
vector< u64 > Ghash::h1TimesU[U_NUM_IN_G];
vector< u64 > Ghash::h2TimesU[U_NUM_IN_G];

vector< vector<u64> > Ghash::projectValue;
//# h1 & h2 random points, used for hashing.
vector<u64> Ghash::h1Points;
vector<u64> Ghash::h2Points;

Ghash::PointVector Ghash::testSample;

u_int Ghash::_M;
u_int Ghash::_K;
double Ghash::_b;
double Ghash::_W;
double Ghash::_R;
bool Ghash::_use_uhash;

//# initial Ghash static fields.
void
Ghash::init(u_int M, u_int K, double W, double R) {
	_M = M, _K = K, _W = W, _R = R;

	string testset_sample_path = Configer::get("project_dir").toString() + Configer::get("testset_sample_path").toString();
	_use_uhash = Configer::get("lsh_use_uhash").toBool();

	loadTestSample(testset_sample_path);

	_b = Util::randomByUniform(0.0, _W);

	uPoints.clear();
	projectValue.clear();
	h1Points.clear();
	h2Points.clear();

	for(int i = 0; i < U_NUM_IN_G; ++i)
		h1TimesU[i].clear(), h2TimesU[i].clear();

	for(u_int i = 0; i < _M; ++i) {
		vector<Point> u;
		for(int j = (int)_K/ 2; j >= 0; --j) {
			Point cp;
			selectRandomVector(cp);
			u.push_back(cp);
		}
		uPoints.push_back(u);
		//# set projectValue : M * K.
		projectValue.push_back(vector<u64>(_K, 0));
	}

	for(u_int i = 0; i < _K; ++i) {
		h1Points.push_back(Util::randomU64(0, (u64)1LL<<63));
		h2Points.push_back(Util::randomU64(0, (u64)1LL<<63));
	}

	for(u_int i = 0; i < U_NUM_IN_G; ++i) {
		h1TimesU[i].resize(_M, 0);
		h2TimesU[i].resize(_M, 0);
	}
}

/* Select a best random vector with predefined equation. */
void Ghash::selectRandomVector(Point &cp) {
	/* try vk times, and select the best random vector with minimize score. */
	int vk = Configer::get("lsh_random_vector_try").toInt();
	double minScore = 1e100;
	while(vk--) {
		Point p;	
		randomPoint(p);
		double score = evaluateVector(p);
		if(score < minScore) {
			cp = p;
			minScore = score;
		}
		//std::cout << "score: " << score << std::endl;
	}
}
/* Evaluate the random vector using testset_sample_path. */
double Ghash::evaluateVector(const Point &p) {
	/* for each node, average of the distance. */
	typedef map<u64, vector<size_t> > BlockMap;
	BlockMap bm;
	for(size_t i = 0; i < testSample.size(); ++i) {
		u64 offset = (u64)((testSample[i] * p + _b ) / _W);
		BlockMap::iterator iter = bm.find(offset);
		if(iter == bm.end()) {
			vector<size_t> block;
			block.push_back(i);
			bm[offset] = block;
		}
		else {
			iter->second.push_back(i);
		}
	}

	double disAve = 0;
	for(BlockMap::iterator iter	= bm.begin(); iter != bm.end(); ++iter) {
		double disSum = 0;
		int edgeNum = 0;
		for(size_t i = 0; i < iter->second.size(); ++i) {
			for(size_t j = i + 1; j < iter->second.size(); ++j) {
				double disTmp = (iter->second)[i] % (iter->second)[j];
				if(disTmp > disSum) disSum = disTmp;
				//disSum += (iter->second)[i] % (iter->second)[j];
				++edgeNum;
			}
		}
		if(edgeNum > 0 && disSum > disAve) {
			disAve = disSum;
		}
		/*
		   if(edgeNum > 0) {
		//disAve += disSum / edgeNum;
		}
		else {
		std::cout << "iter->second.size(): " << iter->second.size() << std::endl;
		}
		 */
	}
	return disAve;
}

/* Load testset sample. */
void Ghash::loadTestSample(const string &path) {
	ifstream in(path.c_str(), ios_base::binary);
	if(!in.is_open()) {
		clog << path + " open fail." << endl;
		throw runtime_error(path + " open fail.");
	}
	Point p;
	int num = 0;
	int rows = Configer::get("testset_sample_rows").toInt();
	while(num++ < rows && in.read((char*)&p, sizeof(Point))) {
		eoaix::normalize(p);
		testSample.push_back(p);
	}
	in.close();
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

	assert(1 == fwrite(&_M, sizeof(u_int), 1, fh));
	assert(1 == fwrite(&_K, sizeof(u_int), 1, fh));
	assert(1 == fwrite(&_b, sizeof(double), 1, fh));
	assert(1 == fwrite(&_W, sizeof(double), 1, fh));
	assert(1 == fwrite(&_R, sizeof(double), 1, fh));
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

	assert(1 == fread(&_M, sizeof(u_int), 1, fh));
	assert(1 == fread(&_K, sizeof(u_int), 1, fh));
	assert(1 == fread(&_b, sizeof(double), 1, fh));
	assert(1 == fread(&_W, sizeof(double), 1, fh));
	assert(1 == fread(&_R, sizeof(double), 1, fh));
}

//# random generate a point.
void
Ghash::randomPoint(Point &p) {
	for(u_int i = 0; i < DIMS; ++i) {
		p.d[i] = Util::randomByGaussian();
	}
}
//#
void
Ghash::preComputeFields(Point &q) {
	//# normalize.
	eoaix::normalize(q);
	//# Projection.
	for(u_int i = 0; i < uPoints.size(); ++i) {
		for(u_int j = 0; j < uPoints[i].size(); ++j) {
			//std::cout << "projectValue[i][j]: " << projectValue[i][j] << " | p*q: " << uPoints[i][j] * q << std::endl;
			projectValue[i][j] = (u64)((uPoints[i][j] * q + _b)/_W);
		}
	}

	for(u_int i = 0; i < U_NUM_IN_G; ++i) {
		for(u_int j = 0; j < _M; ++j) {
			h1TimesU[i][j] = h2TimesU[i][j] = 0;
			for(u_int k = 0; k < (_K/U_NUM_IN_G); ++k) {
				h1TimesU[i][j] = ((h1TimesU[i][j] + projectValue[j][i*(_K/U_NUM_IN_G) + k] * h1Points[i *(_K/U_NUM_IN_G) + k]) % HASH_PRIME ) % TABLE_PRIME;
				h2TimesU[i][j] = (h2TimesU[i][j] + projectValue[j][i*(_K/U_NUM_IN_G) + k] * h2Points[i *(_K/U_NUM_IN_G) + k]) % HASH_PRIME;
			}
		}
	}
}

Ghash::Ghash(u_int *puIndex) {
	for(u_int i = 0; i < U_NUM_IN_G; ++i)
		uIndex[i] = puIndex[i];
	memset(counter, 0, sizeof(u_int) * TABLE_PRIME);
	//# inital tables.
	memset(tables, 0, sizeof(Gnode*) * TABLE_PRIME);
	_unempty = 0;

	if(!_use_uhash) {
		_randVector.resize(_K);

		/* 这里设计有问题. */
		bool flag = true;
		std::string rvid = eoaix::itoa(puIndex[0], 10);

		std::string rvpath = Configer::get("project_dir").toString() + Configer::get("lsh_random_vector_path").toString() + rvid;

		if(Configer::get("lsh_load_random_vector").toBool()) {
			/* load from file. */
			std::ifstream rvin(rvpath.c_str());

			if(rvin.is_open()) {
				rvin >> _b;
				//cout << "load from random vector, rvpath: " << rvpath << endl;
				for(u_int i = 0; i < _K; ++i) {
					for(u_int j = 0; j < DIMS; ++j) {
						rvin >> _randVector[i].d[j];
			//			cout << _randVector[i].d[j] << " ";
					}
			//		cout << endl;
				}
				flag = false;
			}
			rvin.close();
		}
		if(flag) {
			//cout << "randomized select vector, rvpath: " << rvpath << endl;
			for(u_int i = 0; i < _K; ++i) {
				Point p;
				selectRandomVector(p);
				_randVector[i] = p;
			}
		}
		
		if(Configer::get("lsh_save_random_vector").toBool()) {
			std::ofstream orv(rvpath.c_str());
			assert(orv.is_open());
			orv << _b << endl;	
			for(u_int i = 0; i < _K; ++i) {
				for(u_int j = 0; j < DIMS; ++j) {
					if(j > 0) orv << " ";
					orv << _randVector[i].d[j];
				}
				orv << std::endl;
			}
			orv.close();
		}
	}
}

Ghash::~Ghash() {
	// free talbes[];
	for(int i = 0; i < TABLE_PRIME; ++i) {
		if(this->tables[i] != NULL)
			delloc(this->tables[i]);
	}
}

void Ghash::delloc(Gnode* ptr) {
	if(ptr->next != NULL) {
		delloc(ptr->next);
	}
	if(ptr != NULL) {
		delete ptr;
	}
}

u_int
Ghash::getMaxBucketLen() const {
	u_int res = 0;
	for(u_int i = 0; i < TABLE_PRIME; ++i) {
		res = res > counter[i] ? res : counter[i];
	}
	return res;
}

double Ghash::getAveBucketLen() const {
	double sum = getEntryNum();
	return getUnemptyNum() == 0 ? 0 : sum / getUnemptyNum();	
}

u_int Ghash::getEntryNum() const {
	u_int sum = 0;
	for(size_t i = 0; i < TABLE_PRIME; ++i) {
		sum += counter[i];
	}
	return sum;
}

u_int Ghash::getMedian() const {
	int len = getMaxBucketLen();
	int sum = getEntryNum();
	int cc[len];
	memset(cc, 0, sizeof(cc));
	for(int i = 0; i <TABLE_PRIME; ++i)
		++cc[counter[i]];
	int me = 0;
	for(int i = 1; i < len; ++i) {
		if(cc[i] > 0) {
			me += cc[i];
			if(me * 2 >= sum) return i;
		}
	}
	return 1;
}

std::string Ghash::showStat() {
	u_int len = getMaxBucketLen();
	u_int unemp = getUnemptyNum();
	double ave = getAveBucketLen();
	u_int sum = getEntryNum();
	u_int med = getMedian();

	std::stringbuf res;
	ostream os(&res);
	os << "sum: " << sum <<
		" | median: " << med << 
		" | xlen: " << len <<
		" | alen: " << ave << 
		" | unempty: " << unemp <<
		std::endl;
	std::string tres(res.str());
	return tres;
}

/* Return the number of unempty table. */
u_int Ghash::getUnemptyNum() const {
	return _unempty;
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
		++_unempty;
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

	if(_use_uhash) {
		for(u_int i = 0; i < U_NUM_IN_G; ++i) {
			h1mask = ((h1mask + h1TimesU[i][ uIndex[i] ]) % HASH_PRIME) % TABLE_PRIME;
			h2mask = (h2mask + h2TimesU[i][ uIndex[i] ]) % HASH_PRIME;
		}
	}
	else {
		for(u_int i = 0; i < _K; ++i) {
			u64 hv = (u64) ( (_randVector[i] * q + _b) / _W );
			h1mask = ( (h1mask + hv) % HASH_PRIME ) % TABLE_PRIME;
			h2mask = (h2mask + hv) % HASH_PRIME;
		}
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

	assert(1 == fwrite(&_unempty, sizeof(u_int), 1, fh));
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
	assert(1 == fread(&_unempty, sizeof(u_int), 1, fh));
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
