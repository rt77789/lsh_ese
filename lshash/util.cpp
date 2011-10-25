
#include "util.h"
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <cmath>
#include <cstdio>

void
Util::init() {
	srand(time(0));
}

double
Util::randomByUniform(double left, double right) {
	assert(left <= right);
	double res  = left + ((right - left) * (rand() * 1.0 / RAND_MAX));

	assert(res <= right && res >= left);
	return res;
}

double
Util::randomByGaussian() {
	//# Box-Muller transform to generate a point from normal.
	double r1 = 0, r2 = 0;
	do {
		r1 = randomByUniform(0.0, 1.0);
	} while(r1 == 0);
	r2 = randomByUniform(0.0, 1.0);
	double res = sqrt(-2.0 * log(r1)) * cos(2.0 * acos(-1.0) * r2);
	return res;
}

u64 
Util::randomU64(u64 left, u64 right) {
	u64 res = (u64)randomByUniform(left, right);
	return res;
}

void
Util::print_now() {
	time_t t = time(0); 
    char tmp[64]; 
    strftime(tmp, sizeof(tmp), "%Y/%m/%d %X %A",localtime(&t)); 
    puts(tmp);
}



/*
Real// Generate a random real from normal distribution N(0,1).
RealT genGaussianRandom(){
  // Use Box-Muller transform to generate a point from normal
  // distribution.
  RealT x1, x2;
  do{
    x1 = genUniformRandom(0.0, 1.0);
  } while (x1 == 0); // cannot take log of 0.
  x2 = genUniformRandom(0.0, 1.0);
  RealT z;
  z = SQRT(-2.0 * LOG(x1)) * COS(2.0 * M_PI * x2);
  return z;
}

*/
