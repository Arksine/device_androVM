LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(GENERIC_X86_ANDROID_MK)
include $(call all-makefiles-under,$(LOCAL_PATH))
include $(call all-makefiles-under,../common)
include $(call all-makefiles-under,../vbox86)

