#pragma once

// Plugin ABI: plain C types only, so any DLL built with any settings can be loaded.
struct PluginFunction {
    const char* name;   // "sin", "ln", "^" ... (does not depend on the DLL file name)
    int arity;          // number of arguments (operators: exactly 2)
    // Returns 0 on success. On failure returns non-zero and sets *error
    // to a string that stays valid at least until the next call.
    int (*call)(const double* args, double* result, const char** error);
};

#define PLUGIN_EXPORT_NAME "get_plugin_function"
#define PLUGIN_EXPORT extern "C" __declspec(dllexport)

typedef const PluginFunction* (*GetPluginFunctionFn)();