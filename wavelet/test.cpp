#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string.h>
//Å¶ÏǷñ¿¼
int IS_DIR(const char* path)
{
	struct stat st;
	lstat(path, &st);
	return S_ISDIR(st.st_mode);
}
//±éÎ¼þ¼Ðeµݹ麯Ê
void List_Files_Core(const char *path, int recursive)
{
	DIR *pdir;
	struct dirent *pdirent;
	char temp[256];
	pdir = opendir(path);
	if(pdir)
	{
		while(pdirent = readdir(pdir))
		{
			//Ì¹ý".."
			if(strcmp(pdirent->d_name, ".") == 0
					|| strcmp(pdirent->d_name, "..") == 0)
				continue;
			sprintf(temp, "%s/%s", path, pdirent->d_name);
			// printf("%s\n", temp);
			//µ±tempΪĿ¼²¢ÇrecursiveΪ1µÄ±ºò鴦À×Ŀ¼
			if(IS_DIR(temp) && recursive)
			{
				List_Files_Core(temp, recursive);
			}
			else if(!IS_DIR(temp)) {
				int len = strlen(temp);	
				if(strcmp(temp + len - 4, ".SAC") == 0 || strcmp(temp + len - 4, ".sac") == 0) {
					printf("%s\n", temp);
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
void List_Files(const char *path, int recursive)
{
	int len;
	char temp[256];
	//ȥµô²µÄ/'
	len = strlen(path);
	strcpy(temp, path);
	if(temp[len - 1] == '/') temp[len -1] = '\0';

	if(IS_DIR(temp))
	{
		//´¦ÀĿ¼
		List_Files_Core(temp, recursive);
	}
	else   //Ê³öþ
	{
		printf("%s\n", path);
	}
}
int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Usage: ./program absolutePath\n");
		exit(0);
	}

	List_Files(argv[1], 1);

	return 0;
}
