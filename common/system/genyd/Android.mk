LOCAL_PATH		:= $(call my-dir)

###################
## Genyd library ##
###################

# Build shared library
include $(CLEAR_VARS)

LOCAL_CFLAGS            := -Wall

LOCAL_C_INCLUDES        := $(LOCAL_PATH) \
                           bionic \
                           external/stlport/stlport

LOCAL_SRC_FILES         := lib/libgenyd.cpp \
                           lib/sensor_battery.cpp

LOCAL_MODULE_TAGS       := optional
LOCAL_SHARED_LIBRARIES  := liblog libcutils libstlport

LOCAL_MODULE            := libgenyd

include $(BUILD_SHARED_LIBRARY)


# Build static library
include $(CLEAR_VARS)

LOCAL_CFLAGS            := -Wall -O2

LOCAL_C_INCLUDES        := $(LOCAL_PATH) \
                           bionic \
                           external/stlport/stlport

LOCAL_SRC_FILES         := lib/libgenyd.cpp \
                           lib/sensor_battery.cpp

LOCAL_MODULE_TAGS       := optional
LOCAL_STATIC_LIBRARIES  := liblog libcutils libstlport_static

LOCAL_MODULE            := libgenyd

include $(BUILD_STATIC_LIBRARY)


##################
## Genyd daemon ##
##################

# Build executable
include $(CLEAR_VARS)

LOCAL_SRC_FILES		:= main.cpp				\
			   genyd.cpp				\
			   socket.cpp           		\
			   dispatcher.cpp			\
			   gps_handler.cpp                      \
			   battery_handler.cpp			\
			   capabilities_handler.cpp		\
			   accelerometer_handler.cpp		\
			   archive_flash_handler.cpp            \
		           $(call all-proto-files-under, .)

LOCAL_MODULE		:= genyd

LOCAL_C_INCLUDES	:= $(KERNEL_HEADERS)       		  \
			   bionic                  		  \
			   external/stlport/stlport		  \
			   external/protobuf/src		  \
			   device/androVM/common/system/genyd/lib \
			   device/androVM/vbox86/libsensor

LOCAL_MODULE_TAGS	:= optional

LOCAL_CFLAGS		:= -Wall -02

LOCAL_SHARED_LIBRARIES	:= libcutils	\
			   libnetutils	\
			   liblog	\
			   libstlport	\
			   libgenyd

LOCAL_PROTOC_OPTIMIZE_TYPE := full

include $(BUILD_EXECUTABLE)
