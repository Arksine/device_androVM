
#include <cutils/properties.h>

#include "dispatcher.hpp"

Dispatcher::Dispatcher(void)
{
    // "GetParam" callbacks list
    getCallbacks[Parameter::AndroidVersion] = &Dispatcher::getAndroidVersion;
    getCallbacks[Parameter::GenymotionVersion] = &Dispatcher::getGenymotionVersion;
    getCallbacks[Parameter::BatteryStatus] = &Dispatcher::getBatteryStatus;
    getCallbacks[Parameter::BatteryLevel] = &Dispatcher::getBatteryLevel;
    getCallbacks[Parameter::BatteryMode] = &Dispatcher::isBatteryManual;
    getCallbacks[Parameter::GpsStatus] = &Dispatcher::getGpsStatus;
    getCallbacks[Parameter::GpsLatitude] = &Dispatcher::getGpsLatitude;
    getCallbacks[Parameter::GpsLongitude] = &Dispatcher::getGpsLongitude;
    getCallbacks[Parameter::GpsAltitude] = &Dispatcher::getGpsAltitude;
    getCallbacks[Parameter::GpsAccuracy] = &Dispatcher::getGpsAccuracy;
    getCallbacks[Parameter::GpsBearing] = &Dispatcher::getGpsBearing;
    getCallbacks[Parameter::Accelerometer] = &Dispatcher::getAccelerometerValues;
    getCallbacks[Parameter::Capabilities] = &Dispatcher::getCapabilities;
    getCallbacks[Parameter::DeviceId] = &Dispatcher::getDeviceId;
    getCallbacks[Parameter::AndroidId] = &Dispatcher::getAndroidId;

    // "SetParam" callback list
    setCallbacks[Parameter::BatteryStatus] = &Dispatcher::setBatteryStatus;
    setCallbacks[Parameter::BatteryLevel] = &Dispatcher::setBatteryLevel;
    setCallbacks[Parameter::BatteryMode] = &Dispatcher::setBatteryMode;
    setCallbacks[Parameter::GpsStatus] = &Dispatcher::setGpsStatus;
    setCallbacks[Parameter::GpsLatitude] = &Dispatcher::setGpsLatitude;
    setCallbacks[Parameter::GpsLongitude] = &Dispatcher::setGpsLongitude;
    setCallbacks[Parameter::GpsAltitude] = &Dispatcher::setGpsAltitude;
    setCallbacks[Parameter::GpsAccuracy] = &Dispatcher::setGpsAccuracy;
    setCallbacks[Parameter::GpsBearing] = &Dispatcher::setGpsBearing;
    setCallbacks[Parameter::Accelerometer] = &Dispatcher::setAccelerometerValues;
    setCallbacks[Parameter::Clipboard] = &Dispatcher::setClipboard;
    setCallbacks[Parameter::DeviceId] = &Dispatcher::setDeviceId;
    setCallbacks[Parameter::AndroidId] = &Dispatcher::setAndroidId;
}

Dispatcher::~Dispatcher(void)
{

}

void Dispatcher::treatGetParam(const Request &request, Reply *reply, Genyd *genyd)
{
    if (!request.has_parameter()) {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::GenericError);
        return;
    }

    Parameter param = request.parameter();
    Genymotion::Parameter_Type type = param.type();
    SLOGD("Received Get %s", Parameter::Type_Name(type).c_str());

    std::map<int, Dispatcher::t_get_callback>::iterator func = getCallbacks.find(type);

    if (func != getCallbacks.end()) {
        (this->*(func->second))(request, reply, genyd);
    } else {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::NotImplemented);
    }
}

void Dispatcher::treatSetParam(const Request &request, Reply *reply, Genyd *genyd)
{
    if (!request.has_parameter()) {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::GenericError);
        return;
    }

    Parameter param = request.parameter();

    if (!param.has_value()) {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::GenericError);
        return;
    }

    Genymotion::Parameter_Type type = param.type();
    SLOGD("Received Set %s", Parameter::Type_Name(type).c_str());

    std::map<int, Dispatcher::t_set_callback>::iterator func = setCallbacks.find(type);

    if (func != setCallbacks.end()) {
        (this->*(func->second))(request, reply, genyd);
    } else {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::NotImplemented);
    }
}

void Dispatcher::treatCheckArchive(const Request &request, Reply *reply, Genyd *genyd)
{
    if (!request.has_parameter()) {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::GenericError);
        return;
    }

    Parameter param = request.parameter();

    if (!param.has_value()) {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::GenericError);
        return;
    }

    checkArchive(request, reply, genyd);
}

void Dispatcher::treatFlashArchive(const Request &request, Reply *reply, Genyd *genyd)
{
    if (!request.has_parameter()) {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::GenericError);
        return;
    }

    Parameter param = request.parameter();

    if (!param.has_value()) {
        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::GenericError);
        return;
    }

    flashArchive(request, reply, genyd);
}

void Dispatcher::unknownRequest(const Request &request, Reply *reply, Genyd *genyd)
{
    SLOGD("Received unknown request");

    reply->set_type(Reply::Error);
    Status *status = reply->mutable_status();
    status->set_code(Status::InvalidRequest);
}

Reply *Dispatcher::dispatchRequest(const Request &request, Genyd *genyd)
{
    Reply *reply = new Reply();

    switch (request.type()) {
    case Request::Ping:
        treatPing(request, reply, genyd);
        break;
    case Request::SetParam:
        treatSetParam(request, reply, genyd);
        break;
    case Request::GetParam:
        treatGetParam(request, reply, genyd);
        break;
    case Request::CheckArchive:
        treatCheckArchive(request, reply, genyd);
        break;
    case Request::FlashArchive:
        treatFlashArchive(request, reply, genyd);
	break;
    default:
        unknownRequest(request, reply, genyd);
        break;
    }

    return (reply);
}
