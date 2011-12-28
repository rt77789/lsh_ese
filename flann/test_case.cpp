
#include "flann_interface.h"
#include <iostream>

using namespace std;

class TestFlann {

	public:
		static void test() {
			/* Init the config. */
			Configer::init("../all.config");
			FlannInterface flann;
			flann.init();
			size_t top_k = Configer::get("project_top_k").toInt();
			//flann.findTest(TOP_K);
			//vector<int> result;
			//flann.find(NULL, TOP_K, result);
			//flann.load(Configer::get("flann_dataset_path").toString());
			const float* dataset = flann.getData();
			int dims = Configer::get("dims").toInt();
			cout << "dims: " << dims << endl;
			for(int i = 0; i < 10; ++i) {
				float* testset = new float[dims];
				memcpy(testset, dataset + i*dims, dims*sizeof(float));
				vector<unsigned> result;
				flann.find(testset, top_k, result);
				if(testset != NULL) {
					delete[] testset;
				}
				for(size_t j = 0; j < top_k && j < result.size(); ++j) {
					cout << "TestFlann find result[" << j << "]: " << result[j] << endl;
				}
				cout << string(80, '-') << endl;
			}
		}
};

int main() {
	TestFlann::test();
	return 0;
}
