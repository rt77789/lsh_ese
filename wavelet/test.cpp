#include <iostream>
#include "wavelet.h"
#include "weps.h"

using namespace std;

int
main() {
	vector<double> x, nx, y;
	x.push_back(0);
	x.push_back(1);
	x.push_back(2);
	x.push_back(3);
	x.push_back(4);
	x.push_back(5);
	x.push_back(6);
	x.push_back(7);
	for(size_t i = 0; i < x.size(); ++i)
		cout << x[i] << " ";
	cout << endl;

	Wavelet::transform(x, nx);
	for(size_t i = 0; i < nx.size(); ++i)
		cout << nx[i] << " ";
	cout << endl;

	WaveletEps we;
	we.waveletTransform(x);
	for(size_t i = 0; i < x.size(); ++i)
		cout << x[i] << " ";
	cout << endl;

	Wavelet::itransform(nx, y);
	for(size_t i = 0; i < y.size(); ++i)
		cout << y[i] << " ";
	cout << endl;

	return 0;
}
