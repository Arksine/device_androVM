#include <cutils/properties.h>

#include "dispatcher.hpp"

////////////////////
// Helper methods //
////////////////////

static inline std::string getStdStringProperty(const char *key)
{
    char value[PROPERTY_VALUE_MAX];
    property_get(key, value, "");

    return value;
}

// Prepare Reply object with OK status
static void buildOkReply(Reply *reply)
{
    // Make a standard Ok reply
    reply->set_type(Reply::None);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
}

// Prepare Reply object with OK status and a string reply
static void buildStringReply(Reply *reply, const std::string &strValue)
{
    // Build an OK reply
    buildOkReply(reply);

    // Fill reply with given string
    reply->set_type(Reply::Value);
    Value *value = reply->mutable_value();
    value->set_type(Value::String);
    value->set_stringvalue(strValue);
}

/////////////////////////////
// Genymotion daemon verbs //
/////////////////////////////

void Dispatcher::getDeviceId(const Request &request, Reply *reply)
{
    SLOGE("Getting DeviceID");

    // Read Device ID
    //std::string deviceId = "012962001339909"s;
    std::string deviceId = getStdStringProperty(DEVICE_ID);

    // Build reply
    buildStringReply(reply, deviceId);
}

void Dispatcher::setDeviceId(const Request &request, Reply *reply)
{
    SLOGE("Setting DeviceID");

    // Read Device ID from request
    std::string androidId = request.parameter().value().stringvalue();

    // Apply new DeviceID by setting property
    property_set(DEVICE_ID, androidId.c_str());

    // Build standard OK reply;
    buildOkReply(reply);
}

void Dispatcher::getAndroidId(const Request &request, Reply *reply)
{
    SLOGE("Getting AndroidID");

    // Read android ID
    //std::string androidId = "666"s;
    std::string androidId = getStdStringProperty(ANDROID_ID);

    // Build reply
    buildStringReply(reply, androidId);
}

void Dispatcher::setAndroidId(const Request &request, Reply *reply)
{
    SLOGE("Setting AndroidID");

    // Read Device ID from request
    std::string androidId = request.parameter().value().stringvalue();

    // Apply new DeviceID by setting property
    property_set(ANDROID_ID, androidId.c_str());

    // Build standard OK reply;
    buildOkReply(reply);
}
