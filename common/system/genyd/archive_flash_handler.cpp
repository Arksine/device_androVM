
#include <cutils/properties.h>

#include <sstream>
#include <string>

#include "dispatcher.hpp"
#include "libgenyd.hpp"

#define CHECK_ARCHIVE_SCRIPT "sh /system/bin/check-archive.sh \""
#define FLASH_ARCHIVE_SCRIPT "sh /system/bin/flash-archive.sh \""

void Dispatcher::checkArchive(const Request &request, Reply *reply)
{
    std::string archive_path = request.parameter().value().stringvalue();
    
    SLOGD("Received CheckArchive %s", archive_path.c_str());

    std::string script = CHECK_ARCHIVE_SCRIPT;
    std::string cmd = script + archive_path + "\"";

    reply->set_type(Reply::Error);
    Status *status = reply->mutable_status();

    int res = system(cmd.c_str());

    SLOGD("CheckArchive : %s - %d", cmd.c_str(), res);

    if (res == 0) {
        SLOGD("CheckArchive : status OK");
        status->set_code(Status::Ok);
    } else {
        status->set_code(Status::GenericError);
    }
}

void Dispatcher::flashArchive(const Request &request, Reply *reply)
{
    std::string archive_path = request.parameter().value().stringvalue();

    SLOGD("Received FlashArchive %s", archive_path.c_str());

    std::string script = FLASH_ARCHIVE_SCRIPT;
    std::string cmd = script + archive_path + "\"";

    reply->set_type(Reply::Error);
    Status *status = reply->mutable_status();

    int res = system(cmd.c_str());

    SLOGD("FlashArchive : %s - %d", cmd.c_str(), res);

    if (res == 0) {
        SLOGD("FlashArchive : status OK");
        status->set_code(Status::Ok);
    } else {
        status->set_code(Status::GenericError);
    }
}
