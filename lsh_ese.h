
#ifndef LSH_ESE_LSH_ESE_H_XIAOE
#define LSH_ESE_LSH_ESE_H_XIAOE

#include <cstdio>
#include <vector>
#include "lshash/util.h"
#include "lshash/point.h"
#include "lshash/lshash.h"
#include "wavelet/weps.h"
#include "wavelet/utils.h"


using namespace std;

class LShashESE {
	public:
		LShashESE(const char *file);
		LShashESE(const char *file, const char *_if);

		~LShashESE();

		//# find the top-k index of most similar signal.
		void findIndex(const vector<double> &sin, vector<u_int> &_index);
		//# Store lshash.
		void storeLShash(const char *_if);
		//# Restore lshash.
		void restoreLShash(const char *_if);
		//# Random a dataset.
		static void randomDataSet(const char *file, u_int _size);
		
		//# readDataSet and check whether it's right.
		static void readDataSet(const char *file);

		//# readDataSet into a vector<Point> object.
		static void readDataSet(const char *file, vector<Point> &p, u_int _size);

	private:
		//# Read a point from external index file.
		bool readPoint(u_int index, Point &p);
		//# load index into lsh.
		void loadPoint();

		LShash lsh;
		WaveletEps wavelet;
		string indexFile;
		FILE *fhandle;
};

#endif
