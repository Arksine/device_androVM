#include <cutils/properties.h>
#include <sqlite3.h>
#include <algorithm>
#include <cctype>

#include "dispatcher.hpp"

// global file's cache usefull for getAndroidId method and callback
static std::string _android_id_cache;

////////////////////
// Helper methods //
////////////////////

static inline std::string getStdStringProperty(const char *key)
{
    char value[PROPERTY_VALUE_MAX];
    property_get(key, value, "");

    return value;
}

// Prepare Reply object with a given status
static void buildErrorReply(const ::Genymotion::Status_Code code, Reply *reply)
{
    // Make an error reply
    reply->set_type(Reply::None);
    Status *status = reply->mutable_status();
    status->set_code(code);
}

// Prepare Reply object with OK status
static void buildOkReply(Reply *reply)
{
    // Make a standard Ok reply
    return buildErrorReply(Status::Ok, reply);
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

// Test if an AndroidID received char is valid
static inline bool isAndroidIdValid(char c)
{
    return !(std::isalpha(c) || (c == ' ') || (c == '_') || (c == '-') || (c == '.'));
}

// Callback fetched when a line have been found in SELECT sqlite requests
static int sqlite_getcallback(void *data, int argc, char **argv, char **azColName)
{
    const char searched_field[] = "value";

    if (strncmp(azColName[0], searched_field, sizeof(searched_field)) == 0) {
        // Store AndroidID in a global field
        _android_id_cache = argv[0];
    }
    return 0;
}

static int sqlite_setcallback(void *NotUsed, int argc, char **argv, char **azColName)
{
    // TODO: remove this method
    int i;
    for(i=0; i<argc; i++){
        SLOGD("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    SLOGD("\n");
    return 0;
}

static int sqlite_exec(const char *cmd, sqlite3_callback callback)
{
    const char dbpath[] = "/data/data/com.android.providers.settings/databases/settings.db";
    sqlite3 *db;
    char *errmsg = NULL;

    // Open SQLite DB
    int rc = sqlite3_open(dbpath, &db);
    if (rc) {
        SLOGE("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    // Launch command
    rc = sqlite3_exec(db, cmd, callback, 0, &errmsg);
    if (rc != SQLITE_OK && errmsg != NULL) {
        SLOGE("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }
    // Close SQLite DB
    sqlite3_close(db);
    return 0;
}


/////////////////////////////
// Genymotion daemon verbs //
/////////////////////////////

void Dispatcher::getDeviceId(const Request &request, Reply *reply, Genyd* genyd)
{
    // Read Device ID
    std::string deviceId = getStdStringProperty(DEVICE_ID);
    SLOGD("Getting DeviceID value: '%s'", deviceId.c_str());

    // Build reply
    buildStringReply(reply, deviceId);
}

void Dispatcher::setDeviceId(const Request &request, Reply *reply, Genyd* genyd)
{
    // Read Device ID from request
    std::string deviceId = request.parameter().value().stringvalue();

    SLOGD("Setting DeviceID value to '%s'", deviceId.c_str());

    // Apply new DeviceID by setting property
    property_set(DEVICE_ID, deviceId.c_str());

    // Build standard OK reply;
    buildOkReply(reply);
}

void Dispatcher::getAndroidId(const Request &request, Reply *reply, Genyd* genyd)
{
    SLOGE("Getting AndroidID");

    const char pattern[] = "SELECT value FROM secure WHERE name='android_id';";

    // Read value from database
    if (sqlite_exec(pattern, sqlite_getcallback)) {
        return buildErrorReply(Status::GenericError, reply);
    }

    std::string androidId = _android_id_cache;

    SLOGD("Getting AndroidID value: '%s'", androidId.c_str());

    // Build reply
    buildStringReply(reply, androidId);
}

void Dispatcher::setAndroidId(const Request &request, Reply *reply, Genyd* genyd)
{
    const char pattern[] = "UPDATE secure SET value='%s' WHERE name='android_id';";
    char cmd[255];

    // Read Device ID from request
    std::string androidId = request.parameter().value().stringvalue();

    SLOGD("Setting AndroidID value to '%s'", androidId.c_str());

    // Ensure ID is not too long
    if ((sizeof(pattern) - 2 + androidId.length() + 1) > sizeof(cmd)) {
        SLOGE("setAndroidId: AndroidID is too long");
        return buildErrorReply(Status::InvalidRequest, reply);
    }

    // Ensure ID does not have invalid chars
    if (!(std::find_if(androidId.begin(), androidId.end(), isAndroidIdValid) == androidId.end())) {
        SLOGE("setAndroidId: AndroidID has invalid chars");
        return buildErrorReply(Status::InvalidRequest, reply);
    }

    // Save value in database
    snprintf(cmd, sizeof(cmd), pattern, androidId.c_str());
    if (sqlite_exec(cmd, sqlite_setcallback)) {
        return buildErrorReply(Status::GenericError, reply);
    }

    // Build standard OK reply;
    buildOkReply(reply);
}
