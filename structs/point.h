
#ifndef LSH_ESE_POINT_H_XIAOE
#define LSH_ESE_POINT_H_XIAOE

#include <cmath>
#include "../utils/define.h"

//using namespace eoaix;

struct Point {
	//# identity to the external index id.
	u_int identity;

	double d[DIMS];

	/* Inner product, return the projected value. */
	double operator*(const Point &_p) {
		double res = 0;
		for(u_int i = 0; i < DIMS; ++i)
			res += _p.d[i] * d[i];
		return res;
	}
	double operator%(const Point &_p) {
		double dis = 0;
		for(size_t i = 0; i < DIMS; ++i) {
			dis += pow(d[i] - _p.d[i], 2);
		}
		return sqrt(dis);
	}
};

#endif
