
#ifndef MPL_LSH_ESE_H_XIAOE
#define MPL_LSH_ESE_H_XIAOE

#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <boost/format.hpp>
#include <boost/timer.hpp>
#include <lshkit.h>

using namespace std;
using namespace lshkit;

namespace po = boost::program_options; 

class MPLSHash {
	typedef unsigned u_int;

	typedef MultiProbeLshIndex<u_int> Index;
	Index index;
	Index::Parameter param;
	DefaultRng rng;
	TopkScanner<FloatMatrix::Accessor, metric::l2sqr<float> > *scan; 
	float desire_recall;
	/* Top-K: K is the number of the retrivel answer. */
	u_int K;
	u_int L;
	/* R is the radius. */
	float R;

	FloatMatrix data;
	bool loadIndex;

	public:
	MPLSHash();
	~MPLSHash();

	void init(); 

	/* Load the dataset. */
	void load_data(const std::string &_path);
	/* Load index info. */
	void load_index(const std::string &_path);

	/* Store the index into file. */
	bool store(const std::string &_path);
	/* Restore the index from file. */
	bool restore(const std::string &_path);

	void query(float *_point, u_int _len, vector<u_int> &_res);
	/* Random sample test. */
	void test(u_int _topk);
	/* Normalize a point. */
	void normalize(float *_point);
};


#endif
