LOCAL_PATH := $(call my-dir)

### GLESv1 implementation ###########################################
$(call emugl-begin-shared-library,libGLESv1_CM_emulation)
$(call emugl-import,libOpenglSystemCommon libGLESv1_enc lib_renderControl_enc)

LOCAL_CFLAGS += -DLOG_TAG=\"GLES_emulation\" -DGL_GLEXT_PROTOTYPES

LOCAL_SRC_FILES := gl.cpp
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += system/core/include framework/base/include
$(call emugl-end-module)
