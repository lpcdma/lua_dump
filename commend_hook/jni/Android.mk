LOCAL_PATH := $(call my-dir)

######inject#######
include $(CLEAR_VARS)
LOCAL_LDLIBS	+=	-L$(SYSROOT)/usr/lib -llog -lz
LOCAL_MODULE	:= inject
LOCAL_SRC_FILES	:= inject.cpp
LOCAL_ARM_MODE	:= arm
include $(BUILD_EXECUTABLE)
######libmain#######
include $(CLEAR_VARS)
LOCAL_LDLIBS += -L$(SYSTEM)/usr/lib -llog
LOCAL_MODULE := main
LOCAL_SRC_FILES := mainso.cpp hooklib.cpp common.cpp             
include $(BUILD_SHARED_LIBRARY)


dest_path := /data/local/tmp/cocokill
all:
	-@adb shell "mkdir $(dest_path)" 2 > nul
	adb push $(NDK_APP_DST_DIR)/libmain.so $(dest_path)/
	adb push $(NDK_APP_DST_DIR)/inject $(dest_path)/
	adb shell "su -c 'chmod 744 $(dest_path)/inject'"