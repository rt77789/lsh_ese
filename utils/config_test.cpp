
#include "util.h"
#include "config.h"

#include <cmath>
#include <fstream>

using namespace std;
using namespace eoaix;

class TestConfiger {
	public:
		static void testInit(const std::string &path) {
			/* write into a config file then read it. */
			ofstream out(path.c_str());
			if(out) {
				out << "key1=value1\n";
				out << "key2=123\n";
				out << "key3=1.23\n";
				out << "key4=true\n";
				out << "key5=false\n";
				out << " #key5=false  \n";
				out << "   \n";
				out << "   key6  =  value7 \n";
			}
			out.close();

			Configer::init(path);

			equalAssert(string("value1"), Configer::get("key1").toString());
			equalAssert(123, Configer::get("key2").toInt());
			equalAssert(1.23, Configer::get("key3").toDouble());
			equalAssert(true, Configer::get("key4").toBool());
			equalAssert(false, Configer::get("key5").toBool());
			equalAssert(string("value7"), Configer::get("key6").toString());
		}
};

int main() {
	TestConfiger::testInit("test_config");
}
