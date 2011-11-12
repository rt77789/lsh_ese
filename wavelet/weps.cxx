
#include <vector>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "weps.h"
#include "utils.h"
#include "../fft/fft.h"

WaveletEps::WaveletEps() {
	// Initial the h[] & g[].
	init();
}

WaveletEps::WaveletEps(const vector<double> &q) {
	init();

	query.assign(q.begin(), q.end());

	vector<double> tmpSig(query);

	// Wavelet transforming: sin -> wsin.

	WaveletSignal ws;
	for(int i = 0; i < LEVEL; ++i) {

		Signal ts;
		ts.sig.assign(tmpSig.begin(), tmpSig.begin() + ((int)tmpSig.size() >> i));
		// Calculate the ppos of Signal.
		ts.calPPos();
		// cout << tmpSig.size() << endl;
		ws.wsig.push_back(ts);

		//insertWaveletTransform(tmpSig);
		waveletTransform(tmpSig);
	}

	for(size_t i = 0; i < ws.wsig.size() / 2; ++i) {
		swap(ws.wsig[i], ws.wsig[ws.wsig.size()-1-i]);
	}
	queryWS = ws;
}

void
WaveletEps::init() {
	h[0] = (1. + sqrt(3.)) / (4 * sqrt(2.));
	h[1] = (3. + sqrt(3.)) / (4 * sqrt(2.));
	h[2] = (3. - sqrt(3.)) / (4 * sqrt(2.));
	h[3] = (1. - sqrt(3.)) / (4 * sqrt(2.));
	
	g[0] = h[3];
	g[1] = -h[2];
	g[2] = h[1];
	g[3] = -h[0];
}

WaveletEps::WaveletEps(const char *file):fileName(file) {
	// Initial the h[] & g[].
	init();
	loadin();
}

//# Insert a new signal into the waveletEps object.
void
WaveletEps::addSignal(const vector<double> &sin, int _index) {
	// Wavelet transforming: sin -> wsin.
	vector<double> tmpSig(sin.begin(), sin.end());

	WaveletSignal ws;

	for(int i = 0; i < LEVEL; ++i) {
		
		Signal ts;
		ts.sig.assign(tmpSig.begin(), tmpSig.begin() + ((int)tmpSig.size() >> i));
		// Calculate the ppos of Signal.
		ts.calPPos();
		// cout << tmpSig.size() << endl;
		ws.wsig.push_back(ts);
		
		//insertWaveletTransform(tmpSig);
		waveletTransform(tmpSig);
	}
	
#ifdef DEBUG
	cout << "waveletTransforming over...\n";
#endif	
		
	for(size_t i = 0; i < ws.wsig.size() / 2; ++i) {
		swap(ws.wsig[i], ws.wsig[ws.wsig.size()-1-i]);
	}
	
	WSSimilar tmpWSSSig;
	tmpWSSSig.ws = ws;
	tmpWSSSig.sim = 0;
	tmpWSSSig.index = _index;
	sigs.push_back(tmpWSSSig);
}

//# Clear sigs.
void
WaveletEps::clear() {
	sigs.clear();
}

void
WaveletEps::loadin() {
	ifstream fin(fileName.c_str());
	assert(fin.is_open() == true);

	string line;
	//int line_num = 0;
	
	while(getline(fin, line)) {
		WaveletSignal ws;
		for(int i = 0; i < LEVEL; ++i) {
			if(getline(fin, line) == NULL) return;
			Signal sig;
			
			istringstream iss(line);
			iss >> sig.ppos;
			// Here is the 1e100 format, should convert into double.
			string sm;
			int num = 0;
			while(iss >> sm) {
				// Convert sm to double. sm is science number denoting.
				sig.sig.push_back(sci2double(sm));
				++num;
			}
			//cout << "num: " << num << endl;
			ws.wsig.push_back(sig);
		}
		
		WSSimilar tmpSig;
		tmpSig.ws = ws;
		tmpSig.sim = 0;
		sigs.push_back(tmpSig);
		//cout << "record_num: " << ++line_num << endl;
	}
	
	fin.close();
}



