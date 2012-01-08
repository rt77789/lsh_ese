#include "lshash.h"

#include "../structs/searcher.h"
#include "../structs/candidate.h"
#include "../structs/bench.h"

#include <iostream>
#include <stdlib.h>

class LSHTuner {
	vector< float* > _points;
	typedef pair<double, double> Pair;
	typedef map<int, Pair> Mhash;
	Mhash _hash;

	double _time;

	pair<double, double> evaluate(LShash &lsh, Bench &bench) {
		vector< vector<u_int> > apro(_points.size());
		int rows = Configer::get("rows").toInt();
		size_t top_k = Configer::get("project_top_k").toInt();

		double cost = 0;
		std::cout << "_points.size(): " << _points.size() << std::endl;
		for(size_t i = 0; i < _points.size(); ++i) {
			vector<u_int> eid;
			Point p;
			for(int j = 0; j < DIMS; ++j)
				p.d[j] = _points[i][j];
			eoaix::Timer t;

			lsh.find(p, eid);
			cost += 1.0 * eid.size() / rows;

			vector<SearchRes> res;
			res.swap(Searcher::search(eid, _points[i]));

			_time += t.elapsed();

			for(size_t j = 0; j < res.size() && j < top_k; ++j) {
				apro[i].push_back(res[j].getID());
				/*
				   std::cout << "dis: " << res[j].getSim() << 
					" | id: " << res[j].getID() << std::endl;
					*/
			}
			//std::cout << string('-', 80) << std::endl;
		}
		double recall = bench.recall(apro);
		cost = cost / _points.size();
		return make_pair<double, double>(recall, cost);
	}


	void loadData() {
		int query_num = Configer::get("testset_query_num").toInt();
		//cout << "rows & testset_query_num: " << _dataset_rows << " | " << rows << endl;
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
		_time = 0;
		for(int i = 0; i < retry; ++i) {
			LShash lsh;
			lsh.init(param.K, param.prob, param.W, param.R);
			lsh.showStat();
			pair<double, double> res = evaluate(lsh, bench);
			recall += res.first;
			cost += res.second;
			std::cout << "aveEval[" << i << "]: "
				<< "recall: " << res.first
				<< " | cost: " << res.second
				<< " | time: " << _time
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
		double rightW = 0.3;
		const double eps = 0.0001;
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

	/* Calculate min W and max W. */
	void buildMinMaxW() {
		Configer::init("../all.config");
		loadData();

		Bench bench;
		bench.init();

		double prob = 0.9, R = 0.5;
		double min_prob = 0.2, max_prob = 0.9, step_prob;
		int min_K = 8, max_K = 20, step_K = 2;
		ofstream ow("k_w.map");
		for(int i = min_K; i <= max_K; i += step_K) {
			Parameter param(0, i, prob, R);
			double min = tuneW(min_prob, param, bench);	
			double max = tuneW(max_prob, param, bench);	

			_hash[i] = make_pair<double, double>(min, max);

			ow << "K: " << i <<
				" min: " << min << 
				" max: " << max << std::endl;
		}
		ow.close();
	}

	void loadMinMaxW() {
		ifstream iw("k_w.map");
		_hash.clear();
		string nk, nmin, nmax;
		int K;
		double min, max;
		while(iw >> nk >> K >>
				nmin >> min >>
				nmax >> max) {
			_hash[K] = make_pair<double, double>(min, max);
		}
		iw.close();
	}

	void run() {

		Configer::init("../all.config");
		loadMinMaxW();

		int min_K = 10, max_K = 10, step_K = 2;
		double min_prob = 0.2, max_prob = 0.9, step_prob = 0.1;

		double R = Configer::get("lsh_R").toDouble();
		double prob = Configer::get("lsh_prob").toDouble();

		int min_top_k = 10, max_top_k = 10, step_top_k = 1;
		//int min_M = 16, max_M = 20, step_M = 1;


		int min_rows = Configer::get("rows").toInt(), max_rows = min_rows, step_rows = 2;
		loadData();

		for(int r = min_rows; r <= max_rows; r *= step_rows) {

			int min_checks = max_rows, max_checks = r, step_checks = min_checks;
			std::string sr = eoaix::itoa(r, 10);
			Configer::set("rows", sr);
			for(int tk = min_top_k; tk <= max_top_k; tk+=step_top_k) {
				
				std::string stk = eoaix::itoa(tk, 10);
				Configer::set("project_top_k", stk);

				Bench bench;
				bench.init();


				cout << "tk(string): " << stk << " | " << "project_top_k: " << Configer::get("project_top_k").toString() << endl;

				for(int checks = min_checks; checks <= max_checks; checks += step_checks) {

					Configer::set("lsh_max_candidate", eoaix::itoa(checks, 10));
					for(int K = min_K; K <= max_K; K+=step_K) {
						Parameter param(0, K, prob, R);
						if(_hash.find(K) == _hash.end()) {
							buildMinMaxW();
						}

						Mhash::iterator iter = _hash.find(K);
						assert(iter != _hash.end());

						double min_W = iter->second.first;//0.0001;//tuneW(min_prob, param, bench);
						double max_W = iter->second.second;//0.03;//tuneW(max_prob, param, bench);

						double step_W = (max_W - min_W) / 100;

						int sw = 1;
						for(double W = min_W; W <= max_W; W+=step_W*sw) {
							sw += 1;
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
//	lt.buildMinMaxW();

	return 0;
}
