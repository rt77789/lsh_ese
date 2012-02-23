
#ifndef LSH_ESE_SEARCH_RES_H_XIAOE
#define LSH_ESE_SEARCH_RES_H_XIAOE

#include "../utils/util.h"
#include <vector>

/* Search result class. */
class SearchRes {
	std::vector<double> signal;
	double sim;
	u_int id;
	int offset;

	public:
	SearchRes():offset(0) {}
	SearchRes(u_int _id, double _sim, const std::vector<double> &_signal): sim(_sim), id(_id), offset(0) {
		signal.assign(_signal.begin(), _signal.end());
	}
	~SearchRes() {}

	u_int getID() const { return id; }
	int getOffset() const { return offset; }
	double getSim() const { return sim; }
	std::vector<double>& getSignal() { return signal; }

	void setID(u_int _id) { id = _id; }
	void setOffset(int _offset) { offset = _offset; }
	void setSim(double _sim) { sim = _sim; }
	void setSignal(const std::vector<double> &_signal) { 
		signal.assign(_signal.begin(), _signal.end());
	}

	/* Compare function. */
	bool operator<(const SearchRes &_sr) const {
#ifdef L2NORM
		return sim < _sr.sim;
#else
		return sim > _sr.sim;
#endif
	}
};

#endif
