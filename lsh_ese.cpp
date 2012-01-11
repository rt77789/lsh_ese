#include "lsh_ese.h"
#include "structs/searcher.h"

#include <sstream>
#include <fstream>
#include <algorithm>

LShashESE::LShashESE() {
	string dataset = Configer::get("project_dir").toString() + Configer::get("naive_dataset_path").toString();
	fhandle.open(dataset.c_str(), ios::binary);
	if(!fhandle) {
		throw;
	}
}

LShashESE::~LShashESE() {
	if(fhandle)
		fhandle.close();
}

void
LShashESE::init(const string &type) {
	if(type == "mpl") {
		mpl.init();
	}
	else if(type == "lsh") {
		lsh.init();
	}
	else if(type == "flann") {
		flann.init();
	}
	else if(type == "fft" || type == "wavelet") {
	}
	else if(type == "kdmpl") {
		mpl.init();
		flann.init();
	}
	else if(type == "kdlsh") {
		lsh.init();
		flann.init();
	}
	else {
		throw ;
	}
}

int
LShashESE::findIndex(const vector<double> &sin, vector<SearchRes> &resig, const string &_lshtype) {
	
	//# Find by LSHash.
	vector<u_int> eid;
	if(_lshtype == "mpl") {
		cout << "mpl find..." << endl;
		float *fsin = new float[sin.size()];
		for(u_int i = 0; i < sin.size(); ++i)
			fsin[i] = sin[i];
		mpl.query(fsin, sin.size(), eid);
		if(fsin != NULL)
		delete fsin;
	}
	else if(_lshtype == "flann") {
		cout << "flann find..." << endl;
		float *fsin = new float[sin.size()];
		for(u_int i = 0; i < sin.size(); ++i)
			fsin[i] = sin[i];
		int checks = Configer::get("flann_kdtree_checks").toInt();
		flann.find(fsin, checks, eid);
		if(fsin != NULL)
		delete fsin;
	}
	else if(_lshtype == "lsh") {
		cout << "basic lsh find..." << endl;
		Point p;
		for(size_t i = 0; i < sin.size(); ++i) {
			p.d[i] = sin[i];
		}
		lsh.find(p, eid);
	}
	else if(_lshtype == "fft") {
		cout << "fft find..." << endl;
		return naiveFFTConvFind(sin, resig);
	}
	else if(_lshtype == "wavelet") {
		cout << "wavelet naive find..." << endl;
		return naiveWaveletFind(sin, resig);
	}
	else if(_lshtype == "kdmpl") {
		cout << "kdmpl(mpl & flann) find..." << endl;
		float *fsin = new float[sin.size()];
		for(u_int i = 0; i < sin.size(); ++i)
			fsin[i] = sin[i];
		int checks = Configer::get("flann_kdtree_checks").toInt();
		flann.find(fsin, checks, eid);

		vector<u_int> teid;
		mpl.query(fsin, sin.size(), teid);

		set<u_int> tmps;
		for(size_t i = 0; i < teid.size(); ++i) {
			tmps.insert(teid[i]);
		}
		for(size_t i = 0; i < eid.size(); ++i)
			tmps.insert(eid[i]);
		eid.clear();
		for(std::set<u_int>::iterator iter = tmps.begin(); iter != tmps.end(); ++iter) {
			eid.push_back(*iter);
		}
		if(fsin != NULL)
		delete fsin;
	}
	else if(_lshtype == "kdlsh") {
		cout << "kdlsh(lsh & flann) find..." << endl;
		float *fsin = new float[sin.size()];
		for(u_int i = 0; i < sin.size(); ++i)
			fsin[i] = sin[i];
		int checks = Configer::get("flann_kdtree_checks").toInt();
		flann.find(fsin, checks, eid);

		Point p;
		for(size_t i = 0; i < sin.size(); ++i) {
			p.d[i] = sin[i];
		}
		lsh.find(p, eid);
		vector<u_int> teid;
		lsh.find(p, teid);

		set<u_int> tmps;
		for(size_t i = 0; i < teid.size(); ++i) {
			tmps.insert(teid[i]);
		}
		for(size_t i = 0; i < eid.size(); ++i)
			tmps.insert(eid[i]);
		eid.clear();
		for(std::set<u_int>::iterator iter = tmps.begin(); iter != tmps.end(); ++iter) {
			eid.push_back(*iter);
		}
		if(fsin != NULL)
		delete fsin;
	}
	else {
		throw;
	}

	cout << "lsh.find returns: eid.size() == " << eid.size() << endl;

	//# sort eid, decreate disk move. Make sure sort eid[] in Searcher.
	// sort(eid.begin(), eid.end());
	vector<SearchRes> res;
	res.swap(Searcher::search(eid, sin));
	resig.swap(res);
	int topk = Configer::get("project_top_k").toInt();
	if(resig.size() > topk) resig.resize(topk);
	cout << "resig.size(): " << resig.size() << "  - topk: " << topk << endl;

	return eid.size();
}

