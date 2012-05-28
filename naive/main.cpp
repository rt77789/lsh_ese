#include "../structs/searcher.h"
#include "../structs/candidate.h"
#include "../structs/bench.h"
#include "../utils/util.h"

#include <iostream>
#include <stdlib.h>

class NaiveTuner {
	vector< float* > _points;
	double _time;

	vector<double> evaluate(Bench &bench) {
		vector< vector<u_int> > apro(_points.size());

		double cost = 0;
		double time = 0;
		vector<double> tc;

		for(size_t i = 0; i < _points.size(); ++i) {
			vector<u_int> eid;
			eoaix::Timer t;

			cost += 1;

			vector<SearchRes> res;
			res.swap(Searcher::search(_points[i]));
			double ela = t.elapsed();
 
			_time += ela;
			tc.push_back(ela);

			for(size_t j = 0; j < res.size(); ++j) {
				apro[i].push_back(res[j].getID());
				std::cout << "j: " << j << " | sim: " << res[j].getSim() << std::endl;
			}
		}

		double mean_tc = 0;
		for(size_t i = 0; i < tc.size(); ++i) {
			mean_tc += tc[i];
		}
		mean_tc /= tc.size();
		double std_tc = 0;
		for(size_t i = 0; i < tc.size(); ++i) {
			std_tc += (tc[i] - mean_tc) * (tc[i] - mean_tc);
		}

		vector<double> res;
		res.push_back(mean_tc);
		res.push_back(std_tc);
		return res;
	}


	void loadData() {
		int query_num = Configer::get("testset_query_num").toInt();
		cout << "testset_query_num: " << query_num << endl;
		std::vector<Point> points;
		//points.swap(Candidate::get(eid));
		eoaix::readTest(points);
		assert(points.size() == (u_int)query_num);

		//cout << "candidate get over" << endl;
		_points.resize(query_num);
		for(int i = 0; i < query_num; ++i) {
			_points[i] = new float[DIMS];
			for(int j = 0; j < DIMS; ++j) {
				_points[i][j] = points[i].d[j];
			}
		}
	}

	public:
	void run() {
		Configer::init("../all.config");
		int min_top_k = Configer::get("project_top_k").toInt(), max_top_k = min_top_k, step_top_k = 1;

		//cout << "begin loadData()" << endl;
		loadData();
		int _dataset_rows = Configer::get("rows").toInt();
		int multi_file_rows = Configer::get("multi_file_rows").toInt();

		int min_rows = multi_file_rows, max_rows = _dataset_rows, step_rows = min_rows;
		int query_num = Configer::get("testset_query_num").toInt();

		for(int r = min_rows; r <= max_rows; r += step_rows) {
			std::string sr = eoaix::itoa(r, 10);
			Configer::set("rows", sr);
			for(int tk = min_top_k; tk <= max_top_k; tk += step_top_k) {
				string stk = eoaix::itoa(tk, 10);
				Configer::set("project_top_k", stk);

				eoaix::print_now();

				cout << "begin bench init()" << endl;
				_time = 0;
				eoaix::Timer time;
				Bench bench;
				bench.init();
				
				_time += time.elapsed();

				cout << "end bench init()" << endl;
				cout << "tk(string): " << stk << " | " << "project_top_k: " << Configer::get("project_top_k").toString() << endl;


				cout << "before evaluate: "; eoaix::print_now();
				vector<double> res = evaluate(bench);

				std::cerr << "rows: " << sr <<
					" | top_k: " << tk <<
					/*
					" = recall: " << res.first << 
					" - cost: " << res.second << 
					*/
					" - mean_tc: " << res[0] <<
					" - std_tc: " << res[1] << std::endl;
				eoaix::print_now();
			}
		}
	}
};
int main() {
	NaiveTuner tt;
	//ft.run();
	tt.run();
	return 0;
}
