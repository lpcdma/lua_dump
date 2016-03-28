/*
* Description:common functions header
* author: neve
*/
#ifndef _COMMON_H
#define _COMMON_H

#include <android/log.h>

#define  LOGD_TAG "ANDROID_HOOK"
#define  LOGD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG,LOGD_TAG, fmt, ##args)

#define  LOGI_TAG "COCOS"
#define  LOGI(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG,LOGI_TAG, fmt, ##args)


void find_maxsize_file(char* dir,char* &libso);

#endif
