
#include <iostream>
#include <vector>
#include "searcher.h"
#include "candidate.h"
#include "point.h"
#include "../utils/util.h"
#include "search_res.h"

using namespace std;

class TestSearcher {
	public :
		static void test() {
			Configer::init("../all.config");
			cout << "config over" << endl;
			vector<u_int> eid;
			for(int i = 99; i >= 0; --i) {
				eid.push_back(i);
			}
			vector<Point>& points = Candidate::get(eid);

			cout << "candidate get over" << endl;

			for(size_t i = 0; i < points.size(); ++i) {
				vector<double> sin(points[i].d, points[i].d + DIMS);
				vector<SearchRes>& res = Searcher::search(eid, sin);
				for(size_t j = 0; j < res.size(); ++j) {
					cout << "[" << j << "]: " << res[j].getSim() << " - index: " << res[j].getID() << endl;
				}
			}
		}
};

int main() {
	TestSearcher::test();	
	return 0;
}
