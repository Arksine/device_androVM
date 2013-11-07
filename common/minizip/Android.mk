LOCAL_PATH		:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES		:= unzip.c \
                           ioapi.c \
                           miniunz.c

LOCAL_MODULE		:= miniunzip
LOCAL_MODULE_TAGS       := optional

LOCAL_SHARED_LIBRARIES	:= libz

include $(BUILD_EXECUTABLE)
