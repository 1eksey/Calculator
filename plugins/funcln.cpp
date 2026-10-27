#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>

//#include "../hw1/plugin_api.h"
#include "plugin_api.h"

namespace {

    int ln_call(const double* args, double* result, const char** error) {
        static thread_local std::string message;
        try {
            if (args[0] <= 0.0) throw std::domain_error("argument must be > 0");
            *result = std::log(args[0]);
            return 0;
        }
        catch (const std::exception& e) {
            message = e.what();
            *error = message.c_str();
            return 1;
        }
        catch (...) {
            *error = "unknown error";
            return 1;
        }
    }

    const PluginFunction kFunction = { "ln", 1, &ln_call };

} // namespace

PLUGIN_EXPORT const PluginFunction* get_plugin_function() { return &kFunction; }