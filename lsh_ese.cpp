#include "lsh_ese.h"
#include <sstream>
#include <fstream>
#include <algorithm>

LShashESE::LShashESE():fhandle(0) {
}

LShashESE::~LShashESE() {
	if(fhandle != NULL)
		fclose(fhandle);
}

void
LShashESE::init(const string &type) {
	if(type == "mpl") {
		string dataset = Configer::get("naive_dataset_path").toString();
		fhandle = fopen(dataset.c_str(), "rb");
		mpl.init();
	}
	else if(type == "lsh") {

		bool doLoadIndex = Configer::get("lsh_load_index").toBool();
		string dataset = Configer::get("lsh_dataset_path").toString();

		fhandle = fopen(dataset.c_str(), "rb");
		lsh.init();

		if(!doLoadIndex) {
			loadPoint();
			cout << "load Point over." << endl;
			bool doSave = Configer::get("lsh_do_save").toBool();
			if(doSave) {
				string indexPath = Configer::get("lsh_index_path").toString();
				storeLShash(indexPath.c_str());
			}
		}
		else {
			string indexPath = Configer::get("lsh_index_path").toString();
			restoreLShash(indexPath.c_str());
		}
	}
	else if(type == "flann") {
		string dataset = Configer::get("flann_dataset_path").toString();

		fhandle = fopen(dataset.c_str(), "rb");
		flann.init();
	}
	else if(type == "fft" || type == "wavelet") {
		string dataset = Configer::get("naive_dataset_path").toString();
		fhandle = fopen(dataset.c_str(), "rb");
	}
	else {
		throw ;
	}
	assert(fhandle != NULL);
}

//# Load point from external file and add into lsh object.
void
LShashESE::loadPoint() {
	assert(fhandle != NULL);
	Point p;
	for(u_int i = 0; readPoint(i, p) ; ++i) {
		//# sort p.d[].
		lsh.addNode(p);
	}
}

void
LShashESE::findByLSH(const vector<double> &sin, vector<u_int> &_index) {
	assert(sin.size() == DIMS);

	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	lsh.findNodes(q, _index);
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
		flann.find(fsin, TOP_K, eid);
		if(fsin != NULL)
		delete fsin;
	}
	else if(_lshtype == "lsh") {
		cout << "basic lsh find..." << endl;
		findByLSH(sin, eid);
	}
	else if(_lshtype == "fft") {
		cout << "fft find..." << endl;
		return naiveFFTConvFind(sin, resig);
	}
	else if(_lshtype == "wavelet") {
		cout << "wavelet naive find..." << endl;
		return naiveWaveletFind(sin, resig);
	}
	else {
		throw;
	}

	cout << "lsh.findNodes returns: eid.size() == " << eid.size() << endl;

	Point p;
	vector<pair<double, u_int> > xlist;

	//# sort eid, decreate disk move.
	sort(eid.begin(), eid.end());
	/*
	cout << "eid[]: ";
	for(size_t i = 0 ; i < eid.size(); ++i) {
		cout << eid[i] << " " ;
	}
	cout << endl;
	*/

	WaveletEps twe(sin);
	vector< vector<double> > vtin;
	vector<u_int> iden;

	for(u_int i = 0; i < eid.size(); ++i) {
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
	for(u_int i = 0; i < vwss.size() && i < TOP_K; ++i) {
		cout << "[" << i << "]: " << vwss[i].sim << " - index: " << vwss[i].id << endl;
		resig.push_back(SearchRes(vwss[i].id, vwss[i].sim, vwss[i].ws.wsig[vwss[i].ws.wsig.size()-1].sig));
	}
	return eid.size();
}

