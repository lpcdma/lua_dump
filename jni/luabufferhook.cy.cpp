#include <android/log.h>
#include <substrate.h>
#include <stdio.h>
#include <string.h>

#define LOG_TAG "SUBhook"

#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void cigi_hook(void *orig_fcn, void* new_fcn, void **orig_fcn_ptr) {
	MSHookFunction(orig_fcn, new_fcn, orig_fcn_ptr);
}
MSConfig(MSFilterExecutable, "/system/bin/app_process")

int (*original_arc4random)(void);
int replaced_arc4random(void) {
	return 1234;
}

void* (*original_getAge)(int P, char *str, int size);
void* replaced_getAge(int P, char *str, int size) {
	if (strstr(str, "*")) {
		LOGI("* = %s", str);
		LOGI("* size = %d", size);
	}
	original_getAge(P, str, size);
}

void * get_base_of_lib_from_maps(char *soname) {
	void *imagehandle = dlopen(soname, RTLD_LOCAL | RTLD_LAZY);
	if (soname == NULL)
		return NULL;
	if (imagehandle == NULL) {
		return NULL;
	}
	uintptr_t * irc = NULL;
	FILE *f = NULL;
	char line[200] = { 0 };
	char *state = NULL;
	char *tok = NULL;
	char * baseAddr = NULL;
	if ((f = fopen("/proc/self/maps", "r")) == NULL)
		return NULL;
	while (fgets(line, 199, f) != NULL) {
		tok = strtok_r(line, "-", &state);
		baseAddr = tok;
		tok = strtok_r(NULL, "\t ", &state);
		tok = strtok_r(NULL, "\t ", &state); // "r-xp" field
		tok = strtok_r(NULL, "\t ", &state); // "0000000" field
		tok = strtok_r(NULL, "\t ", &state); // "01:02" field
		tok = strtok_r(NULL, "\t ", &state); // "133224" field
		tok = strtok_r(NULL, "\t ", &state); // path field

		if (tok != NULL) {
			int i;
			for (i = (int) strlen(tok) - 1; i >= 0; --i) {
				if (!(tok[i] == ' ' || tok[i] == '\r' || tok[i] == '\n'
						|| tok[i] == '\t'))
					break;
				tok[i] = 0;
			}
			{
				size_t toklen = strlen(tok);
				size_t solen = strlen(soname);
				if (toklen > 0) {
					if (toklen >= solen
							&& strcmp(tok + (toklen - solen), soname) == 0) {
						fclose(f);
						return (uintptr_t*) strtoll(baseAddr, NULL, 16);
					}
				}
			}
		}
	}
	fclose(f);
	return NULL;
}

void * get_base_of_lib_from_soinfo(char *soname) {
	if (soname == NULL)
		return NULL;
	void *imagehandle = dlopen(soname, RTLD_LOCAL | RTLD_LAZY);
	if (imagehandle == NULL) {
		return NULL;
	}
	char *basename;
	char *searchname;
	int i;
	void * libdl_ptr = dlopen("libdl.so", 3);
	basename = strrchr(soname, '/');
	searchname = basename ? basename + 1 : soname;
	for (i = (int) libdl_ptr; i != NULL; i = *(int*) (i + 164)) {
		if (!strcmp(searchname, (char*) i)) {
			unsigned int *lbase = (unsigned int*) i + 140;
			void * baseaddr = (void*) *lbase;
			return baseaddr;
		}
	}
	return NULL;
}

int count = 0;

void (*old_load_buffer)(void *,char *,size_t,char *);
void my_load_buffer(void *L,char *buff,size_t sz,char *name)
{
	if(name[strlen(name)-1] == 0x63
       &&name[strlen(name)-2] == 0x61
       &&name[strlen(name)-3] == 0x75
       &&name[strlen(name)-4] == 0x6c)
	{
		//LOGI("luaname is :%s---",name);
		int name_len = strlen(name);
		char *base_dir = (char *)"/sdcard/atm/";
		char *tail = (char *)".lua";
		char full_name[256];
		//"config/battleConf.luac"
		char *name_t = strdup(name);
		if(strstr(name,"LogoScene") != NULL)
		{
			LOGI("LogoScene:%s",buff);
		}
		int i = name_len;
		while(i>0)
		{
			if(name_t[i] == 0x2f)
			{
				LOGI("in--%s",(char *)&name_t[i+1]);
				sprintf(full_name,"%s%s%d%s",base_dir,(char *)&name_t[i+1],count,tail);
				LOGI("%s\n",full_name);
				count++;
				break;
			}
			i--;
		}
		FILE *fp = fopen(full_name,"w+");
		if(fp != NULL && sz != 0)
		{
			size_t len = 0;
			LOGI("sz1 is %d",sz);
			fwrite(buff,1,sz,fp);
			fclose(fp);
		}else
		{
			LOGI("open file fail!!!");
		}
		free(name_t);
	}
	LOGI("sz2 is %d",sz);
	LOGI("name is %s",name);
	old_load_buffer(L,buff,sz,name);
}

void* lookup_symbol(char* libraryname,char* symbolname)
{
	void *imagehandle = NULL;
	if (dlopen("*.so", RTLD_GLOBAL | RTLD_NOW) == NULL){
		LOGI("(lookup_symbol) *.so didn't work");
		return NULL;
	} else {
		imagehandle = dlopen(libraryname, RTLD_GLOBAL | RTLD_NOW);
		LOGI("(lookup_symbol) imagehandle addr = %p", imagehandle);
	}


	if (imagehandle != NULL){
		LOGI("(lookup_symbol) imagehandle111111 addr = %p", imagehandle);
		void * sym = dlsym(imagehandle, symbolname);
		if (sym != NULL){
			return sym;
			}
		else{
			LOGI("(lookup_symbol) dlsym didn't work");
			return NULL;
		}
	}
	else{
		LOGI("(lookup_symbol) dlerror: %s",dlerror());
		return NULL;
	}
}

MSInitialize {
	//cigi_hook((void *)arc4random,(void*)&replaced_arc4random,(void**)&original_arc4random);
	void* lib_base = get_base_of_lib_from_maps("*game.so");
//OR
//    void* lib_base = get_base_of_lib_from_soinfo("/data/app-lib/*/libtargetLib.so");

	LOGI("lib base is %p",lib_base);
	if (lib_base!=NULL) {
		void * getAgeSym = lib_base + 0x7D405C;
		LOGI("getAge() should be at %p. Let's hook it",getAgeSym);
		cigi_hook(getAgeSym,(void*)&replaced_getAge,(void**)&original_getAge);
	}

	//cigi_hook((void *)arc4random,(void*)&replaced_arc4random,(void**)&original_arc4random);libYvImSdk.so
		//void * getAgeSym1 = lookup_symbol(".so","cJSON_CreateTrue");
		//LOGI("(lpcdma) libYvImSdk: %p",getAgeSym1);
//	    void * getAgeSym = lookup_symbol("*libcocos2dlua.so","luaL_loadbuffer");
//	    LOGI("(lpcdma) libcocos2dlua: %",getAgeSym);
//	    cigi_hook(getAgeSym,(void*)&my_load_buffer,(void**)&old_load_buffer);
}
