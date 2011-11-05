
#ifndef LSH_ESE_FFT_H_XIAOE
#define LSH_ESE_FFT_H_XIAOE

#include "complex.h"
#include <vector>
using namespace std;

class FFT {
	public:
		//# FFT transform.
		static void fft(const vector<Complex> &x, vector<Complex> &nx);
		//# Inverted FFt.
		static void ifft(const vector<Complex> &x, vector<Complex> &nx);
		//# circular convolution.
		static void cconvolve(const vector<Complex> &x, const vector<Complex> &y, vector<Complex> &res);

		//# linear convolution.
		static void lconvolve(const vector<Complex> &x, const vector<Complex> &y, vector<Complex> &res);
		static double xcorr(const vector<double> &x, const vector<double> &y);
		static double xcorr(const double *x, const double *y, size_t len);
};

#endif
