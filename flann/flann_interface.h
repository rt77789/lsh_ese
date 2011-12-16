
#ifndef LSH_ESE_FLANN_INTERFACE_H_XIAOE
#define LSH_ESE_FLANN_INTERFACE_H_XIAOE

#include "../utils/config.h"

#include <flann/flann.h>

#include "../structs/point.h"
#include <cstdio>
#include <iostream>

class FlannInterface {
	float *_dataset;
	struct FLANNParameters _param;
	flann_index_t _index_id;
	float _speedup;

	int _rows;
	int _dims;

	public:
	FlannInterface();
	~FlannInterface();

	/* initialize. */
	void init();

	/* Load dataset . */
	void load(const std::string &path);
	/* Set parameter. */
	void setParam(int trees, int leaf_max_size);
	/* Build index. */
	void buildIndex();
	/* save index. */
	void storeIndex(const std::string &path) const;
	/* restore index. */
	void restoreIndex(const std::string &path);
	/* find top-K ANN indexes. */
	void find(float *query, int K, std::vector<u_int> &result) ; 
	/*
	void findTest(int K);
	*/

	/* Get dataset. */
	const float* getData() ;
};

#endif

