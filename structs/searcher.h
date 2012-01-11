
#ifndef LSH_ESE_SEARCHER_H_XIAOE
#define LSH_ESE_SEARCHER_H_XIAOE

#include "search_res.h"
#include "candidate.h"
#include "point.h"
#include "../utils/util.h"
#include "../fft/fft.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <cassert>

class Searcher {
	std::vector<SearchRes> _res;
	Searcher() {}
	~Searcher() {}

	std::vector<SearchRes>& lsearch(const std::vector<double> &sin) {
		std::string path = Configer::get("project_dir").toString() + Configer::get("naive_dataset_path").toString();
		std::ifstream in(path.c_str(), std::ios_base::binary);	
		assert(in.is_open());

		Point p;

		size_t cpnum = 0;	
		size_t topk = Configer::get("project_top_k").toInt();
		size_t rows = Configer::get("rows").toInt();

		//while((cpnum = fread(p, sizeof(Point), BATCH_READ_NUM, fhandle)) > 0) {
		while(!in.fail() && !in.eof() && cpnum < rows) {
			in.read((char*)&p, sizeof(Point));
			vector<double> tin(p.d, p.d + DIMS);
			//cout << "p.identity: " << p.identity << endl;
			double sim = FFT::corr(sin, tin);

			_res.push_back(SearchRes(p.identity, sim, tin));
			if(cpnum + 1 == topk) {
				make_heap(_res.begin(), _res.end());
			}
			else if(cpnum > topk) {
				pop_heap(_res.begin(), _res.end());
				_res.pop_back();
			}
			else {
			}
			++cpnum;
		}

		std::sort(_res.begin(), _res.end());
		if(_res.size() > topk) _res.resize(topk);
		in.close();
		return _res;
	}

	std::vector<SearchRes>& lsearch(const std::vector<u_int> &eid, const std::vector<double> &sin) {
		_res.clear();
		//points.swap(Candidate::get(eid));
		std::vector<u_int> index(eid.begin(), eid.end());
		std::sort(index.begin(), index.end());


		std::string path = Configer::get("project_dir").toString() + Configer::get("naive_dataset_path").toString();
		size_t rows = Configer::get("rows").toInt();

	std::ifstream _in(path.c_str(), std::ios_base::binary);
	assert(_in.is_open());
		
		size_t topk = Configer::get("project_top_k").toInt();

		u64 base = 0;
		_in.clear();
		_in.seekg(base, std::ios_base::beg);
		assert(!(_in.fail() && _in.bad()));

		Point p;
		for(size_t i = 0; i < index.size(); ++i) {
			if(index[i] >= rows) {
				std::cout << "[Attention]: index[i] >= rows in searcher. " << index[i] << " >= " << rows << std::endl;
				continue;
			}
			u64 offset = (u64)sizeof(Point) * index[i];
			if(i > 0) offset -= sizeof(Point);
			_in.seekg(offset - base, std::ios_base::cur);
			assert(!(_in.fail() && _in.bad()));
			_in.read((char*)&p, sizeof(Point));

			std::vector<double> tin(p.d, p.d + DIMS);

			double sim = FFT::corr(tin, sin);
			_res.push_back(SearchRes(p.identity, sim, tin));

			assert(p.identity == index[i]);
			base = (u64)sizeof(Point) * index[i];

			if(i + 1 == topk) {
				make_heap(_res.begin(), _res.end());
			}
			else if(i > topk) {
				pop_heap(_res.begin(), _res.end());
				_res.pop_back();

			}
		}
		/* Sort the SearchRes objects according to their sims/distance, from low to high. */
		std::sort(_res.begin(), _res.end());
		return _res;
	}

	static Searcher& instance() {
		static Searcher searcher;
		return searcher;
	}

	public:
		/* Read candidates from files and sort them, finally return the results with SearchRes format. */
		static std::vector<SearchRes>& search(const std::vector<u_int> &eid, const std::vector<double> &sin) {
			return instance().lsearch(eid, sin);
		}
		static std::vector<SearchRes>& search(const std::vector<u_int> &eid, const float *in) {
			vector<double> sin(in, in + DIMS);
			return instance().lsearch(eid, sin);
		}
		static std::vector<SearchRes>& search(const std::vector<double> &sin) {
			return instance().lsearch(sin);
		}
		static std::vector<SearchRes>& search(const float *in) {
			vector<double> sin(in, in + DIMS);
			return instance().lsearch(sin);
		}

};

#endif
