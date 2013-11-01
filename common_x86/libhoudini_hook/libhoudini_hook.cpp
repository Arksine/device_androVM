#include <stdio.h>
#include <cutils/log.h>
#include <dlfcn.h>
#include <fcntl.h>

namespace houdini {
    bool hookCheckMethod(void *fnPtr);
    void dvmHookPlatformInvoke(void* pEnv, void* clazz, int argInfo, int argc, const int* argv, const char* shorty, void* func, void* pReturn);
    void* hookDlopen(const char* filename, int flag, bool* useHoudini);
    void* hookDlsym(bool useHoudini, void* handle, const char* symbol);
    void  hookCreateActivity(bool useHoudini, void* createActivityFunc, void* activity, void*houdiniActivity, void* savedState, size_t savedStateSize);
    int hookJniOnload(bool useHoudini, void* func, void* jniVm, void* arg);

static bool (*h_dvmHoudini_hookCheckMethod)(void*) = NULL;
static void (*h_dvmHoudini_dvmHookPlatformInvoke)(void*, void*, int, int, const int*, const char*, void*, void*) = NULL;
static void* (*h_dvmHoudini_hookDlopen)(const char*, int, bool*) = NULL;
static void* (*h_dvmHoudini_hookDlsym)(bool, void*, const char*) = NULL;
static void (*h_dvmHoudini_hookCreateActivity)(bool, void*, void*, void*, void*, size_t) = NULL;
static int (*h_dvmHoudini_hookJniOnload)(bool, void*, void*, void*);

static inline void init_dvm_houdini() {
    static void *h_handle = NULL;

    if (h_handle)
        return;

    h_handle = dlopen("/system/lib/libdvm_houdini.so", RTLD_LAZY);
    if (!h_handle) {
        ALOGE("Unable to open libdvm_houdini lib: %s\n", dlerror());
        return;
    }

    *(void **)(&h_dvmHoudini_hookCheckMethod) = dlsym(h_handle, "_ZN7houdini15hookCheckMethodEPv");
    if (!h_dvmHoudini_hookCheckMethod)
        ALOGE("Unable to find hookCheckMethod() function");

    *(void **)(&h_dvmHoudini_dvmHookPlatformInvoke) = dlsym(h_handle, "_ZN7houdini21dvmHookPlatformInvokeEPvS0_iiPKiPKcS0_S0_");
    if (!h_dvmHoudini_dvmHookPlatformInvoke)
        ALOGE("Unable to find dvmHookPlatformInvoke() function");

    *(void **)(&h_dvmHoudini_hookDlopen) = dlsym(h_handle, "_ZN7houdini10hookDlopenEPKciPb");
    if (!h_dvmHoudini_hookDlopen)
        ALOGE("Unable to find hookDlopen() function");

    *(void **)(&h_dvmHoudini_hookDlsym) = dlsym(h_handle, "_ZN7houdini9hookDlsymEbPvPKc");
    if (!h_dvmHoudini_hookDlsym)
        ALOGE("Unable to find hookDlsym() function");

    *(void **)(&h_dvmHoudini_hookCreateActivity) = dlsym(h_handle, "_ZN7houdini18hookCreateActivityEbPvS0_S0_S0_j");
    if (!h_dvmHoudini_hookCreateActivity)
        ALOGE("Unable to find hookCreateActivity() function");

    *(void **)(&h_dvmHoudini_hookJniOnload) = dlsym(h_handle, "_ZN7houdini13hookJniOnloadEbPvS0_S0_");
    if (!h_dvmHoudini_hookJniOnload)
        ALOGE("Unable to find hookJniOnload() function");
}

bool hookCheckMethod(void *fnPtr) {
    init_dvm_houdini();
    if (h_dvmHoudini_hookCheckMethod)
        return (*h_dvmHoudini_hookCheckMethod)(fnPtr);
    return false;
}

void dvmHookPlatformInvoke(void* pEnv, void* clazz, int argInfo, int argc, const int* argv, const char* shorty, void* func, void* pReturn) {
    init_dvm_houdini();
    if (h_dvmHoudini_dvmHookPlatformInvoke) 
        (*h_dvmHoudini_dvmHookPlatformInvoke)(pEnv, clazz, argInfo, argc, argv, shorty, func, pReturn);
}

void* hookDlopen(const char* filename, int flag, bool* useHoudini) {
    init_dvm_houdini();
    *useHoudini = 0;
    if (h_dvmHoudini_hookDlopen)
        return (*h_dvmHoudini_hookDlopen)(filename, flag, useHoudini);
    else
        return dlopen(filename, flag);
}

void* hookDlsym(bool useHoudini, void* handle, const char* symbol) {
    init_dvm_houdini();
    if (h_dvmHoudini_hookDlsym)
        return (*h_dvmHoudini_hookDlsym)(useHoudini, handle, symbol);
    else
        return dlsym(handle, symbol);
}

void hookCreateActivity(bool useHoudini, void* createActivityFunc, void* activity, void*houdiniActivity, void* savedState, size_t savedStateSize) {
    init_dvm_houdini();
    if (h_dvmHoudini_hookCreateActivity)
        return (*h_dvmHoudini_hookCreateActivity)(useHoudini, createActivityFunc, activity, houdiniActivity, savedState, savedStateSize);
}

int hookJniOnload(bool useHoudini, void* func, void* jniVm, void* arg) {
    init_dvm_houdini();
    if (h_dvmHoudini_hookJniOnload)
        return (*h_dvmHoudini_hookJniOnload)(useHoudini, func, jniVm, arg);
    else {
        int (*f)(void*, void*) = (int (*)(void*, void*))func;
        return (*f)(jniVm, NULL);
    }
}

}
