
#ifndef LSH_ESE_UTIL_H_XIAOE
#define LSH_ESE_UTIL_H_XIAOE

#define TABLE_PRIME ((1LL<<13) - 1)
#define HASH_PRIME ((1LL<<32) - 5)
#define TOP_BUCKET_NUM 100
#define INF 1<<31

#define U_NUM_IN_G 2
//# Dimensions of a signal.
#define DIMS 1024

typedef unsigned int u_int;
typedef unsigned long long u64;


class Util {
	public:
		//# init Util.
		static void init();
		//# random a float number by uniform distribution.
		static double randomByUniform(double left, double right);
		//# random a float number by Gaussian distribution.
		static double randomByGaussian();
		static u64 randomU64(u64 left, u64 right);
		static void print_now();
};
#endif
