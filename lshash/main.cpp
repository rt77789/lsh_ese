#include <iostream>
#include "ghash.h"

using namespace std;

int
main() {
	Util::init();
	Ghash::init(32, 32);
	u_int *uIndex = new u_int[2];

	return 0;
}
