#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string.h>

#include "util.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <cassert>

namespace eoaix {

	template<> void equalAssert(double a, double b) {
			if(fabs(a - b) > zero_eps) {
				std::cerr << a << " != " << b << std::endl;
				throw ;
			}
		}

	void print_now() {
		time_t t = time(0); 
		char tmp[64]; 
		strftime(tmp, sizeof(tmp), "%Y/%m/%d %X %A",localtime(&t)); 
		puts(tmp);
	}


	int isDir(const char* path)
	{
		struct stat st;
		lstat(path, &st);
		return S_ISDIR(st.st_mode);
	}

	void listFilesCore(std::vector<std::string> &v, const char *path, int recursive)
	{
		DIR *pdir;
		struct dirent *pdirent;
		char temp[256];
		pdir = opendir(path);
		if(pdir)
		{
			while((pdirent = readdir(pdir)))
			{
				//Ì¹ý".."
				if(strcmp(pdirent->d_name, ".") == 0
						|| strcmp(pdirent->d_name, "..") == 0)
					continue;
				sprintf(temp, "%s/%s", path, pdirent->d_name);
				// printf("%s\n", temp);
				//µ±tempΪĿ¼²¢ÇrecursiveΪ1µÄ±ºò鴦À×Ŀ¼
				if(isDir(temp) && recursive)
				{
					listFilesCore(v, temp, recursive);
				}
				else if(!isDir(temp)) {
					int len = strlen(temp);	
					if(strncmp(pdirent->d_name, "filt3", 5) == 0 && (strcmp(temp + len - 4, ".SAC") == 0 || strcmp(temp + len - 4, ".sac") == 0)) {
						printf("%s\n", temp);
						v.push_back(temp);
					}
				}
			}
		}
		else
		{
			printf("opendir error:%s\n", path);
		}
		closedir(pdir);
	}
	void listFiles(std::vector<std::string> &v, const char *path, int recursive)
	{
		int len;
		char temp[256];
		//ȥµô²µÄ/'
		len = strlen(path);
		strcpy(temp, path);
		if(temp[len - 1] == '/') temp[len -1] = '\0';

		if(isDir(temp))
		{
			//´¦ÀĿ¼
			listFilesCore(v, temp, recursive);
		}
		else   //Ê³öþ
		{
			printf("%s\n", path);
			v.push_back(path);
		}
	}

	std::string itoa(int num, int base) {
		std::string res = num < 0 ? "-" : "";
		num = abs(num);
		std::string tc;
		while(num) {
			tc += num % base + '0';	
			num /= base;
		}
		for(size_t i = 0; i < tc.size(); ++i) {
			res.push_back(tc[tc.size() - i - 1]);
		}
		if(res.size() == 0) {
			res = "0";
		}
		return res;
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

	/* Normalize the Point, maximum amplute set to 1. */
	void normalize(Point &p) {
#ifdef NORMALIZE
		double max = 0;
		for(int i = 0; i < DIMS; ++i) {
			max = fabs(p.d[i]) > max ? fabs(p.d[i]) : max;
		}
		//std::cout << "max: " << max << std::endl;
		if(fabs(max) > zero_eps) {
			for(int i = 0; i < DIMS; ++i) {
				p.d[i] /= max;
			}
		}
#endif
	}
	void normalize(float *p) {
#ifdef NORMALIZE	
		double max = 0;
		for(int i = 0; i < DIMS; ++i) {
			max = fabs(p[i]) > max ? fabs(p[i]) : max;
		}
		//std::cout << "max: " << max << std::endl;
		if(fabs(max) > zero_eps) {
			for(int i = 0; i < DIMS; ++i) {
				p[i] /= max;
			}
		}
#endif
	}

	// Recursive list all file names in the directory dir.
	void getSacPath(const char *dir, std::vector<std::string> &sacs) {
		listFiles(sacs, dir, 1);
	}

	void Util::init() {
		srand(time(0));
	}

	double Util::randomByUniform(double left, double right) {
		assert(left <= right);
		instance();
		double res  = left + ((right - left) * (rand() * 1.0 / RAND_MAX));

		assert(res <= right && res >= left);
		return res;
	}

	double Util::randomByGaussian() {
		instance();
		//# Box-Muller transform to generate a point from normal.
		double r1 = 0, r2 = 0;
		do {
			r1 = randomByUniform(0.0, 1.0);
		} while(r1 == 0);
		r2 = randomByUniform(0.0, 1.0);
		//double res = sqrt(-2.0 * log(r1)) * cos(2.0 * acos(-1.0) * r2);
		double res = sqrt(-2.0 * log(r1) / r1) * cos(2.0 * acos(-1.0) * r2);
		/* // Box-Muller transform 
		   return sigma * y * sqrt (-2.0 * log (r2) / r2); */
		return res;
	}

	u64 Util::randomU64(u64 left, u64 right) {
		instance();
		u64 res = (u64)randomByUniform(left, right);
		return res;
	}

	Util& Util::instance() {
		static Util util;
		return util;
	}

	Util::Util() {
		init();
	}
	Util::~Util() {
	}
}
