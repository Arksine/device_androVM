#include <cutils/properties.h>

#include "dispatcher.hpp"
#include "libgenyd.hpp"
#include "genyd.hpp"

void Dispatcher::treatPing(const Request &request, Reply *reply, Genyd *genyd)
{
    (void)request;
    SLOGD("Received Ping");

    reply->set_type(Reply::Pong);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
}

void Dispatcher::getAndroidVersion(const Request &request, Reply *reply, Genyd *genyd)
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

void Dispatcher::getGenymotionVersion(const Request &request, Reply *reply, Genyd *genyd)
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

void Dispatcher::setClipboard(const Request &request, Reply *reply, Genyd *genyd)
{
    SLOGD("Received Set Clipboard");

    reply->set_type(Reply::None);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);

    // ClipboardProxy already connected: send the new clipboard
    if (genyd->getClipboardClient()) {
        genyd->getClipboardClient()->write(request.parameter().value().stringvalue().c_str(),
                                           request.parameter().value().stringvalue().size());
    } else {
        // Keep clipboard value, will be send when clipboardProxy will be connected
        genyd->storeClipboard(request.parameter().value().stringvalue());
    }
}
