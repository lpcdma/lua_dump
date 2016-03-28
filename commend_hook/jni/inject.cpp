#define DEBUG_MODE

#include <stdio.h>
#include "inject.h"

int main(int argc, char *argv[])
{
	NDKINJECT inj;
	pid_t zygote_pid;
	pid_t game_pid;
	int sohandle = 0;

	//根据进程名找PID
	zygote_pid = inj.find_pid_of("zygote");
	game_pid = inj.find_pid_of("com.lpcdma.cocos.test");

	LOGD("find zygote pid:%d",zygote_pid);

	//注入参数：PID，要注入的SO路径，注入后立即执行的SO入口函数名，入口函数字符串参数，字符串参数长度，执行完入口函数是否立即释放SO
	sohandle = inj.inject_remote_process(game_pid, "/data/local/tmp/cocokill/libmain.so", "hook_entry",NULL, 0, 0);
	if(-1 == sohandle)
	{
		LOGD("inject so error!");
		return -1;
	}

	 LOGD("sohandle is :%d",sohandle);
	return 0;
}
