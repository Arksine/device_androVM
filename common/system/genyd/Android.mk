LOCAL_PATH		:= $(call my-dir)

##################
## Genyd daemon ##
##################

# Build executable
include $(CLEAR_VARS)

LOCAL_SRC_FILES		:= main.cpp \
			   genyd.cpp \
			   socket.cpp \
			   dispatcher.cpp \
			   gps_handler.cpp \
			   battery_handler.cpp \
			   generic_handler.cpp \
			   capabilities_handler.cpp \
			   accelerometer_handler.cpp \
			   archive_flash_handler.cpp \
			   deviceid_handler.cpp \
		           $(call all-proto-files-under, .)

LOCAL_MODULE		:= genyd

LOCAL_C_INCLUDES	:= bionic \
			   external/stlport/stlport \
			   external/protobuf/src \
			   device/androVM/common/libgenymotion	\
			   device/androVM/vbox86/libsensor \
			   external/sqlite/dist

LOCAL_MODULE_TAGS	:= optional

LOCAL_CFLAGS		:= -Wall -O2

LOCAL_SHARED_LIBRARIES	:= libnetutils \
			   libsqlite \

LOCAL_STATIC_LIBRARIES	:= libgenymotion \
			   libstlport_static \
			   liblog \
			   libcutils

LOCAL_PROTOC_OPTIMIZE_TYPE := full

include $(BUILD_EXECUTABLE)
