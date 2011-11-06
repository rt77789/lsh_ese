#ifndef WAVELET_UTILS_XIAOE
#define WAVELET_UTILS_XIAOE

#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

//#define DEBUG

#define LEVEL 3
#define K 10
#define IN_MEMORY_NUM 10000
#define DIMS 1024
#define FREQUENT 100

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
const int levelLimit[] = {1<<30, 1000, 100, 10};

double	sci2double(string sci);
void	print_now();
void    getSacPath(const char *dir, vector<string> &sacs);
void	loadConfig(const char *cfile);

#endif
