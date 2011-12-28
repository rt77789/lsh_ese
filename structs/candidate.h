
#ifndef LSH_ESE_CANDIDATE_H_XIAOE
#define LSH_ESE_CANDIDATE_H_XIAOE

#include "point.h"
#include <fstream>
#include <stdexcept>
#include <cassert>
#include <algorithm>
#include "../utils/config.h"

/* Candidate set used for read data from file according to eid(index id);
 * And compute the correlation(l2norm or xcorr) and sort by it. 
 * Finally return the result.
 */
class Candidate {
	typedef std::vector<Point> PointVector;
	PointVector _points;
	std::ifstream _in;

	Candidate() 
	try {
		std::string naive_dataset_path = Configer::get("naive_dataset_path").toString();	
		std::string dir = Configer::get("project_dir").toString();
		naive_dataset_path = dir + naive_dataset_path;
		_in.open(naive_dataset_path.c_str(), std::ios_base::binary);
		if(!_in.is_open()) {
			throw;
		}
		std::cout << "open " << naive_dataset_path << " over" << std::endl;
	}
	catch(std::exception e) {
		std::cerr << "Candidate() construct fail. " << e.what() << std::endl;
	}

	~Candidate() {
		if(_in.is_open()) {
			_in.close();
		}
	}

	static Candidate& instance() {
		static Candidate candi;
		return candi;
	}

	/* Read Points from file according to eid. */
	void read(const std::vector<u_int> &eid) {
		std::vector<u_int> index(eid.begin(), eid.end());
		/* Sort the identity, make sure it's from low to high. */
		std::sort(index.begin(), index.end());

		_in.clear(); /* clear the error bits. */
		_points.resize(index.size());

		u64 base = 0;	
		_in.seekg(base, std::ios_base::beg);
		assert(!(_in.fail() && _in.bad()));

		Point p;
		u_int rows = Configer::get("rows").toInt();

		for(size_t i = 0; i < index.size(); ++i) {
			if(index[i] >= rows) {
				std::cout << "[Attention]: index[i] >= rows - " << index[i] << " >= " << rows << std::endl;
				continue;
			}
			u64 offset = (u64)sizeof(Point) * index[i];
			if(i > 0) offset -= sizeof(Point);

			_in.seekg(offset - base, std::ios_base::cur);
			assert(!(_in.fail() && _in.bad()));

			_in.read((char*)&p, sizeof(Point));
			_points[i] = p;
			//std::clog << "p.identity: " << p.identity << " | index: " << index[i] << std::endl;
			assert(p.identity == index[i]);
			base = (u64)sizeof(Point) * index[i];
		}
	}
	PointVector& lget(const std::vector<u_int> &eid) {
		read(eid);
		return _points;
	}

	public:
	static PointVector& get(const std::vector<u_int> &eid) {
		return instance().lget(eid);
	}

};
#endif
