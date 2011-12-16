#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "sac_prep.h"

using namespace std;


PSac::PSac(const char *sfname) {
	// Load sac file according to sac file name.

	data = read_sac(sfname, &header);
	
	if(data == NULL) {
		perror("read_sac fail.");
		exit(0);
	}
	cout << header.npts << endl;
	/*
	
	for(int i = 0; i < header.npts; ++i) {
		cout << data[i] << ' ';
	}
	cout << endl;
	*/
}

void
PSac::data2vector(vector<double> &v) {
	for(int i = 0; i < header.npts; ++i) 
		v.push_back(data[i]);
}
