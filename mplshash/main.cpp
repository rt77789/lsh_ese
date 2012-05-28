#include "mplshash.h"
#include "../structs/searcher.h"
#include "../structs/candidate.h"
#include "../structs/bench.h"
#include "../utils/util.h"

#include <iostream>
#include <stdlib.h>


/* Class used for tuning parameters of mplsh. */
class MPLTuner {
	vector< float* > _points;
	double _time;

	vector<double> evaluate(MPLSHash &mpl, u_int len, Bench &bench) {
		vector< vector<u_int> > apro(_points.size());
		int rows = Configer::get("rows").toInt();

		double cost = 0;
		vector<double> cosv;

		for(size_t i = 0; i < _points.size(); ++i) {
			vector<u_int> eid;

			eoaix::Timer t;
			mpl.query(_points[i], len, eid);

			cosv.push_back(1. * eid.size() / rows);
			cost += 1.0 * eid.size() / rows;

			vector<SearchRes> res;
			res.swap(Searcher::search(eid, _points[i]));
			_time += t.elapsed();
			for(size_t j = 0; j < res.size(); ++j) {
				apro[i].push_back(res[j].getID());
			}
		}
		pair<double, double> recall = bench.recall(apro);
		cost = cost / _points.size();

		double std = 0;
		for(size_t i = 0; i < cosv.size(); ++i) {
			std += (cost - cosv[i]) * (cost - cosv[i]);
		}
		vector<double> res;
		res.push_back(recall.first);
		res.push_back(recall.second);
		res.push_back(cost);
		res.push_back(std);
		return res;
		//return make_pair<double, double>(recall, cost);
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
		float W_;
		u_int M_;
		u_int T_;
		u_int L_;
		u_int Q_;
		u_int K_;
		float R_;
		u_int H_;
		float recall;
		u_int dims;

		Parameter(float W, u_int M, u_int T, u_int L, u_int Q, u_int K, float R, u_int H, float re, u_int d) {
			W_ = W, M_ = M, T_ = T, L_ = L, Q_ = Q, K_ = K, R_ = R, H_ = H, recall = re, dims = d;
		}
	};
	
	double tuneW(double tw, Parameter &param, Bench &bench) {
		double leftW = 0.0;
		double rightW = 10;
		const double eps = 1e-2;
		while(leftW + eps <= rightW)
		{
			param.W_ = (leftW + rightW) / 2;
			MPLSHash mpl;
			mpl.init(param.W_, param.M_, param.T_, param.L_, param.Q_, param.K_, param.R_, param.H_, param.recall, param.dims);
			vector<double> res = evaluate(mpl, param.dims, bench);

			std::cout << "left: " << leftW 
				<< " | mid: " << param.W_ 
				<< " | right: " << rightW 
				<< " | recall: " << res[0]
				<< " | std: " << res[1]
				<< " | cost: " << res[2]
				<< " | std: " << res[3]
				<< std::endl;
			if(res[0] > tw) {
				rightW = param.W_ - eps;
			}
			else {
				leftW = param.W_ + eps;
			}
		}
		return param.W_;
	}
	public:
	void run() {
		Configer::init("../all.config");

		/* prob 没有影响 .*/
		double min_prob = 0.2, max_prob = 0.9, step_prob = 0.1;
		//double recall = Configer::get("mplsh_recall").toDouble();
		//double min_W = 1.5, max_W = 2.5, step_W = 0.1;
		float R =  Configer::get("mplsh_Q").toInt();

		int min_top_k = 10, max_top_k = 10, step_top_k = 1;
		int min_M = 10, max_M = 20, step_M = 2;
		int min_rows = 100, max_rows = 100, step_rows = 2;

		u_int T = Configer::get("mplsh_T").toInt();
		u_int L = Configer::get("mplsh_L").toInt();
		u_int Q =  Configer::get("mplsh_Q").toInt();
		u_int K =  Configer::get("mplsh_K").toInt();
		u_int H =  Configer::get("mplsh_H").toInt();
		u_int dims =  Configer::get("dims").toInt();

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

				for(int M = min_M; M <= max_M; M+=step_M) {
						//for(double recall = min_prob; recall <= max_prob; recall += step_prob) 
					{
						double recall = 0;
							Parameter param(0, M, T, L, Q, K, R, H, recall, dims);
							double min_W = tuneW(min_prob, param, bench);
							double max_W = tuneW(max_prob, param, bench);
							double step_W = (max_W - min_W) / 10;
							for(double W = min_W; W <= max_W; W+=step_W) {
						//for(int ck = min_checks; ck <= max_checks; ck += step_checks) 
						//{
							MPLSHash mpl;
							mpl.init(W, M, T, L, Q, K, R, H, recall, dims);
							_time = 0;
							vector<double> res = evaluate(mpl, dims, bench);
							std::cerr << "rows: " << sr << 
								" | top_k: " << tk << 								" | M: " << M << 
								" | W: " << W << 
								//" | checks: " << ck << 
								" | prob: " << recall <<
								" = recall: " << res[0] << 
								" - std: " << res[1] <<
								" - cost: " << res[2] <<
								" - std: " << res[3] <<
								" - time: " << _time << std::endl;
						//}
						}
					}
				}
			}
		}
	}
};
int main() {
	MPLTuner mt;
	mt.run();
	return 0;
}
