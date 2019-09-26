LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := AK
LOCAL_SRC_FILES := jniLibs/$(TARGET_ARCH_ABI)/libAK.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE	:= cas
LOCAL_SRC_FILES	:=    \
	cas.cpp \
	fake_dlfcn.cpp

LOCAL_LDLIBS	+=	-L$(SYSROOT)/usr/lib -llog -lz
LOCAL_CPPFLAGS := -fexceptions -frtti -g -std=c++11
LOCAL_CFLAGS += -pie -fPIE -std=c++11 -g
# LOCAL_ARM_MODE	:= arm
LOCAL_SHARED_LIBRARIES := AK
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

dest_path := /data/local/tmp
p_path := /data/app/com.game.dir-YcOiPziAOQeX1lmE3smC_Q==/lib/arm64
all:
	adb push $(NDK_APP_DST_DIR)/libcas.so $(dest_path)/
	adb shell "su -c "cp $(dest_path)/libcas.so $(p_path)/""
	adb shell "su -c "chmod 777 $(p_path)/libcas.so""
	adb push $(NDK_APP_DST_DIR)/libAK.so $(dest_path)/
	adb shell "su -c "cp $(dest_path)/libAK.so $(p_path)/""
	adb shell "su -c "chmod 777 $(p_path)/libAK.so""