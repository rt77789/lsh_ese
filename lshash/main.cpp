#include "lshash.h"

#include "../structs/searcher.h"
#include "../structs/candidate.h"
#include "../structs/bench.h"

#include <iostream>
#include <stdlib.h>

class LSHTuner {
	vector< float* > _points;
	double _time;

	pair<double, double> evaluate(LShash &lsh, Bench &bench) {
		vector< vector<u_int> > apro(_points.size());
		int rows = Configer::get("rows").toInt();

		double cost = 0;
		for(size_t i = 0; i < _points.size(); ++i) {
			vector<u_int> eid;
			Point p;
			for(int j = 0; j < DIMS; ++j)
				p.d[j] = _points[i][j];
			eoaix::Timer t;

			lsh.find(p, eid);
			_time += t.elapsed();
			cost += 1.0 * eid.size() / rows;

			vector<SearchRes> res;
			res.swap(Searcher::search(eid, _points[i]));
			for(size_t j = 0; j < res.size(); ++j) {
				apro[i].push_back(res[j].getID());
			}
		}
		double recall = bench.recall(apro);
		cost = cost / _points.size();
		return make_pair<double, double>(recall, cost);
	}

	void loadData() {
		int query_num = Configer::get("testset_query_num").toInt();
		//cout << "rows & testset_query_num: " << _dataset_rows << " | " << rows << endl;
		vector<u_int> eid;
		for(int i = 0; i < query_num; ++i) 
			eid.push_back(i);
		std::vector<Point> points;
		points.swap(Candidate::get(eid));
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

	struct Parameter {
		Parameter(double w, int k, double p, double r):W(w), K(k), prob(p), R(r) {}
		double W;
		int K;
		double prob;
		double R;
	};

	pair<double, double> aveEval(Parameter &param, Bench &bench) {
		double recall = 0, cost = 0;
		int retry = Configer::get("lsh_random_vector_try").toInt();
		for(int i = 0; i < retry; ++i) {
			LShash lsh;
			lsh.init(param.K, param.prob, param.W, param.R);
			pair<double, double> res = evaluate(lsh, bench);
			recall += res.first;
			cost += res.second;
			std::cout << "aveEval[" << i << "]: "
				<< "recall: " << res.first
				<< " | cost: " << res.second
				<< std::endl;
		}
		/* Average the time elapse. */
		_time /= retry;
		recall /= retry;
		cost /= retry;
		return make_pair<double, double>(recall, cost);
	}
	double tuneW(double tw, Parameter &param, Bench &bench) {
		double leftW = 0;
		double rightW = 3;
		const double eps = 0.001;
		while(leftW + eps <= rightW)
		{
			param.W = (leftW + rightW) / 2;
			//_time = 0;
			pair<double, double> res = aveEval(param, bench);
			std::cout << "left: " << leftW 
				<< " | mid: " << param.W 
				<< " | right: " << rightW 
				<< " | recall: " << res.first 
				<< " | cost: " << res.second 
				<< std::endl;
			if(res.first > tw) {
				rightW = param.W - eps;
			}
			else {
				leftW = param.W + eps;
			}
		}
		return param.W;
	}

	public:
	void run() {

		Configer::init("../all.config");

		int min_K = 10, max_K = 10, step_K = 1;
		double min_prob = 0.2, max_prob = 0.9, step_prob = 0.1;

		double R = Configer::get("lsh_R").toDouble();
		double prob = Configer::get("lsh_prob").toDouble();

		int min_top_k = 10, max_top_k = 10, step_top_k = 1;
		//int min_M = 16, max_M = 20, step_M = 1;


		int min_rows = 100, max_rows = 100, step_rows = 2;
		loadData();

		for(int r = min_rows; r <= max_rows; r *= step_rows) {

			int min_checks = r / 10, max_checks = r, step_checks = min_checks;
			std::string sr = eoaix::itoa(r, 10);
			Configer::set("rows", sr);
			Bench bench;
			bench.init();

			for(int tk = min_top_k; tk <= max_top_k; tk+=step_top_k) {
				std::string stk = eoaix::itoa(tk, 10);
				Configer::set("project_top_k", stk);

				cout << "tk(string): " << stk << " | " << "project_top_k: " << Configer::get("project_top_k").toString() << endl;

				for(int checks = min_checks; checks <= max_checks; checks += step_checks) {

					Configer::set("lsh_max_candidate", eoaix::itoa(checks, 10));
					for(int K = min_K; K <= max_K; K+=step_K) {
						Parameter param(0, K, prob, R);
						double min_W = 0.001;//tuneW(min_prob, param, bench);
						double max_W = 0.03;//tuneW(max_prob, param, bench);

						double step_W = (max_W - min_W) / 10;


						for(double W = min_W; W <= max_W; W+=step_W) {
							param.W = W;	
							_time = 0;
							pair<double, double> res = aveEval(param, bench);
							std::cerr << "rows: " << sr << 
								" | top_k: " << tk <<
								" | checks: " << checks <<
								" | K: " << K << 
								" | W: " << W << 
								//" | checks: " << ck << 
								" | prob: " << prob <<
								" = recall: " << res.first << 
								" - cost: " << res.second <<
								" - time: " << _time << std::endl;
							//}

					}
				}
			}
			/**/
		}
	}
}

};
int
main() {
	//	Ghash::init(32, 32);
	//	u_int *uIndex = new u_int[2];
	LSHTuner lt;
	lt.run();

	return 0;
}
