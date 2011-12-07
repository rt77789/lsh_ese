#include "mplshash.h"

class TT {
	MPLSHash mpl;
	public:
	void init() {
		mpl.init("config");
	}
};


int main() {

	MPLSHash mpl;
	mpl.init("config");

//	mpl.test(10);

	return 0;
}
