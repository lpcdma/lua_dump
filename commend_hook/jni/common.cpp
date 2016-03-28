/*
* Description:common functions
* author: neve
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

#include "common.h"

//获取指定目录中最大size的文件的名称
void find_maxsize_file(char* libdir,char* &libso)
{
	DIR *pDir = NULL;
	struct dirent *dmsg;

	if((pDir = opendir(libdir)) != NULL)
	{
			unsigned long filesize = -1;
			while ((dmsg = readdir(pDir)) != NULL)
			{
				char libso_tmp[256];
				sprintf(libso_tmp,"%s/%s",libdir,dmsg->d_name);
				struct stat statbuff;

				if(stat(libso_tmp, &statbuff) < 0)
				{
					LOGD("Get file stat faile!");
					continue;
				}else
				{
					if(statbuff.st_size > filesize)
					{
						filesize = statbuff.st_size;
						strncpy(libso,libso_tmp,strlen(libso_tmp));
					}
				}
			}
			LOGD("library is :%s",libso);
	}else
	{
		LOGD("Open file from %s faile",libdir);
	}
}
