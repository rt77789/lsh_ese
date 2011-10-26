
#ifndef LSH_ESE_POINT_H_XIAOE
#define LSH_ESE_POINT_H_XIAOE

#include "util.h"

struct Point {
	//# identity to the external index id.
	u_int identity;

	double d[DIMS];

	u64 operator*(const Point &_p) {
		u64 res = 0;
		for(u_int i = 0; i < DIMS; ++i)
			res += _p.d[i] * d[i];
		return res;
	}
};

#endif
