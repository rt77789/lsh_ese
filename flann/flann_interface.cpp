
#include "flann_interface.h"
#include <fstream>


FlannInterface::FlannInterface():_dataset(0), _param(DEFAULT_FLANN_PARAMETERS), _index_id(0), _speedup(0), _rows(0), _dims(0) {
}

FlannInterface::~FlannInterface() {
	/* Free memory. */
	if(NULL != _dataset) {
		delete[] _dataset;
	}
	if(NULL != _index_id) {
		flann_free_index(_index_id, &_param);
	}
}

void FlannInterface::init() {
	//try {
	/* Get infos from configer. */
	_dims = Configer::get("dims").toInt();
	_rows = Configer::get("rows").toInt();

	std::string indexPath = Configer::get("flann_index_path").toString();
	std::string dataPath = Configer::get("flann_dataset_path").toString();

	bool doSave = Configer::get("flann_do_save").toBool();
	bool doIndex = Configer::get("flann_do_index").toBool();

	int trees = Configer::get("flann_kdtree_trees").toInt();
	int leaf_max_size = Configer::get("flann_kdtree_leaf_max_size").toInt();

	/*} catch(runtime_error e) {
	  throw "FlannInterface->Configer::get runtime_error.";
	  }
	 */

	std::cout << "Configer::get over." << std::endl;
	setParam(trees, leaf_max_size);

	std::cout << "setParam over." << std::endl;
	_dataset = new float[_dims * _rows];

	try {
		load(dataPath);
	} catch(std::runtime_error e) {
		throw "FlannInterface->load runtime_error.";
	}

	std::cout << "load over." << std::endl;
	if(doIndex) {
		try {
			buildIndex();
		}catch(std::runtime_error e) {
			throw "FlannInterface->buildIndex() runtime_error.";
		}
	}
	else {
		try {
			restoreIndex(indexPath);
		}catch(std::runtime_error e) {
			throw "FlannInterface->restoreIndex() runtime_error.";
		}
	}
	if(doSave) {
		try {
			storeIndex(indexPath);
		}catch(std::runtime_error e) {
			throw "FlannInterface->storeIndex() runtime_error.";
		}
	}

}


void FlannInterface::load(const std::string &path) {
	std::ifstream in(path.c_str());
	if(!in.is_open()) {
		throw path + " open failed...";
	}

	Point p;
	int cur_row = 0;

	while(in.read((char*)&p, sizeof(Point))) {
		for(int i = 0; i < _dims; ++i)
			_dataset[cur_row * _dims + i] = p.d[i];
		++cur_row;
	}
	if(cur_row != _rows) {
		throw "cur_row != _rows";
	}

	in.close();
}

void FlannInterface::setParam(int trees, int leaf_max_size) {
	_param.trees = trees;
	_param.leaf_max_size = leaf_max_size;
}

void FlannInterface::buildIndex() {
	_index_id = flann_build_index(_dataset, _rows, _dims, &_speedup, &_param);
}

void FlannInterface::storeIndex(const std::string &path) const {
	if(_index_id != NULL) {
		char *tmpPath = new char[path.size() + 1];
		strncpy(tmpPath, path.c_str(), path.size());
		tmpPath[path.size()] = 0;

		flann_save_index(_index_id, tmpPath);
	}
}

void FlannInterface::restoreIndex(const std::string &path) {
	if(_dataset != NULL) {
		delete[] _dataset;
	}
	_dataset = new float[_rows * _dims];

	char *tmpPath = new char[path.size() + 1];
	strncpy(tmpPath, path.c_str(), path.size());
	tmpPath[path.size()] = 0;

	_index_id = flann_load_index(tmpPath, _dataset, _rows, _dims);
	if(_index_id == NULL) {
		throw "load index : _index_id == NULL";
	}
}

void FlannInterface::find(float *query, int K, std::vector<u_int> &result) {
	float *dists = new float[K];
	int *res = new int[K];

	flann_find_nearest_neighbors_index(_index_id, query, 1, res, dists, K, &_param);

	result.assign(res, res + K);

	if(dists != NULL) {
		delete[] dists;
	}
	if(res != NULL) {
		delete[] res;
	}
}
/*
void FlannInterface::findTest(int K) {
	float *dists = new float[K*_rows];
	int *res = new int[K*_rows];
	flann_find_nearest_neighbors_index(_index_id, _dataset, _rows, res, dists, K, &_param);
	for(int i = 0;  i < _rows; ++i) {
		for(int j = 0; j < K; ++j) {
			std::cout << res[i*K + j] << " ";
		}
		std::cout << std::endl;
	}
	if(dists != NULL) {
		delete[] dists;
	}
	if(res != NULL) {
		delete[] res;
	}
}
*/
const float* FlannInterface::getData() {
	return _dataset;
}
