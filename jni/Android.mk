LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
SUBSTRATE_LIB_PATH_ARM := ../../cydia_substrate/lib/armeabi
SUBSTRATE_LIB_PATH_x86 := ../../cydia_substrate/lib/x86
LOCAL_MODULE    := luabufferhook.cy
LOCAL_SRC_FILES := luabufferhook.cy.cpp
LOCAL_LDLIBS := -L$(SUBSTRATE_LIB_PATH_ARM) -L$(SUBSTRATE_LIB_PATH_x86) -lsubstrate -lsubstrate-dvm -llog       
LOCAL_C_INCLUDES := ../../cydia_substrate
include $(BUILD_SHARED_LIBRARY)
