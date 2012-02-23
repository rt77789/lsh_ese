
#include "fft.h"
#include "../utils/util.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <cassert>
using namespace std;

//# FFT.
void
FFT::fft(const vector<Complex> &x, vector<Complex> &nx) {
	size_t len = x.size();
	nx.resize(len);
	
	if(len == 1) {
		nx[len-1] = x[len-1];
		return ;
	}

	if((len & 1)) {
		throw;
	}

	vector<Complex> even;
	even.resize(len >> 1);

	for(size_t i = 0; i < (len >> 1); ++i) {
		even[i] = x[i << 1];
	}
	vector<Complex> q;
	fft(even, q);

	vector<Complex> &odd = even;
	for(size_t i = 0; i < (len >> 1); ++i) {
		odd[i] = x[(i<<1) | 1];	
	}
	vector<Complex> r;
	fft(odd, r);

	for(size_t i = 0; i < (len >> 1); ++i) {
		double kth = -2.0 * i * acos(-1.0) / len;
		Complex wk(std::cos(kth), std::sin(kth));

		nx[i] = q[i] + (wk * r[i]);
		nx[i + (len >> 1)] = q[i] - (wk * r[i]);
	}
}

//# Inverted FFT.
void
FFT::ifft(const vector<Complex> &x, vector<Complex> &nx) {
	size_t len = x.size();
	vector<Complex> tx(len);

	for(size_t i = 0; i < len; ++i) {
		tx[i] = x[i].conjugate();
	}

	fft(tx, nx);

	for(size_t i = 0; i < len; ++i) {
		nx[i] = nx[i].conjugate().times(1.0 / len);
	}
}
//# Circular convolution.
void
FFT::cconvolve(const vector<Complex> &x, const vector<Complex> &y, vector<Complex> &res) {
	if(x.size() != y.size()) {
		throw;
	}

	size_t len = x.size();

	//# Pad x and y with 0s before this process, so that length is power of 2.

	vector<Complex> nx, ny;
	fft(x, nx);
	fft(y, ny);
	
	res.resize(len);

	for(size_t i = 0; i < len; ++i) {
		nx[i] = nx[i] * ny[i];
	}
	ifft(nx, res);
}

//# Linear convolution.
void
FFT::lconvolve(const vector<Complex> &x, const vector<Complex> &y, vector<Complex> &res) {
	vector<Complex> nx(x.begin(), x.end()), ny(y.begin(), y.end());
	
	if(x.size() != y.size()) {
		throw;
	}
	size_t len = x.size() << 1;
	nx.resize(len), ny.resize(len);

	for(size_t i = 0; i < x.size(); ++i) nx[i] = x[i], ny[i] = y[i];
	for(size_t i = x.size(); i < len; ++i) nx[i] = ny[i] = Complex(0, 0);

	cconvolve(nx, ny, res);
}

double FFT::corr(const vector<double> &x, const vector<double> &y) {

#ifdef L2NORM
	return l2norm(x, y);
	//return t0xcorr(x, y);
#else

#ifdef T0XCORR
	return t0xcorr(x, y);
#else
	pair<int, double> res = xcorr(x, y);
	return res.second;
#endif

#endif
	throw logic_error("corr logic error.");
}

/* Return distance of x and y in L2 norm metric. */
double FFT::l2norm(const vector<double> &x, const vector<double> &y) {
	if(x.size() != y.size() || x.size() == 0) {
		throw logic_error("in l2norm: x.size != y.size or x.size == 0");
	}
	double dis = 0;
	for(size_t i = 0; i < x.size(); ++i) {
		dis += (x.at(i) - y.at(i)) * (x.at(i) - y.at(i));
	}
	return dis < 0 ? 0 : std::sqrt(dis);
}

