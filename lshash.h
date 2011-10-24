
#ifndef LSH_ESE_LSHASH_H_XIAOE
#define LSH_ESE_LSHASH_H_XIAOE

class LShash {
	public:
		LShash();
		~LShash();
		void findNodes(const Point &q, vector<identity> &eid);
		void addNode(const Point &q);
	private:
	//# Totally #L Ghash for index the dataset.
		vector<Ghash> g;
}

#endif
