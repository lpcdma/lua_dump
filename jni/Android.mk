LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= cas
LOCAL_SRC_FILES	:=    \
	cas.cpp \
	fake_dlfcn.cpp 

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_SRC_FILES += Substrate/hde64.c \
	Substrate/SubstrateDebug.cpp \
	Substrate/SubstrateHook.cpp \
	Substrate/SubstratePosixMemory.cpp 
endif
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_SRC_FILES += And64InlineHook.cpp
endif
	
LOCAL_LDLIBS	+=	-L$(SYSROOT)/usr/lib -llog -lz
LOCAL_CPPFLAGS := -fexceptions -frtti -g -std=c++11
LOCAL_CFLAGS += -pie -fPIE -std=c++11 -g
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

dest_path := /data/local/tmp
p_path := /data/app/com.game.dir-i_RMvdy5OSL55GfGzCemdw==/lib/arm64
all:
	adb push $(NDK_APP_DST_DIR)/libcas.so $(dest_path)/
	adb shell "su -c "cp $(dest_path)/libcas.so $(p_path)/""
	adb shell "su -c "chmod 777 $(p_path)/libcas.so""