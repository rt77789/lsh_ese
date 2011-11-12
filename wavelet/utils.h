#ifndef WAVELET_UTILS_XIAOE
#define WAVELET_UTILS_XIAOE

#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

//#define DEBUG

#define LEVEL 3
#define K 10
//# Maximum number of Points in memory.
#define IN_MEMORY_NUM 10000
//# Batch fread Points at one time.
#define BATCH_READ_NUM 4

#define DIMS 4096
#define FREQUENT 100

//# switch t0.
#define T0XCORR
#define INF 1<<31

/*
class Const {
	public:
		static int 	LEVEL;
		static int	K;
		static int 	DIMS;
		static int 	FREQUENT;
		static int INF;
		static int levelLimit[32];
};
*/

// The array for limit the top-k at different level.
const int levelLimit[] = {1<<30, 8, 8, 8};

double	sci2double(string sci);
void	print_now();
void    getSacPath(const char *dir, vector<string> &sacs);
void	loadConfig(const char *cfile);

#endif
