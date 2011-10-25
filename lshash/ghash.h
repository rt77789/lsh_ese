
#ifndef LSH_ESE_GHASH_H_XIAOE
#define LSH_ESE_GHASH_H_XIAOE


#include <vector>
#include "util.h"
#include "point.h"
#include "gnode.h"
using namespace std;


class Ghash {
	public:
		Ghash(u_int *_uIndex);
		~Ghash();

		//# Insert a new Node into Ghash object.
		void addNode(const Point &q);
		//# Find a Point in Ghash object.
		Gnode *findNode(const Point &q);
		//# Find all nodes hashed into buckets as same as q.
		void findNodes(const Point &q, vector<u_int> &eid);

		//# Get the maximal length of buckets in current Ghash object.
		u_int getMaxLen();

		//# static functions.
		//# initial all the fields.
		static void init(u_int _M, u_int _K);
		//# Random a point and store it in _p.
		static void randomPoint(Point &_p);
		//# Pre-compute fields used in next step(addNode & findNode).
		static void preComputeFields(const Point &q);
	private:
		//# cal h1 & h2 mask.
		pair<u64, u64> calh1Andh2(const Point &q);

		//# tables[x] is the head of some chain.
		Gnode* tables[TABLE_PRIME];
		u_int counter[TABLE_PRIME];
		u_int uIndex[U_NUM_IN_G];
		
		//# Global u hash function points.
		static vector< vector<Point> > uPoints;
		//# h1 & h2 hash function times uPoints tables with respect to current query q.
		static vector< u64 > h1TimesU[U_NUM_IN_G];
		static vector< u64 > h2TimesU[U_NUM_IN_G];

		//# projected hash values with respect to uPoints.
		static vector< vector<u64> > projectValue;
		//# h1 & h2 random points, used for hashing.
		static vector<u64> h1Points;
		static vector<u64> h2Points;

		//# All parameters used in Ghash.
		static u_int M;
		static u_int K;
		static double b;
		static double w;
};
#endif
