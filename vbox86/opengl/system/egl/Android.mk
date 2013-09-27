ifneq (false,$(BUILD_EMULATOR_OPENGL_DRIVER))

LOCAL_PATH := $(call my-dir)

$(call emugl-begin-shared-library,libEGL_emulation)
$(call emugl-import,libOpenglSystemCommon)
$(call emugl-set-shared-library-subpath,egl)

LOCAL_CFLAGS += -DLOG_TAG=\"EGL_emulation\" -DEGL_EGLEXT_PROTOTYPES -DWITH_GLES2

LOCAL_SRC_FILES := \
    eglDisplay.cpp \
    egl.cpp \
    ClientAPIExts.cpp

LOCAL_SHARED_LIBRARIES += libdl
LOCAL_MODULE_TAGS := optional
# Used to access the Bionic private OpenGL TLS slot
LOCAL_C_INCLUDES += bionic/libc/private framework/base/include

$(call emugl-end-module)

#### egl.cfg ####

# Ensure that this file is only copied to emulator-specific builds.
# Other builds are device-specific and will provide their own
# version of this file to point to the appropriate HW EGL libraries.
#
include $(CLEAR_VARS)

LOCAL_MODULE := egl.cfg
LOCAL_SRC_FILES := $(LOCAL_MODULE)

LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/egl
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC

include $(BUILD_PREBUILT)
endif # BUILD_EMULATOR_OPENGL_DRIVER != false