int
LShashESE::naiveFFTConvFind(const vector<double> &sin, vector<SearchRes> &resig) {
	assert(sin.size() == DIMS);

	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	vector<u_int> eid;
	//cout << "lsh.findNodes returns: eid.size() == " << eid.size() << endl;

	assert(0 == fseek(fhandle, 0LL, SEEK_SET));
	vector<SearchRes> xlist, rlist;

	int tnum = 0;

	Point p[BATCH_READ_NUM];

	int cpnum = 0;	

	while((cpnum = fread(p, sizeof(Point), BATCH_READ_NUM, fhandle)) > 0) {
		for(int i = 0; i < cpnum; ++i) {
			vector<double> tin(p[i].d, p[i].d + DIMS);
			//cout << "p.identity: " << p.identity << endl;
#ifdef T0XCORR
			double sim = FFT::t0xcorr(sin, tin);
#else
			double sim = FFT::xcorr(sin, tin);
#endif
			xlist.push_back(SearchRes(p[i].identity, sim, tin));
			if(xlist.size() >= IN_MEMORY_NUM) {
				//# merge
				rlist.insert(rlist.end(), xlist.begin(), xlist.end());
				sort(rlist.begin(), rlist.end());
				if(rlist.size() > IN_MEMORY_NUM) {
					rlist.resize(IN_MEMORY_NUM);
				}
				xlist.clear();
			}
		}
		tnum += cpnum;
	}
	if(xlist.size() > 0) {
		//# merge
		rlist.insert(rlist.end(), xlist.begin(), xlist.end());
		sort(rlist.begin(), rlist.end());
		if(rlist.size() > IN_MEMORY_NUM) {
			rlist.resize(IN_MEMORY_NUM);
		}
	}

	cout << "FFT total signals : " << tnum << endl;
	cout << "rlist.size() : " << rlist.size() << "-K: " << TOP_K << endl;


	/* clear the result buffer. */
	resig.clear();
	for(u_int i = 0; i < rlist.size() && i < TOP_K; ++i) {
		cout << "[" << i << "]: " << rlist[i].getSim() << " - id:" << rlist[i].getID() << endl;
		resig.push_back(rlist[i]);
	}
	return tnum;
}

int
LShashESE::naiveWaveletFind(const vector<double> &sin, vector<SearchRes> &resig) {
	assert(sin.size() == DIMS);
	Point q;
	for(u_int i = 0; i < sin.size(); ++i)
		q.d[i] = sin[i];

	WaveletEps twe(sin);

	assert(0 == fseek(fhandle, 0LL, SEEK_SET));

	int tnum = 0;

	vector< vector<double> > vtin;
	vector<u_int> iden;
	Point p[BATCH_READ_NUM];
	int cpnum = 0;

	while((cpnum = fread(&p, sizeof(Point), 1, fhandle)) > 0) {
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
	for(u_int i = 0; i < vwss.size() && i < TOP_K; ++i) {
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
	u64 blockSize = 10000000LL;
	u64 seekTimes = offset / blockSize;

	for(u64 i = 0; i < seekTimes; ++i) {
		if(i == 0) {
			assert(0 == fseek(fhandle, blockSize, SEEK_SET));
		}
		else {
			assert(0 == fseek(fhandle, blockSize, SEEK_CUR));
		}
	}

	if(seekTimes == 0)
		assert(0 == fseek(fhandle, offset % blockSize, SEEK_SET));
	else
	assert(0 == fseek(fhandle, offset % blockSize, SEEK_CUR));

	int rv = fread(&p, sizeof(Point), 1, fhandle);
	/*
	cerr << "offset: " << offset << " | rv: " << rv << " | blockSize: " << blockSize << " | remain: " << offset % blockSize << endl;
	*/
	assert(fhandle != NULL);
	if(rv > 0) {
		/*
		cout << "p.identity: " << p.identity << " | index: " << index << endl;
		*/
		assert(p.identity == index);
	}
	//assert(rv == 1);
	return rv == 1;
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
				p.d[j++] = sci2double(sd);	
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
	while(fread(&p, sizeof(Point), 1, fh) == 1) {
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
