#ifndef DISPATCHER_HPP_
#define DISPATCHER_HPP_

#include "global.hpp"

class Genyd;

class Dispatcher {

public:
    Dispatcher(void);
    ~Dispatcher(void);

private:
    Dispatcher(const Dispatcher &);
    Dispatcher operator=(const Dispatcher &);

    typedef void (Dispatcher::*t_get_callback)(const Request &, Reply *, Genyd *);
    std::map<int, t_get_callback> getCallbacks;

    typedef void (Dispatcher::*t_set_callback)(const Request &, Reply *, Genyd *);
    std::map<int, t_set_callback> setCallbacks;

public:
    // Switch among requests
    Reply *dispatchRequest(const Request &request, Genyd *genyd);

private:

    /////////////////////////
    // Dispatchers         //
    /////////////////////////

    // Answer "Ping" request
    void treatPing(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "GetParam" requests
    void treatGetParam(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "SetParam" requests
    void treatSetParam(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "CheckArchive" requests
    void treatCheckArchive(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "SetParam" requests
    void treatFlashArchive(const Request &request, Reply *reply, Genyd *genyd);

    // Fallback for unknown requests
    void unknownRequest(const Request &request, Reply *reply, Genyd *genyd);



    /////////////////////////
    // Generic requests    //
    // generic_handler.cpp //
    /////////////////////////

    // Answer "GetParam AndroidVersion" request
    void getAndroidVersion(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "GetParam Capabilities" request
    void getCapabilities(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "GetParam GenymotionVersion" request
    void getGenymotionVersion(const Request &request, Reply *reply, Genyd *genyd);

    // Answer setClipboard
    void setClipboard(const Request &request, Reply *reply, Genyd *genyd);

    /////////////////////////
    // Battery requests    //
    // battery_handler.cpp //
    /////////////////////////

    // Miscs
    void setDefaultBatteryLevel(void) const;
    void setDefaultBatteryStatus(void) const;

    // Action behind "SetParam Battery Level" request
    void setBatteryLevel(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "GetParam Battery Level" request
    void getBatteryLevel(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "SetParam Battery Status" request
    void setBatteryStatus(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "GetParam Battery Status" request
    void getBatteryStatus(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "SetParam Battery Mode" request
    void setBatteryMode(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "GetParam Battery Mode" == manual request
    void isBatteryManual(const Request &request, Reply *reply, Genyd *genyd);


    /////////////////////
    // GPS requests    //
    // gps_handler.cpp //
    /////////////////////

    // Answer "Get/Set GPS Status" request
    void getGpsStatus(const Request &request, Reply *reply, Genyd *genyd);
    void setGpsStatus(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "Get/Set GPS Latitude" request
    void getGpsLatitude(const Request &request, Reply *reply, Genyd *genyd);
    void setGpsLatitude(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "Get/Set GPS Longitude" request
    void getGpsLongitude(const Request &request, Reply *reply, Genyd *genyd);
    void setGpsLongitude(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "Get/Set GPS Altitude" request
    void getGpsAltitude(const Request &request, Reply *reply, Genyd *genyd);
    void setGpsAltitude(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "Get/Set GPS Accuracy" request
    void getGpsAccuracy(const Request &request, Reply *reply, Genyd *genyd);
    void setGpsAccuracy(const Request &request, Reply *reply, Genyd *genyd);

    // Answer "Get/Set GPS Bearing" request
    void getGpsBearing(const Request &request, Reply *reply, Genyd *genyd);
    void setGpsBearing(const Request &request, Reply *reply, Genyd *genyd);


    ///////////////////////////////
    // Accelerometer requests    //
    // accelerometer_handler.cpp //
    ///////////////////////////////

    // Answer "Get/Set Accelerometer Values" request
    void getAccelerometerValues(const Request &request, Reply *reply, Genyd *genyd);
    void setAccelerometerValues(const Request &request, Reply *reply, Genyd *genyd);


    ///////////////////////////////
    // Archive flash requests    //
    // archive_flash_handler.cpp //
    ///////////////////////////////

    void checkArchive(const Request &request, Reply *reply, Genyd *genyd);
    void flashArchive(const Request &request, Reply *reply, Genyd *genyd);

    //////////////////////////////////////////
    // DeviceID / IMEI / AndroidID requests //
    // deviceid_handler.cpp                 //
    //////////////////////////////////////////

    void getDeviceId(const Request &request, Reply *reply, Genyd *genyd));
    void setDeviceId(const Request &request, Reply *reply, Genyd *genyd));

    void getAndroidId(const Request &request, Reply *reply, Genyd *genyd));
    void setAndroidId(const Request &request, Reply *reply, Genyd *genyd));
};

#endif
