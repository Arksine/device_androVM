LOCAL_PATH		:= $(call my-dir)

########################
## Genymotion library ##
########################

# Build static library
include $(CLEAR_VARS)

LOCAL_CFLAGS            := -Wall

LOCAL_C_INCLUDES        := $(LOCAL_PATH) \
                           bionic \
                           external/stlport/stlport

LOCAL_SRC_FILES         := libgenyd.cpp \
                           sensor_battery.cpp

LOCAL_MODULE_TAGS       := optional
LOCAL_STATIC_LIBRARIES  := liblog libcutils libstlport_static

LOCAL_MODULE            := libgenymotion

include $(BUILD_STATIC_LIBRARY)
