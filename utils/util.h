
#ifndef LSH_ESE_UTIL_H_XIAOE
#define LSH_ESE_UTIL_H_XIAOE

#include <iostream>
#include <cmath>
#include <vector>
typedef unsigned int u_int;
typedef unsigned long long u64;


namespace eoaix {


	
	//#define DEBUG

#define LEVEL 3
#define TOP_K 10
	//# Maximum number of Points in memory.
#define IN_MEMORY_NUM 10000
	//# Batch fread Points at one time.
#define BATCH_READ_NUM 4

#define DIMS 4096
#define FREQUENT 100

//# switch t0.
//#define T0XCORR

#ifndef T0XCORR /* T0XCORR priority is higher than L2NORM. */
#define L2NORM /* L2 metric for correlation. */
#endif

/* Switch for normalize the input dataset and query or not. */
//#define DATA_NORMALIZE
#define INF 1<<31

#define TABLE_PRIME ((1LL<<13) - 1)
#define HASH_PRIME ((1LL<<32) - 5)

#define U_NUM_IN_G 2
	//# Dimensions of a signal.



	// The array for limit the top-k at different level.
	const int levelLimit[] = {1<<30, 10, 10, 10};

	void getSacPath(const char *dir, std::vector<std::string> &sacs);
	void print_now();
	double sci2double(std::string sci);

	/* Template function used for check equal. */
	template <typename T> void equalAssert(T a, T b) {
		if(a != b) {
			std::cerr << a << " != " << b << std::endl;
			throw ;
		}
	}
	/* specialize funtion must after generic function definition. */
	template <> void equalAssert(double a, double b);

	class Util {
		Util();
		~Util();
		/* Get the singleton instance. */
		static Util& instance();
		//# init Util.
		void init();
		public:
		//# random a float number by uniform distribution.
		static double randomByUniform(double left, double right);
		//# random a float number by Gaussian distribution.
		static double randomByGaussian();
		static u64 randomU64(u64 left, u64 right);
	};
}

#endif
