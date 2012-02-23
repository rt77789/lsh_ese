
#include <iostream>
#include <cmath>
#include "complex.h"
#include "fft.h"

using namespace std;

void
test_fft() {
	vector<Complex> x;
	vector<Complex> nx;
	x.push_back(Complex(-0.03480425839330703, 0));
	x.push_back(Complex(0.07910192950176387, 0));
	x.push_back(Complex(0.7233322451735928, 0));
	x.push_back(Complex(0.1659819820667019, 0));

	FFT::fft(x, nx);

	for(size_t i = 0; i < nx.size(); ++i) {
		cout << i << " : " << nx[i].real() << " - " << nx[i].imagin() << endl;
	}
	cout << string(80, '-') << endl;

	vector<Complex> y;
	FFT::ifft(nx, y);
	for(size_t i = 0; i < y.size(); ++i) {
		cout << i << " : " << y[i].real() << " - " << y[i].imagin() << endl;
	}

	cout << string(80, '-') << endl;

	FFT::cconvolve(x, x, nx);

	for(size_t i = 0; i < nx.size(); ++i) {
		cout << i << " : " << nx[i].real() << " - " << nx[i].imagin() << endl;
	}

	cout << string(80, '-') << endl;

	FFT::lconvolve(x, x, nx);

	for(size_t i = 0; i < nx.size(); ++i) {
		cout << i << " : " << nx[i].real() << " - " << nx[i].imagin() << endl;
	}
}

void
test_complex() {
	Complex a(1, 2);
	Complex b(2, 3);

	Complex c = a + b;
	cout << c.real() << " " << c.imagin() << endl;
	c = a * b;
	cout << c.real() << " " << c.imagin() << endl;
	c = a / b;
	cout << c.real() << " " << c.imagin() << endl;
	c = a - b;
	cout << c.real() << " " << c.imagin() << endl;

	//	size_t i = -8;
	//	cout << i << " " << (i / 2) << endl;
}

void
test_xcorr() {
	vector<Complex> x, y;
	x.push_back(Complex(1.2, 0));
	x.push_back(Complex(2.9, 0));
	x.push_back(Complex(0.123, 0));
	x.push_back(Complex(6.78, 0));

	y.resize(x.size());
	for(int i = (int) x.size() - 1; i >= 0; --i) {
		y[x.size() - 1 - i] = x[i];
	}
	vector<Complex> res;

	//# non-normalize...
	FFT::cconvolve(x, y, res);
	for(size_t i = 0; i < res.size(); ++i) {
		cout << i << " : " << res[i].real() << " - " << res[i].imagin() << endl;
	}
	cout << string(80, '-') << endl;

	FFT::lconvolve(x, y, res);
	for(size_t i = 0; i < res.size(); ++i) {
		cout << i << " : " << res[i].real() << " - " << res[i].imagin() << endl;
	}
	cout << string(80, '-') << endl;

	//# normalize...
	Complex mean;
	//# 
	Complex xx;
	for(size_t i = 0; i < x.size(); ++i) {
		mean = mean + x[i];	
	}
	mean = mean / Complex(x.size(), 0);
	cout << "mean: " << mean.real() << " " << mean.imagin() << endl;


	for(size_t i = 0; i < x.size(); ++i) {
		x[i] = (x[i] - mean);
		xx = xx + (x[i] * x[i]);
	}

	cout << "xx: " << xx.real() << " " << xx.imagin() << endl;

	y.resize(x.size());
	for(int i = (int) x.size() - 1; i >= 0; --i) {
		y[x.size() - 1 - i] = x[i];
	}

	FFT::cconvolve(x, y, res);
	for(size_t i = 0; i < res.size(); ++i) {
		res[i] = res[i] / xx;
		cout << i << " : " << res[i].real() << " - " << res[i].imagin() << endl;
	}
	cout << string(80, '-') << endl;

	FFT::lconvolve(x, y, res);
	for(size_t i = 0; i < res.size(); ++i) {
		res[i] = res[i] / xx;
		cout << i << " : " << res[i].real() << " - " << res[i].imagin() << endl;
	}
	cout << string(80, '-') << endl;

}
void
test_xcorr2() {
	vector<double> x;
	x.push_back((1));
	x.push_back((2));
	x.push_back((3));
	x.push_back((4));
	x.push_back((5));
	x.push_back((6));

	vector<double> y;
	y.resize(x.size());
	for(size_t i = 0; i < y.size(); ++i)
		y[i] = x[i] - 1; //sin(x[i] * x[i] * x[i]);

	pair<int, double> res = FFT::xcorr(x, y);

	cout << res.first << " " << res.second << endl;

}

int
main() {
	test_xcorr2();
	return 0;
}
