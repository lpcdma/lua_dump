#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <jni.h>

#include "hooklib.h"
#include "common.h"

Hooklib hooklib;

void *addr_loadbuffer;

void (*old_load_buffer)(void *,char *,size_t,const char *); 
void my_load_buffer(void *L,char *buff,size_t sz,const char *name)
{ 
	int i = 0;
	if(strcmp(name,"config/battleConf.luac") == 0)
	{
		LOGI("luaname is :%s---",name);
		for(i;i<sz;i++)
		{
			if(buff[i] == 0x72
			   &&buff[i+1] == 0x4c
			   &&buff[i+2] == 0x65
			   &&buff[i+3] == 0x76
			   &&buff[i+4] == 0x65
			   &&buff[i+5] == 0x6c
			   &&buff[i+11] == 0x2c
			)
			{
				buff[i+9] = 0x30;
				buff[i+10] = 0x34;
			}
		}
	}
	old_load_buffer(L,buff,sz,name);
} 

extern "C" void hook_entry(char* parameters){

	addr_loadbuffer = hooklib.lookup_symbol((char *)"libcocos2dlua.so",(char *)"luaL_loadbuffer");

	hooklib.hook_wrapper((void *)addr_loadbuffer,(void *)&my_load_buffer,(void **)&old_load_buffer);
}
 
