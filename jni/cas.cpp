#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <jni.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h> 
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// #include <android/log.h>

#include "fake_dlfcn.h"

#if defined(__aarch64__)
#include "And64InlineHook.hpp"
#endif
#if defined(__arm__)
#include "Substrate/SubstrateHook.h"
#endif

#define LOG_TAG "LOGXX"
#define LOG_BUF_SIZE 40960
#define LOGD(fmt,args...) __android_log_print(3,LOG_TAG, fmt, ##args)

typedef int (*t__android_log_write)(int prio, const char *tag,  const char *msg);
typedef int (*t__android_log_vprint)(int prio, const char *tag, const char *fmt, va_list ap);
t__android_log_write __android_log_write = nullptr;
t__android_log_vprint __android_log_vprint = nullptr;

typedef void (*PFN_MSHookFunction)(void *symbol, void *replace, void **result);

#if defined(__aarch64__)
PFN_MSHookFunction pfn_MSHookFunction = A64HookFunction;
#endif
#if defined(__arm__)
PFN_MSHookFunction pfn_MSHookFunction = MSHookFunction;
#endif

extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt,...)
{
    va_list ap;  
	char buf[LOG_BUF_SIZE] = {0};
  
    va_start(ap, fmt);  
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);  
    va_end(ap);

	return __android_log_write(prio,tag,buf);
}

uint64_t get_module_base(const char *module_name) {
    uint64_t addr = 0;
    char line[1024];
    uint64_t start = 0;
    uint64_t end = 0;
    char flags[5];
    char path[PATH_MAX];

    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp != nullptr) {
        while (fgets(line, sizeof(line), fp)) {
            strcpy(path, "");
            sscanf(line, "%" PRIx64"-%" PRIx64" %s %*" PRIx64" %*x:%*x %*u %s\n", &start, &end,
                   flags, path);
#if defined(__aarch64__)
            if (strstr(flags, "x") == 0)
                continue;
#endif
            if (strstr(path, module_name)) {
                addr = start;
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 16
#endif

void hex_dump(void *mem, unsigned int len) {

    char szTmp[40960] = {0x00};
    unsigned int i, j;

    for (i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++) {
        /* print offset */
        if (i % HEXDUMP_COLS == 0) {
#if defined(__x86_64__)
            sprintf(&szTmp[strlen(szTmp)], "0x%06x:\t", (unsigned long)mem + i);
#elif defined(__aarch64__)
            sprintf(&szTmp[strlen(szTmp)], "0x%06x:\t", (unsigned long)mem + i);
#elif defined(__mips64)
            sprintf(&szTmp[strlen(szTmp)], "0x%06x:\t", (unsigned long)mem + i);
#else
            sprintf(&szTmp[strlen(szTmp)], "0x%06x:\t", (unsigned int) mem + i);
#endif
        }

        /* print hex data */
        if (i < len) {
            sprintf(&szTmp[strlen(szTmp)], "%02x ", 0xFF & ((char *) mem)[i]);
        } else /* end of block, just aligning for ASCII dump */
        {
            sprintf(&szTmp[strlen(szTmp)], "   ");
        }

        /* print ASCII dump */
        if (i % HEXDUMP_COLS == (HEXDUMP_COLS - 1)) {
            for (j = i - (HEXDUMP_COLS - 1); j <= i; j++) {
                if (j >= len) /* end of block, not really printing */
                {
                    sprintf(&szTmp[strlen(szTmp)], " ");
                } else if (((char *) mem)[j] > 0x20 &&
                           ((char *) mem)[j] < 0x7e) /* printable char */
                {
                    sprintf(&szTmp[strlen(szTmp)], "%c", 0xFF & ((char *) mem)[j]);
                } else /* other char */
                {
                    sprintf(&szTmp[strlen(szTmp)], ".");
                }
            }
            sprintf(&szTmp[strlen(szTmp)], "\n");
        }
    }
    LOGD("%s", szTmp);
}

int count = 0;

int (*old_lua_loadbuffer)(void *,char *,size_t,char *, int a5 ,int a6);
int my_lua_loadbuffer(void *L,char *buff,size_t sz,char *name, int a5 ,int a6)
{
    // LOGD("AAAAAAAAAAAAAAAAAAAAAaa");
    int ret = old_lua_loadbuffer(L,buff,sz,name, a5, a6);
    if(name[strlen(name)-1] == 0x61
       &&name[strlen(name)-2] == 0x75
       &&name[strlen(name)-3] == 0x6c)
    // std::string ssss(name);
    if (strlen(name) < 128)
    {
        LOGD("luaname is :%s---",name);
        int name_len = strlen(name);
        char *base_dir = (char *)"/data/data/com.game.dir/files/lua_out/";
        char *tail = (char *)".lua";
        char full_name[256];
        //"config/battleConf.luac"
        char *name_t = strdup(name);
        if(strstr(name,"LogoScene") != NULL)
        {
            LOGD("LogoScene:%s",buff);
        }
        int i = name_len;
        while(i>0)
        {
            if(name_t[i] == 0x2f)
            {
                LOGD("in--%s",(char *)&name_t[i+1]);
                sprintf(full_name,"%s%s%d%s",base_dir,(char *)&name_t[i+1],count,tail);
                LOGD("%s\n",full_name);
                count++;
                break;
            }
            i--;
        }
        FILE *fp = fopen(full_name,"w+");
        if(fp != NULL && sz != 0)
        {
            size_t len = 0;
            LOGD("sz1 is %d",sz);
            fwrite(buff,1,sz,fp);
            fclose(fp);
        }else
        {
            LOGD("open file fail!!!");
        }
        free(name_t);
    }
    LOGD("sz2 is %d",sz);
    LOGD("name is %s",name);
    return ret;
}

int (*old_xxtea_decrypt)(int a1, int a2, int a3, unsigned int a4, void *a5);
int new_xxtea_decrypt(int a1, int a2, int a3, unsigned int a4, void *a5) {
    LOGD("xxtea_decrypt");
    return old_xxtea_decrypt(a1, a2, a3, a4, a5);
}

void hook_entry() __attribute__((constructor));
void hook_entry()
{
	void * liblog_handle = dlopen("/system/lib64/liblog.so", RTLD_NOW);///system/lib/arm/liblog.so
	if (liblog_handle)
	{
		__android_log_write = (t__android_log_write)dlsym(liblog_handle,"__android_log_write");
        __android_log_vprint = (t__android_log_vprint)dlsym(liblog_handle,"__android_log_vprint");
	}

    LOGD("hook_entry");
    // AKInitializeOnce();
    void * libm_handle = dlopen("/data/app/com.game.dir-i_RMvdy5OSL55GfGzCemdw==/lib/arm64/libcocos2dlua.so", RTLD_LAZY);
    if (!libm_handle)
    {
        LOGD("Open Error:%s",(char*)dlerror());
        return;
    }
    LOGD("libm_handle = %p", (char*)libm_handle);
    void *p = dlsym(libm_handle, "luaL_loadbufferx");
    LOGD("luaL_loadbuffer = %p", (char*)p);
    pfn_MSHookFunction(p, (void *)my_lua_loadbuffer, (void **)&old_lua_loadbuffer);
    p = dlsym(libm_handle, "xxtea_decrypt");
    LOGD("xxtea_decrypt = %p", (char*)p);
    // uint64_t base = get_module_base("libcocos2dlua.so");
    // LOGD("base ==> %lx", (unsigned long)base);
    // // 5FF374
    // p = (void*)(base + 0x5FF374);
    // pfn_MSHookFunction(p, (void *)new_xxtea_decrypt, (void **)&old_xxtea_decrypt);
}