void LShashESE::queryDB(const vector<double> &sin, const vector<u_int> &eid, vector<SearchRes> &resig) {

#ifdef L2NORM
	resig.clear();
	for(u_int i = 0; i < eid.size(); ++i) {
		Point p;
		assert(true == readPoint(eid[i], p));
		vector<double> tin(p.d, p.d + DIMS);
		double sim = FFT::corr(tin, sin);

		resig.push_back(SearchRes(p.identity, sim, tin));
	}
	sort(resig.begin(), resig.end());
	u_int top_k = Configer::get("project_top_k").toInt();
	if(resig.size() > top_k) {
		resig.resize(top_k);
	}
	for(size_t i = 0; i < resig.size(); ++i) {
		std::cout << "[" << i << "]: " << resig[i].getSim() << " - index: " << resig[i].getID() << endl;
	}

#else
	WaveletEps twe(sin);
	vector< vector<double> > vtin;
	vector<u_int> iden;

	for(u_int i = 0; i < eid.size(); ++i) {
		Point p;
		assert(true == readPoint(eid[i], p));
		vector<double> tin(p.d, p.d + DIMS);

		vtin.push_back(tin);
		iden.push_back(p.identity);

		if(vtin.size() >= IN_MEMORY_NUM) {
			try {
				twe.batch_push(vtin, iden);
			}catch(...) {
				cerr << "batch_push exception" << endl;
				throw;
			}
			vtin.clear();
			iden.clear();
		}
	}

	if(vtin.size() > 0) {
		twe.batch_push(vtin, iden);
	}

	vector<WSSimilar> &vwss = twe.find(sin);

	resig.clear();
	u_int top_k = Configer::get("project_top_k").toInt();
	for(u_int i = 0; i < vwss.size() && i < top_k; ++i) {
		cout << "[" << i << "]: " << vwss[i].sim << " - index: " << vwss[i].id << endl;
		resig.push_back(SearchRes(vwss[i].id, vwss[i].sim, vwss[i].ws.wsig[vwss[i].ws.wsig.size()-1].sig));
	}
#endif
}

int
LShashESE::naiveFFTConvFind(const vector<double> &sin, vector<SearchRes> &resig) {
	resig.swap(Searcher::search(sin));
	int rows = Configer::get("rows").toInt();
	return rows;
}

