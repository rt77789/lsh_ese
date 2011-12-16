
#ifndef LSH_ESE_LSHASH_H_XIAOE
#define LSH_ESE_LSHASH_H_XIAOE

#include "../utils/util.h"
#include "ghash.h"

class LShash {
	public:
		LShash();
		~LShash();
		void init();
		//# Find nodes those are hashed into the same buckets with input query q, and eid is the result vecotr storing external index id.
		void findNodes(const Point &q, std::vector<u_int> &eid);
		//# Add a new Node into the LShash object.
		void addNode(const Point &q);
		//# Print the maximal bucket length.
		int getMaxBuckLen();
		//# Auto self tune the parameters, K & M.
		void tuneParameter();

		//# Store ghashes into file.
		void storeGhash(const char *_file);
		//# Restore ghashes from file.
		void restoreGhash(const char *_file);
	private:
		//# Estimate parameter M according to current k.
		int estimateParaM(int k, double prob);
		//# Totally #L Ghash for index the dataset.
		std::vector<Ghash> g;
		u_int K;
		int M;
		double prob;
};

#endif
