
#ifndef LSH_ESE_DEFINE_H_XIAOE
#define LSH_ESE_DEFINE_H_XIAOE

typedef unsigned int u_int;
typedef unsigned long long u64;

//#define DEBUG

#define LEVEL 3
//#define TOP_K 10
//# Maximum number of Points in memory.
#define IN_MEMORY_NUM 10000
//# Batch fread Points at one time.
#define BATCH_READ_NUM 1

#define DIMS 2048
#define FREQUENT 100

//# switch t0.
//#define T0XCORR

//#ifndef T0XCORR /* T0XCORR priority is higher than L2NORM. */
//#define L2NORM /* L2 metric for correlation. */
//#endif

/* Switch for normalize the input dataset and query or not. */
//#define NORMALIZE
#define INF 1<<31

#define TABLE_PRIME 17681 //((1LL<<13) - 1)
#define HASH_PRIME ((1LL<<32) - 5)

#define U_NUM_IN_G 2
//# Dimensions of a signal.


// The array for limit the top-k at different level.
const int levelLimit[] = {1<<30, 10, 10, 10};

/* EPS = 1e-32. */
const double zero_eps = 1e-32;

#endif