int
LShashESE::naiveWaveletFind(const vector<double> &sin, vector<SearchRes> &resig) {
	assert(sin.size() == DIMS);
	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	WaveletEps twe(sin);
	fhandle.clear();
	fhandle.seekg(0LL, ios_base::beg);
	//assert(0 == fseek(fhandle, 0LL, SEEK_SET));

	int tnum = 0;

	vector< vector<double> > vtin;
	vector<u_int> iden;
	Point p[BATCH_READ_NUM];
	int cpnum = 0;

	//while((cpnum = fread(&p, sizeof(Point), 1, fhandle)) > 0) {
	while(!fhandle.fail() && !fhandle.eof()) {
		fhandle.read((char*)p, sizeof(Point) * BATCH_READ_NUM);
		cpnum = fhandle.gcount() / sizeof(Point);
		assert(fhandle.gcount() % sizeof(Point) == 0);
		for(int i = 0; i < cpnum; ++i) {
			vector<double> tin(p[i].d, p[i].d + DIMS);
			//# Can't load all dataset in memory.
			vtin.push_back(tin);
			iden.push_back(p[i].identity);
			if(vtin.size() >= IN_MEMORY_NUM) {
				twe.batch_push(vtin, iden);
				vtin.clear();
				iden.clear();
			}
		}
		tnum += cpnum;
	}
	if(vtin.size() > 0) {
		twe.batch_push(vtin, iden);
	}

	cout << "Wavelet FFT total signals : " << tnum << endl;

	vector<WSSimilar> &vwss = twe.find(sin);

	resig.clear();
	u_int top_k = Configer::get("project_top_k").toInt();
	for(u_int i = 0; i < vwss.size() && i < top_k; ++i) {
		cout << "[" << i << "]: " << vwss[i].sim << " - index: " << vwss[i].id << endl;
		resig.push_back(SearchRes(vwss[i].id, vwss[i].sim, vwss[i].ws.wsig[vwss[i].ws.wsig.size()-1].sig));
	}
	return tnum;
}

//# Read a point from external index file and insert into waveletEps obejct.
bool
LShashESE::readPoint(u_int index, Point &p) {
	//int offset = sizeof(u_int) + DIMS * sizeof(double);
	u64 offset = (u64)sizeof(Point) * index;
	fhandle.clear();
/*
	fhandle.seekg(0LL, ios::end);
	assert(!(fhandle.fail() && fhandle.bad()));
	long long length = fhandle.tellg();

	clog << "offset: " << offset << " | length: " << length << endl;
	*/
	fhandle.seekg(offset, ios_base::beg);
	assert(!(fhandle.fail() && fhandle.bad()));
	fhandle.read((char*)&p, sizeof(Point));
	//int rv = fread(&p, sizeof(Point), 1, fhandle);
	//clog << "p.identity: " << p.identity << " | index: " << index << endl;
	assert(p.identity == index);
	return !(fhandle.fail() || fhandle.eof() || fhandle.bad());
}

//# Transform dataset into binary format.
void
LShashESE::transformDataSet(const char *_fin, const char *_fout) {
	ifstream in(_fin);
	assert(in.is_open());
	FILE *fh = fopen(_fout, "wb");
	assert(fh != NULL);

	string line;
	int i = 0;

	while(getline(in, line)) {
		istringstream iss(line);
		double d;
		Point p;
		p.identity = i++;
		u_int j = 0;
		string sd;
		while(iss >> sd) {
			try {
				p.d[j++] = eoaix::sci2double(sd);	
			}catch(...) {
				cerr << "sci2double exception." << endl;
				throw;
			}
		}
		assert(j == DIMS);

		assert(fwrite(&p, sizeof(Point), 1, fh) == 1);
	}

	fclose(fh);
	in.close();
}

//#
void
LShashESE::iTransformDataSet(const char *_fin, const char *_fout) {
	FILE *fh = fopen(_fin, "rb");
	assert(fh != NULL);

	ofstream out(_fout);
	assert(out.is_open());

	Point p;
	while(fread(&p, sizeof(Point), 1, fh) == 1) {
		for(int i = 0; i < DIMS; ++i) {
			out << p.d[i];		
			if(i + 1 != DIMS)
				out << " ";
		}
		out << endl;
	}

	out.close();
	fclose(fh);
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
	int rows = Configer::get("rows").toInt();
	while(num < rows && fread(&p, sizeof(Point), 1, fh) == 1) {
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

void
LShashESE::storeLShash(const char *_if) {
	lsh.storeGhash(_if);
}

void
LShashESE::restoreLShash(const char *_if) {
	lsh.restoreGhash(_if);
}
