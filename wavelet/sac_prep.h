
/**
* sac file pre-process.
*/

#ifndef WAVELET_SAC_PREP_XIAOE
#define WAVELET_SAC_PREP_XIAOE

#include "sac.h"
#include <vector>
using namespace std;

// Self defined Sac struct.
class PSac {
	// Sac Header
	SACHEAD header;
	// Sac data.
	float  *data;
	
public:
	PSac(const char *sfname);
	~PSac();
	void data2vector(vector<double> &v);
};

#endif