// Loadin the external data, and filter the top-K.
void
WaveletEps::loadin(const WaveletSignal &sin) {
	ifstream fin(fileName.c_str());
	assert(fin.is_open() == true);
	
	vector<WSSimilar> tmpSigs;

	string line;
	while(getline(fin, line)) {
		WaveletSignal ws;
		for(int i = 0; i < LEVEL; ++i) {
			if(getline(fin, line) == NULL) return;
			Signal sig;
			
			istringstream iss(line);
			iss >> sig.ppos;
			// Here is the 1e100 format, should convert into double.
			string sm;
			int num = 0;
			while(iss >> sm) {
				// Convert sm to double. sm is science number denoting.
				sig.sig.push_back(sci2double(sm));
				++num;
			}
			//cout << "num: " << num << endl;
			ws.wsig.push_back(sig);
		}
		pair<double, int> p = cross_correlation(ws.wsig[0], sin.wsig[0]);
		
		WSSimilar tmpSig;
		tmpSig.ws = ws;
		tmpSig.sim = p.first;
		tmpSigs.push_back(tmpSig);
		if(tmpSigs.size() >= K) {
			mergeWSS(tmpSigs);
			tmpSigs.clear();
		}
	}
	
	mergeWSS(tmpSigs);
	tmpSigs.clear();
	
	fin.close();
}

//# Batch push into WaveletEps object.
void
WaveletEps::batch_push(const vector< vector<double> > &vsin, const vector<int> &index) {
	vector<WSSimilar> vwss;
	for(size_t vi = 0; vi < vsin.size(); ++vi) {
		vector<double> tmpSig(vsin[vi]);

		// Wavelet transforming: sin -> wsin.

		WaveletSignal ws;
		for(int i = 0; i < LEVEL; ++i) {

			Signal ts;
			ts.sig.assign(tmpSig.begin(), tmpSig.begin() + ((int)tmpSig.size() >> i));
			// Calculate the ppos of Signal.
			ts.calPPos();
			// cout << tmpSig.size() << endl;
			ws.wsig.push_back(ts);

			//insertWaveletTransform(tmpSig);
			waveletTransform(tmpSig);
		}

		for(size_t i = 0; i < ws.wsig.size() / 2; ++i) {
			swap(ws.wsig[i], ws.wsig[ws.wsig.size()-1-i]);
		}

		try {
#ifdef T0XCORR
			double sim = FFT::t0xcorr(ws.wsig[0].sig, queryWS.wsig[0].sig);
#else
			double sim = FFT::xcorr(ws.wsig[0].sig, queryWS.wsig[0].sig);
#endif

			WSSimilar wss;
			wss.ws = ws;
			wss.index = index[vi];
			wss.sim = sim;
			vwss.push_back(wss);
		}catch(...) {
			cerr << "FFT::xcorr exception." << endl;
			throw;
		}
	}

	try {
		mergeWSS(vwss);	
	}catch(...) {
		cerr << "mergeWSS exception." << endl;
		throw;
	}
}

// Merge tmpWSS with sigs, and copy back to the source sigs.
void 
WaveletEps::mergeWSS(vector<WSSimilar> &tmpWSS) {
	// Sort the temporary WSSimilar vector.
	sort(tmpWSS.begin(), tmpWSS.end());
	// Merge tmpWSS to the  sigs.
	size_t i = 0, j = 0;
	vector<WSSimilar> resWSS;

	while(resWSS.size() < IN_MEMORY_NUM && (i < tmpWSS.size() || j < sigs.size()) ) {
		if(j >= sigs.size() || (i < tmpWSS.size() && tmpWSS[i].sim > sigs[j].sim)) {
				resWSS.push_back(tmpWSS[i++]);
		}
		else if (i >= tmpWSS.size() || (j < sigs.size() && sigs[j].sim >= tmpWSS[i].sim)) {
			resWSS.push_back(sigs[j++]);
		}
		else {
			perror("mergeWSS error.");
			exit(0);
		}
	}
	// Copy back to the source sigs.
	sigs.assign(resWSS.begin(), resWSS.end());
}

// Find the most similar signal.
vector<WSSimilar>&
WaveletEps::find(const vector<double> &sin) {
	vector<double> tmpSig(sin);
	
#ifdef DEBUG
	cout << "waveletTransforming now...\n";
#endif
	
	// Wavelet transforming: sin -> wsin.
	
	WaveletSignal ws;
	for(int i = 0; i < LEVEL; ++i) {
		
		Signal ts;
		ts.sig.assign(tmpSig.begin(), tmpSig.begin() + ((int)tmpSig.size() >> i));
		// Calculate the ppos of Signal.
		ts.calPPos();
		// cout << tmpSig.size() << endl;
		ws.wsig.push_back(ts);
		
		//insertWaveletTransform(tmpSig);
		waveletTransform(tmpSig);
	}
	
#ifdef DEBUG
	cout << "waveletTransforming over...\n";
#endif
	
		
	for(size_t i = 0; i < ws.wsig.size() / 2; ++i) {
		swap(ws.wsig[i], ws.wsig[ws.wsig.size()-1-i]);
	}
	
#ifdef DEBUG
	cout << "loadin now...\n";
	// loadin: Load and get the top-K most similar sigs useing LEVEL = 0, largest scale.
	//loadin(ws);
	cout << "loadin over...\n";
	
	cout << "findByLayer now...\n";
#endif
	// 3-layer cross-correlation.
	for(int i = 0; i < LEVEL; ++i) {
		// i = 0 already computed in the loadin process.
		findByLayer(ws, i);
	}
	
#ifdef DEBUG
	cout << "findByLayer over...\n";
#endif
	// return most similar Signal.
	// assert(sigs.size() > 0);
//	return sigs[0];
	return sigs;
}

