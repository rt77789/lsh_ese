
#include <fstream>
#include "mplshash.h"

/* Constructor. */
MPLSHash::MPLSHash() {
	scan = NULL;
}

/* Deconstructor. */
MPLSHash::~MPLSHash() {
	if(scan != NULL) {
		delete scan;
	}
}

/* Initializer. */
void MPLSHash::init(const std::string &_config) {
	config(_config);
}

/* Load configuration. */
void MPLSHash::config(const std::string &_path) {
	string dataset_path, index_path;
	u_int T, Q, M, H;
	u_int dim;
	float W;
	ifstream is(_path.c_str(), std::ios::binary);
	po::options_description desc("Allowed options");
	desc.add_options()
		("W", po::value<float>(&W)->default_value(1.0), "# W.")
		("M", po::value<u_int>(&M)->default_value(1), "# M.")
		("T", po::value<u_int>(&T)->default_value(1), "# T.")
		("L", po::value<u_int>(&L)->default_value(1), "# L.")
		("Q", po::value<u_int>(&Q)->default_value(10), "# Q.")
		("K", po::value<u_int>(&K)->default_value(10), "# Top-K anwser.")
		("R", po::value<float>(&R)->default_value(10), "# radius.")
		("H", po::value<u_int>(&H)->default_value(1017881), "# hash table size.")
		("dim", po::value<u_int>(&dim)->default_value(1024), "# dimensionality.")
		("recall", po::value<float>(&desire_recall)->default_value(1.0), "# desire recall.")
		("dataset", po::value<string>(&dataset_path)->default_value("./dataset.in"), "# dataset.")
		("index", po::value<string>(&index_path)->default_value("./dataset.index"), "# dataset index file.")
		;
	po::variables_map vm;
	po::store(po::parse_config_file(is, desc, true), vm);
	po::notify(vm);

	/*
	   cout << desc;
	   cout << "W: " << W << endl;
	   cout << "M: " << M << endl;
	   cout << "T: " << T << endl;
	   cout << "L: " << L << endl;
	   cout << "Q: " << Q << endl;
	   cout << "K: " << K << endl;
	   cout << "R: " << R << endl;
	   cout << "H: " << H << endl;
	   cout << "desire_recall: " << desire_recall<< endl;
	   cout << "dataset : " << dataset_path << endl;
	   cout << "index: " << index_path << endl;
	 */

	param.W = W;
	param.range = H;
	param.repeat = M;
	param.dim = dim;

	load_data(dataset_path);
	load_index(index_path);
}

/* Load dataset. */
void MPLSHash::load_data(const std::string &_path) {
	data.load(_path);

	FloatMatrix::Accessor accessor(data);
	metric::l2sqr<float> l2sqr(param.dim);
	scan = new TopkScanner<FloatMatrix::Accessor, metric::l2sqr<float> >(accessor, l2sqr, K, R);
	if(scan == NULL) throw std::runtime_error("scan == NULL");
}

/* Normalize a point. */
void MPLSHash::normalize(float *_point) {
	float maxd = 0;
	for(u_int i = 0; i < param.dim; ++i) {
		maxd += _point[i];
	}
	maxd /= param.dim;
	for(u_int i = 0; i < param.dim; ++i)
		_point[i] -= maxd;
	double ts = 0;
	for(u_int i = 0; i < param.dim; ++i)
		ts += _point[i] * _point[i];
	ts = sqrt(ts);

	for(u_int i = 0; i < param.dim; ++i)
		_point[i] /= ts;
}

/* Load index file, if failed build new index and save. */
void MPLSHash::load_index(const std::string &_path) {

	/* Try to restore the index file, if failed build a new index and store it.  */
	cout << "Try to restore from index file..." << endl;
	if(!restore(_path)) {
		cout << "restore failed..." << endl;
		DefaultRng rng;	
		index.init(this->param, rng, L);

		boost::progress_display progress(data.getSize());
		for(int i = 0; i < data.getSize(); ++i) {
			/* normalizing. */
			normalize(data[i]);
			index.insert(i, data[i]);
			++progress;
		}

		cout << "store the index file..." << endl;
		store(_path);
	}
}

/* Random sample test. */
void MPLSHash::test(u_int _topk) {
	/* Just _topk random sample test. */
	boost::progress_display qp(_topk);
	int dsize = data.getSize();
	srand(time(0));

	for(u_int i = 0; i < _topk; ++i) {
		int index = rand() % dsize;
		// cout << "random index: " << index << endl;
		// scan->reset(data[index]);
		vector<u_int> res;
		query(data[index], param.dim, res);
		++qp;
	}
}

/* Store the index into file. */
bool MPLSHash::store(const std::string &_path) {
	ofstream os(_path.c_str(), ios_base::binary);
	if(!os.is_open()) return false;
	os.exceptions(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
	index.save(os);
	return true;
}

/* Restore the index from file. */
bool MPLSHash::restore(const std::string &_path) {
	ifstream is(_path.c_str(), ios_base::binary);
	if(!is.is_open()) return false;
	is.exceptions(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
	index.load(is);
	return true;
}

/* Query a point. */
void MPLSHash::query(float *_point, u_int _len, vector<u_int> &_res) {
	assert(_len == param.dim);
	assert(scan != NULL);
	scan->reset(_point);

	normalize(_point);
	index.query_recall(_point, desire_recall, *scan);
	// Key is unsigned.
	Topk<unsigned> topk = scan->topk();

	_res.clear();
	for(typename std::vector<TopkEntry<u_int> >::const_iterator jj = topk.begin(); jj != topk.end(); ++jj) {
		_res.push_back(jj->key);
	}
}
