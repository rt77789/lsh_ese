
#ifndef LSH_ESE_SEARCHER_H_XIAOE
#define LSH_ESE_SEARCHER_H_XIAOE

#include "search_res.h"
#include "candidate.h"
#include "point.h"
#include "../utils/util.h"
#include "../fft/fft.h"
#include <vector>
#include <algorithm>

class Searcher {
	std::vector<SearchRes> _res;
	Searcher() {}
	~Searcher() {}

	std::vector<SearchRes>& lsearch(const std::vector<u_int> &eid, const std::vector<double> &sin) {
		_res.clear();
		std::vector<Point> points;
		points.swap(Candidate::get(eid));
		for(size_t i = 0; i < points.size(); ++i) {
			std::vector<double> tin(points[i].d, points[i].d + DIMS);
			double sim = FFT::corr(tin, sin);
			_res.push_back(SearchRes(points[i].identity, sim, tin));
		}
		/* Sort the SearchRes objects according to their sims/distance, from low to high. */
		std::sort(_res.begin(), _res.end());
		return _res;
	}

	static Searcher& instance() {
		static Searcher searcher;
		return searcher;
	}

	public:
		/* Read candidates from files and sort them, finally return the results with SearchRes format. */
		static std::vector<SearchRes>& search(const std::vector<u_int> &eid, const std::vector<double> &sin) {
			return instance().lsearch(eid, sin);
		}
		static std::vector<SearchRes>& search(const std::vector<u_int> &eid, const float *in) {
			vector<double> sin(in, in + DIMS);
			return instance().lsearch(eid, sin);
		}
};

#endif
