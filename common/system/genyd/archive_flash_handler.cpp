
#include <cutils/properties.h>

#include <sstream>
#include <string>

#include "dispatcher.hpp"
#include "libgenyd.hpp"

#define SH_PATH              "/system/bin/sh"
#define CHECK_ARCHIVE_SCRIPT "/system/bin/check-archive.sh"
#define FLASH_ARCHIVE_SCRIPT "/system/bin/flash-archive.sh"

void Dispatcher::checkArchive(const Request &request, Reply *reply, Genyd *genyd)
{
    std::string archive_path = request.parameter().value().stringvalue();

    SLOGD("Received CheckArchive %s", archive_path.c_str());

    std::string cmd = std::string(SH_PATH) + " " +
                      CHECK_ARCHIVE_SCRIPT + " " +
                      "\"" + archive_path + "\"";

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

void Dispatcher::flashArchive(const Request &request, Reply *reply, Genyd *genyd)
{
    std::string archive_path = request.parameter().value().stringvalue();

    SLOGD("Received FlashArchive %s", archive_path.c_str());

    std::string cmd = std::string(SH_PATH) + " " +
                      FLASH_ARCHIVE_SCRIPT + " " +
                      "\"" + archive_path + "\"";

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