void FFT::normalize(vector<double> &sin) {
	//# Calculate mean of nx and ny.
	double mean = 0;
	size_t len = sin.size();
	if(len == 0) return;

	for(size_t i = 0; i < len; ++i) {
		mean += sin[i];
	}
	mean /= len;

	for(size_t i = 0; i < len; ++i) {
		sin[i] -= mean;
	}
	double var = 0;
	for(size_t i = 0; i < len; ++i) {
		var += sin[i] * sin[i];
	}
	var = sqrt(var);
	if(fabs(var) > zero_eps) {
		for(size_t i = 0; i < len; ++i)
			sin[i] /= var;
	}
}
//# cross-correlation.
pair<int, double>
FFT::xcorr(const vector<double> &x, const vector<double> &y) {
	//# It's normalized.

	vector<double> nx(x.begin(), x.end()), ny(y.begin(), y.end());
	size_t len = nx.size();

	if(nx.size() != ny.size() || len == 0) {
		throw;
	}

	//# padding nx and ny.
	size_t upper = 1;
	while(upper < len) upper <<= 1;
	nx.resize(upper, 0); ny.resize(upper, 0);
	len = upper;

	//# Calculate mean of nx and ny.
	double xmean = 0, ymean = 0;
	for(size_t i = 0; i < len; ++i) {
		xmean += nx[i], ymean += ny[i];
	}
	xmean /= len; ymean /= len;

	for(size_t i = 0; i < len; ++i) {
		nx[i] -= xmean; ny[i] -= ymean;
	}
	double xnorm = 0, ynorm = 0;
	for(size_t i = 0; i < len; ++i) {
		xnorm += nx[i] * nx[i]; ynorm += ny[i] * ny[i];
	}
	xnorm = sqrt(xnorm); ynorm = sqrt(ynorm);
	
	//# reverse ny.

	vector<Complex> cx, cy, res;
	cx.resize(len); cy.resize(len);
	for(size_t i = 0; i < len; ++i) {
		cx[i] = Complex(nx[i], 0);
		cy[i] = Complex(ny[len - 1 - i], 0);
	}

	cconvolve(cx, cy, res);	

	double maxcorr = -1e100;

	//# Attention, here 'len' is suitable?
	int mini = -1;
	for(size_t i = 0; i < len; ++i) {
		//cout << i << " [:] " << res[i].real() << " - " << res[i].imagin() << endl;
		if(res[i].real() > maxcorr) {
			maxcorr = res[i].real();
			mini = i;
		}
	}

	return make_pair<int, double>(len - mini - 1, maxcorr / (xnorm * ynorm));
}

pair<int, double> FFT::shift(const vector<double> &sa, const vector<double> &sb) {
	double ma = 0, mb = 0;

	// cout << sa.sig.size() << endl << sb.sig.size() << endl;
	assert(sa.size() == sb.size());

	int len = sa.size();

	for(int i = 0; i < len; ++i) {
		ma += sa[i];
		mb += sb[i];
	}

	ma /= len;
	mb /= len;

	double res = -INF;
	int offset = -INF;

	double deta = 0;
	double detb = 0;
	for(int i = 0; i < len; ++i) {
		deta += (sa[i] - ma) * (sa[i] - ma);
		detb += (sb[i] - mb) * (sb[i] - mb);
	}

	deta = sqrt(deta * detb);
	//# Make sure, it's fair for all resolutions.
	int eps = len;
	for(int dp = -eps + 1; dp < eps; ++dp) {
		// cout << "test" << endl;
		int d = dp;
		double num = 0;
		for(int i = 0; i < len; ++i) {
			num += (sa[i] - ma) * (sb[((i + d) % len + len) %len] - mb);
		}

		double rd = num / deta;
		if(rd > res) {
			res = rd;
			offset = d;
		}
	}
	return make_pair<int, double>(offset, res);
	//return offset;
}

//# override-above function.
pair<int, double>
FFT::xcorr(const double *x, const double *y, size_t len) {
	vector<double> nx, ny;
	nx.resize(len); ny.resize(len);
	for(size_t i = 0; i < len; ++i) {
		nx[i] = x[i]; ny[i] = y[i];
	}
	return xcorr(nx, ny);
}

//# T0 moment cross-correlation.
double
FFT::t0xcorr(const vector<double> &sa, const vector<double> &sb) {
	double ma = 0, mb = 0;

	if(sa.size() != sb.size()) {
		throw "sa.size() != sb.size()";
	}

	size_t len = sa.size();

	for(size_t i = 0; i < len; ++i) {
		ma += sa[i];
		mb += sb[i];
	}
	ma /= len;
	mb /= len;

	double res = -1e100;

	double deta = 0;
	double detb = 0;

	for(size_t i = 0; i < len; ++i) {
		deta += (sa[i] - ma) * (sa[i] - ma);
		detb += (sb[i] - mb) * (sb[i] - mb);
	}

	deta = sqrt(deta * detb);
	//# Make sure, it's fair for all resolutions.
	for(int dp = 0; dp < 1; ++dp) {
		// cout << "test" << endl;
		double num = 0;
		for(size_t i = 0; i < len; ++i) {
			num += (sa[i] - ma) * (sb[i] - mb);
		}

		double rd = num / deta;
		if(rd > res) {
			res = rd;
		}
	}
	// cout << "res: " << res << endl;
	return res;
}
