/*
* Description:hook core header
* author: neve
*/

#ifndef _HOOKLIB_
#define _HOOKLIB_ 

#include <stdio.h>
#include <stdlib.h>


class Hooklib
{

public:
	void* lookup_symbol(char* library,char* symbol);
	void hook_wrapper(void* symbol, void* replace, void** result);

private:
	void* addr_hook_func;
	
};

#endif
