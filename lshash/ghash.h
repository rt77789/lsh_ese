
#ifndef LSH_ESE_GHASH_H_XIAOE
#define LSH_ESE_GHASH_H_XIAOE


#include <vector>
#include <cstdio>
#include <cassert>

#include "gnode.h"

#include "../utils/util.h"
#include "../structs/point.h"

using namespace std;
class Ghash {
	typedef vector<Point> PointVector;

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
	u_int getMaxBucketLen() const;
	/**/
	u_int getUnemptyNum() const;
	double getAveBucketLen() const;
	u_int getEntryNum() const;
	u_int getMedian() const;

	std::string showStat();

	//# static functions.
	//# initial all the fields.
	static void init(u_int M, u_int K, double W, double R);
	//# Random a point and store it in _p.
	static void randomPoint(Point &p);
	//# Pre-compute fields used in next step(addNode & findNode).
	static void preComputeFields(Point &q);
	//# Store static fields.
	static void storeStaticFields(FILE *fh);
	//# Restore static fields.
	static void restoreStaticFields(FILE *fh);
	/* Load testset sample. */
	static void loadTestSample(const string &path);
	/* Evaluate the random vector using testset sample path. */
	static double evaluateVector(const Point &p);
	/**/
	static void selectRandomVector(Point &cp);


	//# store object fields.
	void storeObjectFields(FILE *fh);
	//# Restore object fields.
	void restoreObjectFields(FILE *fh);
	private:
	/* Testset sample. */
	static PointVector testSample;
	//static void normalize(Point &p);
	//# cal h1 & h2 mask.
	pair<u64, u64> calh1Andh2(const Point &q);

	//# tables[x] is the head of some chain.
	Gnode* tables[TABLE_PRIME];
	u_int _unempty; /* Unempty table number. */
	u_int counter[TABLE_PRIME]; /* Buckuts number of each table. */
	u_int uIndex[U_NUM_IN_G];

	//# Global u hash function points.
	static vector<PointVector> uPoints;
	//# h1 & h2 hash function times uPoints tables with respect to current query q.
	static vector< u64 > h1TimesU[U_NUM_IN_G];
	static vector< u64 > h2TimesU[U_NUM_IN_G];

	//# projected hash values with respect to uPoints.
	static vector< vector<u64> > projectValue;
	//# h1 & h2 random points, used for hashing.
	static vector<u64> h1Points;
	static vector<u64> h2Points;

	//# All parameters used in Ghash.
	static u_int _M; /* U hash function number. */
	static u_int _K; /* Normal hash fucntion number, size of g(). */
	static double _b;/* offset of projection. */
	static double _W;/* interval length of projection. */
	static double _R;/* Radius. */

	static bool _use_uhash;

	/* Random vector whose element is selected from Gaussian distribution. */
	PointVector _randVector; /* Random vectors of current hash tables, totally #_K. */
};
#endif
