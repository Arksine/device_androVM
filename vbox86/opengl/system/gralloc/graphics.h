/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYSTEM_CORE_INCLUDE_ANDROID_GRAPHICS_H
#define SYSTEM_CORE_INCLUDE_ANDROID_GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * If the HAL needs to create service threads to handle graphics related
 * tasks, these threads need to run at HAL_PRIORITY_URGENT_DISPLAY priority
 * if they can block the main rendering thread in any way.
 *
 * the priority of the current thread can be set with:
 *
 *      #include <sys/resource.h>
 *      setpriority(PRIO_PROCESS, 0, HAL_PRIORITY_URGENT_DISPLAY);
 *
 */

#define HAL_PRIORITY_URGENT_DISPLAY     (-8)

// see hardware.h

/**
 * pixel format definitions
 */

enum {
    /*
     * Android RAW sensor format:
     *
     * This format is exposed outside of the HAL to applications.
     *
     * RAW_SENSOR is a single-channel 16-bit format, typically representing raw
     * Bayer-pattern images from an image sensor, with minimal processing.
     *
     * The exact pixel layout of the data in the buffer is sensor-dependent, and
     * needs to be queried from the camera device.
     *
     * Generally, not all 16 bits are used; more common values are 10 or 12
     * bits. All parameters to interpret the raw data (black and white points,
     * color space, etc) must be queried from the camera device.
     *
     * This format assumes
     * - an even width
     * - an even height
     * - a horizontal stride multiple of 16 pixels (32 bytes).
     */
    HAL_PIXEL_FORMAT_RAW_SENSOR = 0x20,

    /*
     * Android binary blob graphics buffer format:
     *
     * This format is used to carry task-specific data which does not have a
     * standard image structure. The details of the format are left to the two
     * endpoints.
     *
     * A typical use case is for transporting JPEG-compressed images from the
     * Camera HAL to the framework or to applications.
     *
     * Buffers of this format must have a height of 1, and width equal to their
     * size in bytes.
     */
    HAL_PIXEL_FORMAT_BLOB = 0x21,

    /*
     * Android format indicating that the choice of format is entirely up to the
     * device-specific Gralloc implementation.
     *
     * The Gralloc implementation should examine the usage bits passed in when
     * allocating a buffer with this format, and it should derive the pixel
     * format from those usage flags.  This format will never be used with any
     * of the GRALLOC_USAGE_SW_* usage flags.
     *
     * If a buffer of this format is to be used as an OpenGL ES texture, the
     * framework will assume that sampling the texture will always return an
     * alpha value of 1.0 (i.e. the buffer contains only opaque pixel values).
     *
     */
    HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED = 0x22,
};


/**
 * Transformation definitions
 *
 * IMPORTANT NOTE:
 * HAL_TRANSFORM_ROT_90 is applied CLOCKWISE and AFTER HAL_TRANSFORM_FLIP_{H|V}.
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_CORE_INCLUDE_ANDROID_GRAPHICS_H */
