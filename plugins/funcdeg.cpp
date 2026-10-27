#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>

//#include "../hw1/plugin_api.h"
#include "plugin_api.h"

namespace {

    int pow_call(const double* args, double* result, const char** error) {
        static thread_local std::string message;
        try {
            const double r = std::pow(args[0], args[1]);
            if (!std::isfinite(r)) throw std::domain_error("result is undefined or too large");
            *result = r;
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

    const PluginFunction kFunction = { "^", 2, &pow_call };

} // namespace

PLUGIN_EXPORT const PluginFunction* get_plugin_function() { return &kFunction; }