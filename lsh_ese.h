
#ifndef LSH_ESE_LSH_ESE_H_XIAOE
#define LSH_ESE_LSH_ESE_H_XIAOE

#include <vector>

using namespace std;

class LShashESE {
	public:
		LShashESE(const char *file);
		~LShashESE();

		//# find the top-k index of most similar signal.
		void findIndex(const vector<double> &sin, vector<int> &index);

		//# Read a point from external index file.
		void readPoint(u_int index, Point &p);

	private:
		LShash lsh;
		WaveletEps wavelet;
		string indexFile;
		FILE *fhandle;
};

#endif
