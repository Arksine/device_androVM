
#include <cutils/properties.h>
#include <sys/wait.h>

#include "libgenyd.hpp"
#define __NO_PROTO
#include "global.hpp"

// Singleton object
LibGenyd LibGenyd::instance = LibGenyd();

// Constructor
LibGenyd::LibGenyd(void)
{
    // Populate callbacks links
    initBatteryCallbacks();
}

// Destructor
LibGenyd::~LibGenyd(void)
{
}

// Get singleton object
LibGenyd& LibGenyd::getInstance(void)
{
    return instance;
}

// Store current value to Genymotion cache
void LibGenyd::storeCurrentValue(const char *key,
                                 const char *buff,
                                 const size_t size)
{
    char value[PROPERTY_VALUE_MAX];

    // Store value
    SLOGD("Forking");
    pid_t p_id = fork();

    if (p_id < 0) {
        SLOGE("Unable to fork.");
        return;
    } else if (p_id == 0) {
        SLOGD("Launching execl");
        execl("/system/bin/androVM_setprop",
              "androVM_setprop", key, buff, NULL);
        return;
    } else {
        int status = 0;
        wait(&status);
        SLOGD("Process exits with status %d", WEXITSTATUS(status));
    }

    property_get(key, value, VALUE_USE_REAL);

    SLOGD("Cached value [%s] = '%s'", key, value);
}


// Overload /proc values with genymotion configuration
int LibGenyd::getValueFromProc(const char *path, char *buf, size_t size)
{
    SLOGI("Searching system value from '%s': '%s'", path, buf);

    LibGenyd &instance = LibGenyd::getInstance();

    LibGenyd::t_dispatcher_member sensorCallback = instance.getSensorCallback(path);

    if (sensorCallback) {
        return (instance.*sensorCallback)(path, buf, size);
    }
    SLOGI("%s No callback found. Returning", __FUNCTION__);
    return -1;
}

bool LibGenyd::useRealValue(const char *key)
{
    char property[PROPERTY_VALUE_MAX];
    property_get(key, property, VALUE_USE_REAL);
    return !strcmp(property, VALUE_USE_REAL);
}

LibGenyd::t_dispatcher_member LibGenyd::getSensorCallback(const char *path)
{
    std::map<std::string, LibGenyd::t_dispatcher_member>::iterator begin = sensor_callbacks.begin();
    std::map<std::string, LibGenyd::t_dispatcher_member>::iterator end = sensor_callbacks.end();

    std::string haystack(path);

    while (begin != end) {
        // if haystack starts with
        if (haystack.find(begin->first) == 0)
            return begin->second;
        ++begin;
    }

    return NULL;
}