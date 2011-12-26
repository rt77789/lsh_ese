
#include <fstream>
#include "mplshash.h"

#include "../utils/config.h"

/* Constructor. */
MPLSHash::MPLSHash():scan(0), desire_recall(0.95), K(20), L(10), R(10000), loadIndex(false) {

	/*
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
		("load_index", po::value<bool>(&loadIndex)->default_value(false), "# load index?.")
		("dataset", po::value<string>(&dataset_path)->default_value("./dataset.in"), "# dataset.")
		("index", po::value<string>(&index_path)->default_value("./dataset.index"), "# dataset index file.")
		;
	po::variables_map vm;
	po::store(po::parse_config_file(is, desc, true), vm);
	po::notify(vm);

	*/

}

/* Deconstructor. */
MPLSHash::~MPLSHash() {
	if(scan != NULL) {
		delete scan;
	}
}


void MPLSHash::init(float W_, u_int M_, u_int T_, u_int L_, u_int Q_, u_int K_, float R_, u_int H_, float recall, u_int dims) {
	string testset_sample_path = Configer::get("testset_sample_path").toString();
	
	string dataset_path = Configer::get("mplsh_dataset_path").toString();
	string index_path = Configer::get("mplsh_index").toString();
	loadIndex = Configer::get("mplsh_load_index").toBool();

	u_int T = T_;
	u_int Q = Q_;
	u_int M = M_;
	u_int H = H_;
	u_int dim = dims;

	float W = W_;

	R = R_;
	desire_recall = recall;
	K = K_;
	L = L_;
	
	cout << "loadIndex: " << loadIndex << endl;

	param.W = W;
	param.range = H;
	param.repeat = M;
	param.dim = dim;

	load_data(dataset_path);
	load_index(index_path);
}

/* Initializer. */
void MPLSHash::init() {
	u_int T_ = Configer::get("mplsh_T").toInt();
	u_int Q_ = Configer::get("mplsh_Q").toInt();
	u_int M_ = Configer::get("mplsh_M").toInt();
	u_int H_ = Configer::get("mplsh_H").toInt();
	u_int dims = Configer::get("dims").toInt();

	float W_ = Configer::get("mplsh_W").toFloat();

	float R_ = Configer::get("mplsh_R").toFloat();
	float recall = Configer::get("mplsh_recall").toDouble();
	u_int K_ = Configer::get("mplsh_K").toInt();
	u_int L_ = Configer::get("mplsh_L").toInt();
	init(W_, M_, T_, L_, Q_, K_, R_, H_, recall, dims);
}

/* Load dataset. */
void MPLSHash::load_data(const std::string &_path) {
	data.load(_path);

	/* Normalize dataset. */
	for(int i = 0; i < data.getSize(); ++i) {
		normalize(data[i]);
	}

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

	if(!loadIndex || !restore(_path)) {
		cout << "restore failed..." << endl;
		DefaultRng rng;	
		index.init(param, rng, L);

		boost::progress_display progress(data.getSize());
		for(int i = 0; i < data.getSize(); ++i) {
			/* normalizing. */
			index.insert(i, data[i]);
			++progress;
		}

		cout << "store the index file..." << endl;
		assert(store(_path));
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
	if(!os) return false;
	os.exceptions(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
	index.save(os);
	os.close();
	return true;
}

/* Restore the index from file. */
bool MPLSHash::restore(const std::string &_path) {
	ifstream is(_path.c_str(), ios_base::binary);
	if(!is) return false;
	is.exceptions(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
	index.load(is);
	is.close();
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

	std::cout << "mplsh candidates number: " << scan->cnt() << std::endl;

	_res.clear();
	unsigned i = 0;
	for(typename std::vector<TopkEntry<u_int> >::const_iterator jj = topk.begin(); jj != topk.end() && i < scan->cnt(); ++jj, ++i) {
		_res.push_back(jj->key);
	}
}
