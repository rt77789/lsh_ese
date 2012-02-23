#include <iostream>
#include "sac.h"
#include <cstdlib>
#include <cstdio>

using namespace std;

void
read(const char *file) {
	//const char *file = "filt3.EZP.Z.SAC";
	SACHEAD *sHead = new SACHEAD();
	float *res = read_sac(file, sHead);
	if(res == NULL) {
		perror("read_sachead fail.");
		exit(0);
	}
	
/*	
	printf("delta: %f\nscale: %f\no: %f\na: %f\nb: %f\ne: %f\n",
			sHead->delta, sHead->scale, sHead->o, sHead->a, sHead->b, sHead->e);
	cout << sHead->npts << endl;
*/	
	for(int i = 0; i < sHead->npts && i < 2048; ++i) {
		cout << res[i] << ' ';
	}
	cout << endl;
}

int
main(int argc, char **argv) {
	if(argc < 2) {
		perror("argc <2");
	}
	read(argv[1]);
	return 0;
}
