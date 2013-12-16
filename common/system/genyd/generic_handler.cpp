#include <cutils/properties.h>

#include "dispatcher.hpp"
#include "libgenyd.hpp"

void Dispatcher::treatPing(const Request &request, Reply *reply)
{
    (void)request;
    SLOGD("Received Ping");

    reply->set_type(Reply::Pong);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
}

void Dispatcher::getAndroidVersion(const Request &request, Reply *reply)
{
    SLOGD("Received Get AndroidVersion");

    reply->set_type(Reply::Value);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
    Value *value = reply->mutable_value();
    value->set_type(Value::String);

    char property[PROPERTY_VALUE_MAX];
    property_get(ANDROID_VERSION, property, "Unknown");
    value->set_stringvalue(property);
}

void Dispatcher::getGenymotionVersion(const Request &request, Reply *reply)
{
    SLOGD("Received Get GenymotionVersion");

    reply->set_type(Reply::Value);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
    Value *value = reply->mutable_value();
    value->set_type(Value::String);

    char property[PROPERTY_VALUE_MAX];
    property_get(GENYMOTION_VERSION, property, "Unknown");
    value->set_stringvalue(property);
}

void Dispatcher::setClipboardValue(const Request &request, Reply *reply)
{
    (void)request;
    (void)reply;

    SLOGD("Received Set Clipboard");
}
