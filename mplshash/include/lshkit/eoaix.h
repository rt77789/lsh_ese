
#ifndef LSHKIT_EOAIX_H
#define LSHKIT_EOAIX_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
/*
   */
namespace eoaix {
	class Plugin {
		typedef std::vector<float> Point;
		typedef std::vector<Point> PointVector;
		PointVector _test_sample;
		bool _is_init;
		int _retry_num;
		Plugin():_is_init(false) {}
		~Plugin() {}
		/* Load the test sample. */
		void loadTestSample(const std::string &path) {
			std::ifstream in(path.c_str());
			if(!in.is_open()) {
				throw std::runtime_error(path + " open fail.");
			}
			std::string line;
			while(getline(in, line)) {
				std::istringstream iss(line);
				Point p;
				std::string s;
				while(iss >> s) {
					double sd = sci2double(s);
					p.push_back(sd);
				}
				normalize(p);
				/*
				for(size_t i = 0; i < p.size(); ++i)
					std::cout << p[i] << " ";
				std::cout << std::endl;
				*/
				_test_sample.push_back(p);
			}
			_is_init = true;
		}

		static Plugin& instance() {
			//std::cout << "into instance()" << std::endl;
			static Plugin plugin;
			return plugin;
		}

		// Science number denoting -> double.
		double sci2double(std::string sci) {
			size_t i = sci.find('e');
			double res = 0;
			if(i != std::string::npos) {
				res = atof(sci.substr(0, i).c_str());
				res *= pow(10., atof(sci.substr(i+1).c_str()));
			}
			else {
				i = sci.find('E');
				if(i != std::string::npos) {
					res = atof(sci.substr(0, i).c_str());
					res *= pow(10., atof(sci.substr(i+1).c_str()));
				}
				else {
					res = atof(sci.c_str());
				}
			}

			return res;
		}
		double eval(const Point &rv, float b, float w) {
			typedef unsigned long long u64;
			typedef std::map<u64, std::vector<size_t> > Block;
			Block blockMap;

			std::cout << "before map.\n";

			for(size_t i = 0; i < _test_sample.size(); ++i) {
				double dis = 0;
				//std::cout << "_test_sample[i].size(): " << _test_sample[i].size() << " | rv.size(): " << rv.size() << std::endl;
				assert(rv.size() == _test_sample[i].size());
				for(size_t j = 0; j < _test_sample[i].size(); ++j) {
					dis += _test_sample[i][j] * rv.at(j);	
				}
				dis += b;
				u64 index = (u64)(dis / w);
				Block::iterator iter = blockMap.find(index);
				if(iter == blockMap.end()) {
					std::vector<size_t> np;
					np.push_back(i);
					blockMap[index] = np;
				}
				else {
					iter->second.push_back(i);
				}
			}
			std::cout << "after map.\n";

			double disAve = 0;
			const double eps = 1e-32;
			for(Block::iterator iter = blockMap.begin(); iter != blockMap.end(); ++iter) {
				double disSum = 0;
				int edgeNum = 0;
				//std::cout << "iter->second.size(): " << iter->second.size() << std::endl;
				for(size_t i = 0; i < iter->second.size(); ++i) {
					for(size_t j = i + 1; j < iter->second.size(); ++j) {
						double dis = 0;
						for(size_t k = 0; k < _test_sample[i].size(); ++k) {
							dis += (double)(_test_sample[iter->second[i]][k] - _test_sample[iter->second[j]][k]) * (_test_sample[iter->second[i]][k] - _test_sample[iter->second[j]][k]);;

						//std::cout << "dis: " << dis << " | " <<_test_sample[iter->second[i]][k] << " | " << _test_sample[iter->second[j]][k] << std::endl;
						}
						disSum += dis < eps ? 0 : sqrt(dis);
						//disSum += dis;
						++edgeNum;
					}
				}
				//std::cout << "disSum: " << disSum << " | edgeNum: " << edgeNum << " | disAve: " << disAve << std::endl;
				if(edgeNum > 0) {
					disAve += disSum / edgeNum;
				}
			}

			//std::cout << "disAve: " << disAve << " after disAve.\n";
			return disAve;
		}
		static void normalize(Point &p) {
			const double eps = 1e-32;
			double maxd = 0;
			//# Normalize the initial signal, point[] / max{ point[] }.
			for(int i = 0; i < p.size(); ++i)
				maxd += p[i];
			assert(p.size() > 0);
			maxd /= p.size();

			for(int i = 0; i < p.size(); ++i)
				p[i] -= maxd;
			double ts = 0;
			for(int i = 0; i < p.size(); ++i)
				ts += p[i] * p[i];
			ts = sqrt(ts);
			//std::cout << "ts: " << ts << std::endl;

			/* Make sure it's not 0 before dividing. */
			if(fabs(ts) > eps) {
				for(int i = 0; i < p.size(); ++i)
					p[i] = p[i] / ts;
			}
		}
		public:
		static void init(const std::string &path, int retry_num) {
			instance().loadTestSample(path);
			instance()._retry_num = retry_num;
		}
		static double evaluate(const Point &rv, float b, float w) {
			//std::cout << "into evaluate" << std::endl;
			return instance().eval(rv, b, w);	
		}
		static size_t getRetryNum() {
			return instance()._retry_num;
		}
	};
}
#endif
