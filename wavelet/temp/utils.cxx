#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string.h>
#include <cmath>
#include "utils.h"

int isDir(const char* path)
{
	struct stat st;
	lstat(path, &st);
	return S_ISDIR(st.st_mode);
}

void List_Files_Core(vector<string> &v, const char *path, int recursive)
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
				List_Files_Core(v, temp, recursive);
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
void List_Files(vector<string> &v, const char *path, int recursive)
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
		List_Files_Core(v, temp, recursive);
	}
	else   //Ê³öþ
	{
		printf("%s\n", path);
		v.push_back(path);
	}
}
		
// Science number denoting -> double.
double
sci2double(string sci) {
	size_t i = sci.find('e');
	double res = 0;
	if(i != string::npos) {
		res = atof(sci.substr(0, i).c_str());
		res *= pow(10., atof(sci.substr(i+1).c_str()));
	}
	else {
		i = sci.find('E');
		if(i != string::npos) {
			res = atof(sci.substr(0, i).c_str());
			res *= pow(10., atof(sci.substr(i+1).c_str()));
		}
		else {
			res = atof(sci.c_str());
		}
	}
	
	return res;
}
void
print_now() {
	time_t t = time(0); 
    char tmp[64]; 
    strftime(tmp, sizeof(tmp), "%Y/%m/%d %X %A",localtime(&t)); 
    puts(tmp);
}

// Recursive list all file names in the directory dir.
void
getSacPath(const char *dir, vector<string> &sacs) {
	List_Files(sacs, dir, 1);
}
