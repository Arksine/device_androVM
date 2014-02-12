LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, .)

LOCAL_SDK_VERSION := current

LOCAL_PACKAGE_NAME := ClipboardProxy

include $(BUILD_PACKAGE)
