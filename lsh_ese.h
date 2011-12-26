
#ifndef LSH_ESE_LSH_ESE_H_XIAOE
#define LSH_ESE_LSH_ESE_H_XIAOE

#include "mplshash/mplshash.h"
#include "flann/flann_interface.h"

#include <cstdio>
#include <vector>

#include "utils/util.h"

#include "structs/point.h"
#include "structs/search_res.h"

#include "lshash/lshash.h"
#include "lshash/ghash.h"

#include "fft/fft.h"

#include "wavelet/weps.h"


using namespace std;



class LShashESE {
	public:
		LShashESE();

		~LShashESE();

		//# find the top-k index of most similar signal.
		int findIndex(const vector<double> &sin, vector<SearchRes> &resig, const string &_lshtype);

		/* Read point from db according to eid and compute the correlation(L2 or xcorr); then return the SearchRes. */
		void queryDB(const vector<double> &sin, const vector<u_int> &eid, vector<SearchRes> &resig);

		//#
		void findByLSH(const vector<double> &sin, vector<u_int> &_index);

		//# Naive wavelet find the top-k index of most similar signal.
		int naiveWaveletFind(const vector<double> &sin, vector<SearchRes> &resig);
		
		//# Naive FFT-Convolution computing cross-correlation.
		int naiveFFTConvFind(const vector<double> &sin, vector<SearchRes> &resig);

		//# Store lshash.
		void storeLShash(const char *_if);

		//# Restore lshash.
		void restoreLShash(const char *_if);

		/* init. */
		void init(const string &type);
		
		//# Random a dataset.
		static void randomDataSet(const char *file, u_int _size);

		//# Transform dataset into binary format.
		static void transformDataSet(const char *_fin, const char *_fout);

		//# Inverted transform.
		static void iTransformDataSet(const char *_fin, const char *_fout); 

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
		MPLSHash mpl;
		FlannInterface flann;

		ifstream fhandle;
		//FILE *fhandle;
};
#endif
