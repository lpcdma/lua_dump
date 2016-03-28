/*
* Description:hook core 
* author: neve
*/

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "hooklib.h"
#include "common.h"



typedef void (*MSHookFunction_ptr)(void *,void *,void **);

MSHookFunction_ptr MSHookFunction;

/*
* find the function address from symbol
*/
void* Hooklib::lookup_symbol(char *library, char *symbol)
{
	void *imagehandle = dlopen(library, RTLD_GLOBAL | RTLD_NOW);

    if (imagehandle != NULL){
        void * sym = dlsym(imagehandle, symbol);
        if (sym != NULL){
            LOGD("(lookup_symbol) find loadbuffer");
            return sym;
            }
        else{
            LOGD("(lookup_symbol) dlsym didn't work");
            return NULL;
        }
    }
    else{
        LOGD("(lookup_symbol) dlerror: %s",dlerror());
        return NULL;
    }
}

/*
* hook implementation
*/
void Hooklib::hook_wrapper(void *symbol, void *replace, void **result)
{
	addr_hook_func = lookup_symbol((char*)"/data/local/tmp/libsubstrate.so",(char*)"MSHookFunction");
	MSHookFunction = (MSHookFunction_ptr)addr_hook_func;
	MSHookFunction(symbol,replace,result);
}
