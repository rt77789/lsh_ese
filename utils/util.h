
#ifndef LSH_ESE_UTIL_H_XIAOE
#define LSH_ESE_UTIL_H_XIAOE

#include <iostream>
#include <cmath>
#include <vector>
#include "define.h"
#include "../structs/point.h"
#include <ctime>

namespace eoaix {
	
	void getSacPath(const char *dir, std::vector<std::string> &sacs);
	void print_now();
	double sci2double(std::string sci);
	std::string itoa(int num, int base);

	void normalize(float *p); 
	void normalize(Point &p); 

	void readTest(std::vector<Point> &points); 

	/* Template function used for check equal. */
	template <typename T> void equalAssert(T a, T b) {
		if(a != b) {
			std::cerr << a << " != " << b << std::endl;
			throw ;
		}
	}
	/* specialize funtion must after generic function definition. */
	template <> void equalAssert(double a, double b);

	class Util {
		Util();
		~Util();
		/* Get the singleton instance. */
		static Util& instance();
		//# init Util.
		void init();
		public:
		//# random a float number by uniform distribution.
		static double randomByUniform(double left, double right);
		//# random a float number by Gaussian distribution.
		static double randomByGaussian();
		static u64 randomU64(u64 left, u64 right);
	};

	/* Just a Timer class. */
	class Timer {
		std::clock_t _start;
		std::clock_t _lapse;
		public:
			Timer() {
				reset();
			}

			void reset() {
				_lapse = 0;
				_start = std::clock();
			}

			double elapsed() {
				return double(std::clock() - _start) /CLOCKS_PER_SEC;	
			}
	};
}

#endif
