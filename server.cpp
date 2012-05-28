#include "lsh_ese.h"
#include "lshash/ghash.h"

#include "utils/util.h"
#include "utils/config.h"
#include "utils/indexmapper.h"
#include "wavelet/sac_prep.h"

#include "structs/bench.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "lsh_ese.h"
#include "utils/indexmapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <assert.h>


using namespace std;

class SearchEngineServer {

	LShashESE _lsese;
	std::string _indexType;

	/* Search from server and return the results. */
	void search(const vector<double> &sin, vector<SearchRes> &resig) {
		int candi = _lsese.findIndex(sin, resig, _indexType);
		/* Reorder by calculating cross-correlation. */
#ifndef DEBUG
		for(u_int i = 0; i < resig.size(); ++i) {
			pair<int, double> fres = FFT::xcorr(sin, resig[i].getSignal());
			//pair<int, double> fres = FFT::xcorr(sin, resig[i].getSignal());

			//assert((fres.first % sin.size() + sin.size()) % sin.size() == xres.first);

			int offset = fres.first;
			double sim = fres.second;

			/**/
			resig[i].setSim(sim);
			/**/
			resig[i].setOffset(offset);
		}
		sort(resig.begin(), resig.end());
#endif
#ifdef L2NORM
		for(size_t i = 0; i*2 < resig.size(); ++i) {
			SearchRes tmp = resig[i];
			resig[i] = resig[resig.size() - 1 - i];
			resig[resig.size() - 1 - i] = tmp;
		}
#endif
	}

	void init() {

		IndexMapper::init(Configer::get("project_dir").toString() + Configer::get("index_mapper_path").toString());

		_indexType = Configer::get("index_type").toString();
		_lsese.init(_indexType);
	}

	public:
	SearchEngineServer() {
		init();
		server_run();
	}
	~SearchEngineServer() {
	}

//	void search(double *dbu, char *meches) {
//		/** format of return info.
//		  focal mechanize parameters | signal info. 
//		  xxxx.xxxx.xxxx.xxxx.xxxx.z | 0.1 0.2 ....
//		  32 bytes + 4096 * sizeof(double).
//		 */
//		vector<double> sin;
//		vector<SearchRes> resig;
//
//		sin.assign(dbu, dbu + DIMS);
//		search(sin, resig);
//		int block_size = 32 + DIMS * sizeof(double);
//
//		for(size_t i = 0; i < resig.size(); ++i) {
//			std::string param = IndexMapper::get(resig[i].getID());
//			char header[32] = {0};
//			assert(param.size() <= 32);
//			//strcmp(header, param.c_str());
//			strncmp(&meches[block_size * i], param.c_str(), 32);
//			strncmp(&meches[block_size * i + 32], param.c_str(), DIMS * sizeof(double));
//		}
//	}

	/* Listen to the port and make a search action for each invoking. */

	void server_run() {
		int portno = Configer::get("socket_port").toInt();
		const int BUFFER_SIZE = DIMS;
		double dbu[BUFFER_SIZE];
		char buffer[BUFFER_SIZE * sizeof(double)];
		sockaddr_in serverAddr;
		sockaddr &serverAddrCast = (sockaddr &) serverAddr;

		int sockfd = socket(AF_INET, SOCK_STREAM, 0);

		bzero(&serverAddr, sizeof(serverAddr));

		serverAddr.sin_family = AF_INET;

		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(portno);

		bind(sockfd, &serverAddrCast, sizeof(serverAddr));

		listen(sockfd, 5);
		const int block_size = 32 + DIMS * sizeof(double) + sizeof(double);
		char *block_content = new char[block_size];
		double *sig_info = new double[DIMS + 1];

		while(true) {
			int cfd = accept(sockfd, (sockaddr*) NULL, NULL);

			if(read(cfd, buffer, sizeof(double) * BUFFER_SIZE) < 0) {
				perror("server read returns < 0\n");
			}
			else {
				//		printf("%s\n", buffer);

				char2double(buffer, &dbu[0], BUFFER_SIZE);
				/*
				   for(int i = 0; i < BUFFER_SIZE; ++i)
				   printf("Recieve: %lf\n", dbu[i]);
				 */
				vector<double> sin;
				vector<SearchRes> resig;

				sin.assign(dbu, dbu + DIMS);
				search(sin, resig);

				/* resig to return format. */


				for(size_t i = 0; i < resig.size(); ++i) {
					std::string param = IndexMapper::get(resig[i].getID());
					assert(param.size() <= 32);
					//strcmp(header, param.c_str());
					strcpy(block_content, param.c_str());
					std::cout << "P[" << i << "]: " << block_content << std::endl;
					vector<double>& st = resig[i].getSignal();
					for(size_t j = 0; j < st.size(); ++j) {
						sig_info[j] = st[j];
					}
					sig_info[DIMS] = resig[i].getSim();
					double2char(block_content + 32, sig_info, DIMS + 1);
					//strncpy(block_content + 32, resig[i].getSignal(), DIMS * sizeof(double));
					/* write back to the client. */
					int n = write(cfd, block_content, block_size);
					if(n != block_size) {
						perror("write n != block_size\n");
					}
				}
			}

			shutdown(cfd, 2);
			close(cfd);
		}

		if(block_content != NULL) {
			delete[] block_content;
		}
	}
	void char2double(char* ch, double *dh, int size) {
		//double dbu[3] = {0.1, 0.2, 0.3};
		char *tp = (char*)&ch[0];
		char *td = (char*)&dh[0];

		for(int i = 0; i < size; ++i) {
			for(int j = 0; j < sizeof(double); ++j) {
				*td++ = *tp++;
			}
		}
	}
	void double2char(char* ch, double *dh, int size) {
		char *tp = (char*)&ch[0];
		char *td = (char*)&dh[0];

		for(int i = 0; i < size; ++i) {
			for(int j = 0; j < sizeof(double); ++j) {
				*tp++ = *td++;
			}
		}
	}


};

int main() {
	Configer::init("all.config");
	SearchEngineServer ses;

	return 0;
}
