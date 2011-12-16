
#ifndef LSH_ESE_GNODE_H_XIAOE
#define LSH_ESE_GNODE_H_XIAOE

#include "../utils/util.h"

struct Gnode {
	u64 h2value;
	u_int identity;

	Gnode* next;
};

#endif
