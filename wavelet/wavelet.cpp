
#include "wavelet.h"
#include <cmath>

double Wavelet::h[4];
double Wavelet::g[4];
static bool _nouse_ = Wavelet::initialize();

bool
Wavelet::initialize() {
	try {
	Wavelet wavelet;
	}catch(...) {
		return false;
	}
	return true;
}

Wavelet::Wavelet() {
	h[0] = (1. + sqrt(3.)) / (4 * sqrt(2.));
	h[1] = (3. + sqrt(3.)) / (4 * sqrt(2.));
	h[2] = (3. - sqrt(3.)) / (4 * sqrt(2.));
	h[3] = (1. - sqrt(3.)) / (4 * sqrt(2.));

	g[0] = h[3];
	g[1] = -h[2];
	g[2] = h[1];
	g[3] = -h[0];
}

Wavelet::~Wavelet() {
}

void
Wavelet::transform(const vector<double> &w, vector<double> &nw) {

	if(w.size() >= 4) {
		size_t len = w.size();
		size_t half = len >> 1;
		nw.resize(len);

		int i = 0;

		for(size_t j = 0; j < len-3; j += 2) {
			nw[i]      = w[j]*h[0] + w[j+1]*h[1] + w[j+2]*h[2] + w[j+3]*h[3];
			nw[i+half] = w[j]*g[0] + w[j+1]*g[1] + w[j+2]*g[2] + w[j+3]*g[3];
			i++;
		}
		nw[i] = w[len-2]*h[0] + w[len-1]*h[1] + w[0]*h[2] + w[1]*h[3];
		nw[i+half] = w[len-2]*g[0] + w[len-1]*g[1] + w[0]*g[2] + w[1]*g[3];
		++i;
		// cout << "i: " << i << endl;
	}
}

//# Never verify this function whether it's right.
void
Wavelet::itransform(const vector<double> &w, vector<double> &nw) {
	if(w.size() >= 4) {
		size_t len = w.size();
		size_t half = len >> 1;
		nw.resize(len);

		nw[0] = w[half-1]*h[0] + w[len-1]*h[1] + w[0]*h[2] + w[half]*h[3];
		nw[1] = w[half-1]*g[0] + w[len-1]*g[1] + w[0]*g[2] + w[half]*g[3];

		int j = 2;
		for(size_t i = 0; i < half-1; ++i) {
			nw[j++] = w[i]*h[0] + w[i+half]*h[1] + w[i+1]*h[2] + w[i+half+1]*h[3];
			nw[j++] = w[i]*g[0] + w[i+half]*g[1] + w[i+1]*g[2] + w[i+half+1]*g[3];
		}
	}
}

