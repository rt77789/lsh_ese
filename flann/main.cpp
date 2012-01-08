#include "flann_interface.h"
#include "../structs/searcher.h"
#include "../structs/candidate.h"
#include "../structs/bench.h"
#include "../utils/util.h"

#include <iostream>
#include <stdlib.h>

class FLANNTuner {
	vector< float* > _points;
	double _time;

	pair<double, double> evaluate(int checks, FlannInterface &_flann, Bench &bench) {
		int rows = Configer::get("rows").toInt();
		vector< vector<u_int> > apro(_points.size());

		double cost = 0;
		for(size_t i = 0; i < _points.size(); ++i) {
			vector<u_int> eid;
			eoaix::Timer t;
			_flann.find(_points[i], checks, eid);
			cost += 1.0 * eid.size() / rows;

			vector<SearchRes> res;
			res.swap(Searcher::search(eid, _points[i]));
			_time += t.elapsed();
			for(size_t j = 0; j < res.size(); ++j) {
				apro[i].push_back(res[j].getID());
				std::cout << "j: " << j << " | sim: " << res[j].getSim() << std::endl;
			}
		}

		double recall = bench.recall(apro);
		cost = cost / _points.size();
		return make_pair<double, double>(recall, cost);
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
		int min_trees = 4, max_trees = 4, step_trees = 1;
		int min_leafs = 4, max_leafs = 4, step_leafs = 1;
		int min_top_k = Configer::get("project_top_k").toInt(), max_top_k = min_top_k, step_top_k = 1;

		//cout << "begin loadData()" << endl;
		loadData();
		int _dataset_rows = Configer::get("rows").toInt();
		int min_rows = _dataset_rows / 10, max_rows = _dataset_rows, step_rows = min_rows;
		for(int r = min_rows; r <= max_rows; r += step_rows) {
			int min_checks = r / 100, max_checks = min_checks, step_checks = min_checks;

			std::string sr = eoaix::itoa(r, 10);
			Configer::set("rows", sr);
			cout << "begin bench init()" << endl;
			cout << "end bench init()" << endl;
			for(int tk = min_top_k; tk <= max_top_k; tk += step_top_k) {
				string stk = eoaix::itoa(tk, 10);
				Configer::set("project_top_k", stk);
				cout << "before bench.init(): ";
				eoaix::print_now();
				Bench bench;
				bench.init();
				eoaix::print_now();

				cout << "tk(string): " << stk << " | " << "project_top_k: " << Configer::get("project_top_k").toString() << endl;
				for(int i = min_trees; i <= max_trees; i+=step_trees) {
					for(int j = min_leafs; j <= max_leafs; j += step_leafs) {
						int ck = 1;
						for(int k = min_checks; k <= max_checks && k <= r; k += step_checks * ck) 
						{
							++ck;

							cout << "before flann.init(): ";
							eoaix::print_now();
							FlannInterface _flann;
							_flann.init(i, j, k);
							//cout << "flann init over" << endl;
							_time = 0;
							cout << "before evaluate: ";
							eoaix::print_now();
							pair<double, double> res = evaluate(k, _flann, bench);
							std::cerr << "rows: " << sr <<
								" | top_k: " << tk <<
								" | trees: " << i <<
								" | leafs: " << j <<
								" | checks: " << k << 
								" = recall: " << res.first << 
								" - cost: " << res.second << 
								" - time: " << _time << std::endl;
							eoaix::print_now();
						}
					}
				}
			}
		}
	}

};
int main() {
	FLANNTuner ft;
	ft.run();
	return 0;
}