// Wavelet transforming.
void
WaveletEps::waveletTransform(vector<double> &sv) {
	
	if(sv.size() >= 4) {
		int n = sv.size();
		int half = n >> 1;
		vector<double> tmp(n, 0);
		int i = 0;
		
		for(int j = 0; j < n-3; j += 2) {
			tmp[i]      = sv[j]*h[0] + sv[j+1]*h[1] + sv[j+2]*h[2] + sv[j+3]*h[3];
            tmp[i+half] = sv[j]*g[0] + sv[j+1]*g[1] + sv[j+2]*g[2] + sv[j+3]*g[3];
            i++;
		}
		tmp[i] = sv[n-2]*h[0] + sv[n-1]*h[1] + sv[0]*h[2] + sv[1]*h[3];
		tmp[i+half] = sv[n-2]*g[0] + sv[n-1]*g[1] + sv[0]*g[2] + sv[1]*g[3];
		++i;
		// cout << "i: " << i << endl;
		sv.assign(tmp.begin(), tmp.end());
	}
}

// Inserted Wavelet transforming.
void
WaveletEps::insertWaveletTransform(vector<double> &sv) const {
}

void
WaveletEps::findByLayer(const WaveletSignal &sin, int layer) {
	// Compute the cross-correlation of sin & every sigs at layer 'layer'.
	size_t upper = sigs.size();
	upper = upper < (size_t)levelLimit[layer] ? upper : (size_t)levelLimit[layer];
	
	for(size_t i = 0; i < upper; ++i) {
		//pair<double, int> p = cross_correlation(sigs[i].ws.wsig[layer], sin.wsig[layer]);
#ifdef T0XCORR
		double sim  = FFT::t0xcorr(sigs[i].ws.wsig[layer].sig, sin.wsig[layer].sig);
#else
		double sim  = FFT::xcorr(sigs[i].ws.wsig[layer].sig, sin.wsig[layer].sig);
#endif
		sigs[i].sim = sim;
	}
	sort(sigs.begin(), (size_t)levelLimit[layer] < sigs.size() ? (sigs.begin() + levelLimit[layer]) : sigs.end());
	// Erase/delete/clean the useless data.
	//assert(levelLimit[layer] < sigs.size());
	//if( levelLimit[layer] < (int)sigs.size())
	//sigs.erase(sigs.begin() + levelLimit[layer], sigs.end());
}
	

// Just compute the corss-correlation of sa an sb.
pair<double, int>
WaveletEps::cross_correlation(const Signal &sa, const Signal &sb) const {
	double ma = 0, mb = 0;

	// cout << sa.sig.size() << endl << sb.sig.size() << endl;
	assert(sa.sig.size() == sb.sig.size());

	int len = sa.sig.size();

	for(int i = 0; i < len; ++i) {
		ma += sa.sig[i];
		mb += sb.sig[i];
	}

	ma /= len;
	mb /= len;

	double res = -INF;
	int offset = -INF;

	double deta = 0;
	double detb = 0;
	for(int i = 0; i < len; ++i) {
		deta += (sa.sig[i] - ma) * (sa.sig[i] - ma);
		detb += (sb.sig[i] - mb) * (sb.sig[i] - mb);
	}

	deta = sqrt(deta * detb);
	//# Make sure, it's fair for all resolutions.
	int eps = len / 10;
	for(int dp = -eps; dp < eps; ++dp) {
		// cout << "test" << endl;
		int d = dp + abs(sa.ppos - sb.ppos);
		double num = 0;
		for(int i = 0; i < len; ++i) {
			num += (sa.sig[i] - ma) * (sb.sig[((i + d) % len + len) %len] - mb);
		}

		double rd = num / deta;
		if(rd > res) {
			res = rd;
			offset = d;
		}
	}
	// cout << "res: " << res << endl;
	return make_pair<double, int>(res, offset);
}
/*
vector<WSSimilar>&
WaveletEps::getWSSimilar() {
	return sigs;
}
*/

