#include <iostream>
#include "sac.h"

using namespace std;

void
read() {
	const char *file = "filt3.EZP.Z.SAC";
	SACHEAD *sHead = new SACHEAD();
	float *res = read_sac(file, sHead);
	if(res == NULL) {
		perror("read_sachead fail.");
		exit(0);
	}
	
	cout << sHead->npts << endl;
	for(int i = 0; i < sHead->npts; ++i) {
		cout << res[i] << ' ';
	}
	cout << endl;
}

int
main() {
	read();
	return 0;
}
