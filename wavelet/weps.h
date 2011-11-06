
#ifndef WAVELET_WEPS_XIAOE
#define WAVELET_WEPS_XIAOE

#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;


// Signal.
struct Signal {
	vector<double> sig;
	int ppos;
	
	void calPPos() {
		double squareSum = 0, linearSum = 0;
		
		for(size_t i = 0; i < sig.size(); ++i) {
			squareSum += sig[i] * sig[i];
			linearSum += sig[i];
		}
		int n = sig.size();
		double leftss = 0, leftls = 0;
		double mini_aci = 1e200;
		
		ppos = -1;
		
		double dom = n;
		
		for(int i = 0; i < n-1; ++i) {
			leftss += sig[i] * sig[i];
			leftls += sig[i];
			
			double leftVar = log( leftss / dom - pow(leftls/dom, 2) );
			double rightVar = log( (squareSum - leftss) / dom - pow( (linearSum - leftls) / dom, 2) );
			
			double cur_aci = leftVar * (i+1) + rightVar * (n - 2 - i);
			if(cur_aci < mini_aci) {
				mini_aci = cur_aci;
				ppos = i;
			}
		}
		// Maybe ppos == -1 means there's no P-wave.
		assert(ppos != -1);
	}
};
// Wavelet signal.
struct WaveletSignal {
	vector< Signal > wsig;
};

// WaveletSignal & Similar Structrue.
struct WSSimilar {
	WaveletSignal ws;
	int index;
	double sim;
	
	// Sort by sim, from high value to low.
	bool operator<(const WSSimilar &wss) const {
		return sim > wss.sim;
	}
};


class WaveletEps {
private:
	
	double h[4];
	double g[4];
	vector<double> query;
	WaveletSignal queryWS;
	vector<WSSimilar> sigs;
	string fileName;
	
	// Sort the signals by level.
	void sortByLevel(int level);
	
	// Merge tmpWSS with sigs, and copy back to the source sigs.
	void mergeWSS(vector<WSSimilar> &tmpWSS);
	
	// Just compute the corss-correlation of sa & sb.
	pair<double, int> cross_correlation(const Signal &sa, const Signal &sb) const;
	
	// Inserted Wavelet transforming.
	void insertWaveletTransform(vector<double> &sv) const;
	
	// Compute the similarity at layer 'layer', and sort.
	void findByLayer(const WaveletSignal &sin, int layer);
	
	// Loadin the data, and use limit to filter the top-K. 
	void loadin(const WaveletSignal &sin);
	
	void loadin();
	
public:
	// Find the most similar signal.
	vector<WSSimilar>& find(const vector<double> &sin);
	
	//# Insert a new signal into the waveletEps object.
	void addSignal(const vector<double> &sin, int _index);
	//# Clear sigs.
	void clear();
	
	//#
	void batch_push(const vector< vector<double> > &sin, const vector<int> &index);


	// Constructor.
	WaveletEps(const char *file);
	WaveletEps();
	WaveletEps(const vector<double> &q);
	void init();
	// vector<WSSimilar>& getWSSimilar();

	// Wavelet transforming.
	void waveletTransform(vector<double> &sv);
};

#endif
