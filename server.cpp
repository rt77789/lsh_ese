#include "lsh_ese.h"
#include "lshash/ghash.h"

#include "utils/util.h"
#include "utils/config.h"
#include "wavelet/sac_prep.h"

#include "structs/bench.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;

class SearchEngineServer {

	LShashESE _lsese;

	/* Search from server and return the results. */
	void search(const vector<double> &sin, vector<SearchRes> &resig) {
		int candi = _lsese.findIndex(sin, resig, _indexType);
		/* Reorder by calculating cross-correlation. */
		for(u_int i = 0; i < resig.size(); ++i) {
			pair<int, double> fres = FFT::xcorr(sin, resig[i].getSignal());
			//pair<int, double> fres = FFT::xcorr(sin, resig[i].getSignal());

			//assert((fres.first % sin.size() + sin.size()) % sin.size() == xres.first);

			int offset = fres.first;
			double sim = fres.second;
			
			/**/
			resig[i].setSim(sim);
			/**/
			resig[i].setOffset(offset);
		}
		sort(resig.begin(), resig.end());
#ifdef L2NORM
		for(size_t i = 0; i*2 < resig.size(); ++i) {
			SearchRes tmp = resig[i];
			resig[i] = resig[resig.size() - 1 - i];
			resig[resig.size() - 1 - i] = tmp;
		}
#endif
	}

	void init() {
		_indexType = Configer::get("index_type").toString();
		_lsese.init(_indexType);
	}

	public:
	SearchEngineServer() {
		init();
	}
	~SearchEngineServer();

	/* Listen to the port and make a search action for each invoking. */
	void run() {
		
	}
};
