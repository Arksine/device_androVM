$(call inherit-product,$(SRC_TARGET_DIR)/product/generic_x86.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/board/generic_x86/BoardConfig.mk)

PRODUCT_NAME := vbox86p
PRODUCT_DEVICE := vbox86p
PRODUCT_MODEL := androVM for VirtualBox ('Phone' version)


PRODUCT_PACKAGES += $(THIRD_PARTY_APPS)
PRODUCT_PACKAGES += libGLESv1_enc \
	libGLESv2_enc \
	local_opengl \
	libGLESv1_CM_emulation \
	libGLESv2_emulation \
	libEGL_emulation \
	egl.cfg \
	gralloc.vbox86 \
	vinput \

LOCAL_KERNEL := device/androVM/vbox86/kernel-vbox86

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel \
    device/androVM/vbox86/init.vbox86.rc:root/init.vbox86.rc

