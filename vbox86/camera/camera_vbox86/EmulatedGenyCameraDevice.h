/*
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

#ifndef HW_EMULATOR_CAMERA_EMULATED_GENY_CAMERA_DEVICE_H
#define HW_EMULATOR_CAMERA_EMULATED_GENY_CAMERA_DEVICE_H

/*
 * Contains declaration of a class EmulatedGenyCameraDevice that encapsulates
 * an emulated camera device connected to the host.
 */

#include "EmulatedCameraDevice.h"
#include "GenyClient.h"

namespace android {

class EmulatedGenyCamera;

/* Encapsulates an emulated camera device connected to the host.
 */
class EmulatedGenyCameraDevice : public EmulatedCameraDevice {
public:
    /* Constructs EmulatedGenyCameraDevice instance. */
    explicit EmulatedGenyCameraDevice(EmulatedGenyCamera* camera_hal);

    /* Destructs EmulatedGenyCameraDevice instance. */
    ~EmulatedGenyCameraDevice();

    /***************************************************************************
     * Public API
     **************************************************************************/

public:
    /* Initializes EmulatedGenyCameraDevice instance.
     * Param:
     *  device_name - name of the camera (front or back for now)
     *  local_srv_port - Port to connect to local_camera srv
     * Return:
     *  NO_ERROR on success, or an appropriate error status.
     */
    status_t Initialize(const char *device_name, const int local_srv_port);

    /* Get device info from the player.
     * Param:
     *  p_info_str - an address to store the allocated information string that
     *               will be returned by the player
     * Return:
     *  NO_ERROR on success, or an appropriate error status.
     */
    status_t getDeviceInfo(char **p_info_str);

    /***************************************************************************
     * Emulated camera device abstract interface implementation.
     * See declarations of these methods in EmulatedCameraDevice class for
     * information on each of these methods.
     **************************************************************************/

public:
    /* Connects to the camera device. */
    status_t connectDevice();

    /* Disconnects from the camera device. */
    status_t disconnectDevice();

    /* Starts capturing frames from the camera device. */
    status_t startDevice(int width, int height, uint32_t pix_fmt);

    /* Stops capturing frames from the camera device. */
    status_t stopDevice();

    /***************************************************************************
     * EmulatedCameraDevice virtual overrides
     * See declarations of these methods in EmulatedCameraDevice class for
     * information on each of these methods.
     **************************************************************************/

public:
    /* Gets current preview fame into provided buffer.
     * We override this method in order to provide preview frames cached in this
     * object.
     */
    status_t getCurrentPreviewFrame(void* buffer);

    /***************************************************************************
     * Worker thread management overrides.
     * See declarations of these methods in EmulatedCameraDevice class for
     * information on each of these methods.
     **************************************************************************/

protected:
    /* Implementation of the worker thread routine. */
    bool inWorkerThread();

    /***************************************************************************
     * Qemu camera device data members
     **************************************************************************/

private:
    /* Qemu client that is used to communicate with the 'emulated camera'
     * service, created for this instance in the emulator. */
    CameraGenyClient    mGenyClient;

    /* Name of the camera device connected to the host. */
    String8             mDeviceName;

    /* Emulated FPS (frames per second).
     * We will emulate 50 FPS. */
    static const int    mEmulatedFPS = 25;//50;
    int32_t mLastFrame;
};

}; /* namespace android */

#endif  /* HW_EMULATOR_CAMERA_EMULATED_GENY_CAMERA_DEVICE_H */
