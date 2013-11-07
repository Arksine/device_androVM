$(call inherit-product,$(SRC_TARGET_DIR)/product/generic_x86.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/board/generic_x86/BoardConfig.mk)

PRODUCT_NAME := vbox86p
PRODUCT_DEVICE := vbox86p
PRODUCT_MODEL := androVM for VirtualBox ('Phone' version)

PRODUCT_PROPERTY_OVERRIDES := \
    wifi.interface=eth1


PRODUCT_PACKAGES += $(THIRD_PARTY_APPS)
PRODUCT_PACKAGES += libGLESv1_enc \
	libGLESv2_enc \
	local_opengl \
	libGLESv1_CM_emulation \
	libGLESv2_emulation \
	libEGL_emulation \
	gralloc.vbox86 \
	vinput \
        androVM-prop \
        androVM-vbox-sf \
        androVM_setprop \
        busybox \
        miniunzip \
        genyd \
        gps.vbox86 \
        local_gps \
        sensors.vbox86 \
#        camera.vbox86 \
#        local_camera \

LOCAL_KERNEL := device/androVM/vbox86/kernel-vbox86

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel \
    device/androVM/vbox86/init.vbox86.rc:root/init.vbox86.rc \
    device/androVM/vbox86/init.androVM.sh:system/etc/init.androVM.sh \
    device/androVM/vbox86/uvesafb.ko:system/lib/uvesafb.ko \
    device/androVM/vbox86/vbox/vboxguest.ko:system/lib/vboxguest.ko \
    device/androVM/vbox86/vbox/vboxsf.ko:system/lib/vboxsf.ko \
    device/androVM/vbox86/vbox/vboxvideo.ko:system/lib/vboxvideo.ko \
    device/androVM/vbox86/egl.cfg:system/lib/egl/egl.cfg \
    device/androVM/vbox86/init.rc:root/init.rc \
    device/androVM/vbox86/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf \
    device/androVM/vbox86/wpa_supplicant.conf:data/misc/wifi/wpa_supplicant.conf \
    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    device/androVM/common/gps.conf:system/etc/gps.conf \
    frameworks/base/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/base/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    device/androVM/common/scripts/check-archive.sh:system/bin/check-archive.sh \
    device/androVM/common/scripts/flash-archive.sh:system/bin/flash-archive.sh \
#    frameworks/base/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \


