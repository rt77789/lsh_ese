
#ifndef LSH_ESE_WAVELET_H_XIAOE
#define LSH_ESE_WAVELET_H_XIAOE

#include <vector>
using namespace std;

class Wavelet {
	public:
		static void transform(const vector<double> &w, vector<double> &nw);
		static void itransform(const vector<double> &w, vector<double> &nw);
		static bool initialize();
	private:
		Wavelet();
		~Wavelet();
		static double h[4];
		static double g[4];
};

#endif
