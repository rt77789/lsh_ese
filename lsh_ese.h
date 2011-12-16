
#ifndef LSH_ESE_LSH_ESE_H_XIAOE
#define LSH_ESE_LSH_ESE_H_XIAOE

#include "mplshash/mplshash.h"
#include "flann/flann_interface.h"

#include <cstdio>
#include <vector>

#include "utils/util.h"

#include "structs/point.h"

#include "lshash/lshash.h"
#include "lshash/ghash.h"

#include "fft/fft.h"

#include "wavelet/weps.h"


using namespace std;


/* Search result class. */
class SearchRes {
	vector<double> signal;
	double sim;
	u_int id;

	public:
	SearchRes() {}
	SearchRes(u_int _id, double _sim, const vector<double> &_signal):id(_id), sim(_sim) {
		signal.assign(_signal.begin(), _signal.end());
	}
	~SearchRes() {}

	u_int getID() const { return id; }
	double getSim() const { return sim; }
	vector<double>& getSignal() { return signal; }

	void setID(u_int _id) { id = _id; }
	void setSim(double _sim) { sim = _sim; }
	void setSignal(const vector<double> &_signal) { 
		signal.assign(_signal.begin(), _signal.end());
	}

	/* Compare function. */
	bool operator<(const SearchRes &_sr) const {
		return sim > _sr.sim;
	}
};


class LShashESE {
	public:
		LShashESE();

		~LShashESE();

		//# find the top-k index of most similar signal.
		int findIndex(const vector<double> &sin, vector<SearchRes> &resig, const string &_lshtype);

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

		FILE *fhandle;
};
#endif
