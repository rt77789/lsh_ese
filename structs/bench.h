
#ifndef LSH_ESE_BENCHMARK_H_XIAOE
#define LSH_ESE_BENCHMARK_H_XIAOE

#include <set>
#include <vector>
#include <fstream>
#include "point.h"
#include "../utils/util.h"
#include "searcher.h"
#include "search_res.h"
#include "candidate.h"

class Bench {
	vector< set<u_int> > _bench;
	vector< vector<double> > _e;
	int _rows;

	void save() {
		string path = Configer::get("project_dir").toString() + Configer::get("benchmark_path").toString();

		int file_num = Configer::get("rows").toInt() / Configer::get("multi_file_rows").toInt() - 1;

		path += "." + eoaix::itoa(file_num, 10);

		ofstream out(path.c_str());

		out << _bench.size() << endl;

		for(size_t i = 0; i < _bench.size(); ++i) {
			out << _bench[i].size();
			for(set<u_int>::iterator iter = _bench[i].begin(); iter != _bench[i].end(); ++iter) {
				out << " " <<  *iter;		
			}
			out << endl;
		}

		out << _e.size() << endl;

		for(size_t i = 0; i < _e.size(); ++i) {
			out << _e[i].size();
			for(size_t j = 0; j < _e[i].size(); ++j) {
				out << " " << _e[i][j];
			}
			out << endl;
		}

		out.close();
	}

	bool load() {
		try {
			string path = Configer::get("project_dir").toString() + Configer::get("benchmark_path").toString();
			int file_num = Configer::get("rows").toInt() / Configer::get("multi_file_rows").toInt() - 1;

			path += "." + eoaix::itoa(file_num, 10);

			ifstream in(path.c_str());
			if(!in.is_open()) return false;

			size_t len = 0;
			in >> len;
			_bench.resize(len);

			for(size_t j = 0; j < len; ++j) {
				set<u_int> su;
				size_t num;
				in >> num;
				for(size_t i = 0; i < num; ++i) {
					u_int temp;
					in >> temp;
					su.insert(temp);
				}
				_bench[j] = su;
			}
			in >> len;
			_e.resize(len);


			for(size_t i = 0; i < len; ++i) {
				vector<double> vd;
				size_t num = 0;
				in >> num;
				for(size_t j = 0; j < num; ++j) {
					double temp = 0;
					in >> temp;
					vd.push_back(temp);
				}
				_e[i] = vd;
			}

			in.close();
		} catch (exception e) {
			cout << "bench load fail..." << endl;
			return false;
		}
			cout << "bench load ok..." << endl;
		return true;
	}

	void doBench(const vector<Point> &points) {
		_bench.resize(points.size());
		_e.resize(points.size());

		size_t top_k = Configer::get("project_top_k").toInt();

		for(size_t j = 0; j < points.size(); ++j) {
			vector<SearchRes> res;

			vector<double> tin(points[j].d, points[j].d + DIMS);
			res.swap(Searcher::search(tin));
			std::cout << "end search" << std::endl;

			for(size_t i = 0; i < top_k && i < res.size(); ++i) {
				
				std::cout << "bench(dim): " << res[i].getSim() << 
			" | id: " << res[i].getID() << std::endl;
			
				_bench[j].insert(res[i].getID());
				_e[j].push_back(res[i].getSim());
			}
			std::cout << string(80, '-') << std::endl;
		}
	}

	public:
	void init() {
		int num = Configer::get("testset_query_num").toInt();
		int rows = Configer::get("rows").toInt();
		_rows = rows;
		if(rows < num) {
			std::cout << "[Attention]: testset_query_num > rows, set testset_query_num = rows." << std::endl;
		}

		/*
		vector<u_int> eid;
		for(int i = 0; i < num && i < rows; ++i)
			eid.push_back(i);
			*/
		std::vector<Point> points;
		//points.swap(Candidate::get(eid));
		eoaix::readTest(points);
		std::cout << " before doBench | "<< points.size() << std::endl;
		if(!Configer::get("load_benchmark_file").toBool() || !load()) {
			doBench(points);
			save();
		}
	}

	int getRows() {
		return _rows;
	}

	double recall(const vector< vector<double> > &apro) {
		double insect = 0;
		double total = 0;
		for(size_t i = 0; i < apro.size(); ++i) {
			double te = 0;
			int tt = 0;
			for(size_t j = 0; j < apro[i].size(); ++j) {
				//std::cout << apro[i][j] << " " << _e[i][j] << endl;
				if(fabs(_e[i][j]) > zero_eps) {
					te += (apro[i][j] - _e[i][j]) / _e[i][j];	
					++tt;
				}
			}

			if(tt > 0) {
				total += te / tt;
			}
			else {
				/* candidate < top_k. */
			}
		}
		return apro.size() > 0 ? total / apro.size() : total;	
	}

	double recall(const vector< vector<u_int> > &apro) {
		int insect = 0;
		double total = 0;
		double min_case = 1;
		double max_case = 0;

		for(size_t i = 0; i < apro.size(); ++i) {
			double si = 0;
			for(size_t j = 0; j < apro[i].size(); ++j) {
				if(_bench[i].find(apro[i][j]) != _bench[i].end()) {
					++insect;
					++si;
				}
			}
			total += _bench[i].size();
			double st = si / _bench[i].size();
			if(st < min_case) min_case = st;
			if(st > max_case) max_case = st;
		}
		std::cout << "min single case precision: " << min_case << std::endl;
		std::cout << "max single case precision: " << max_case << std::endl;
		return insect / total;	
	}
};
#endif
