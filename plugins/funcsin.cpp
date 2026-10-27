#define _USE_MATH_DEFINES
#include <cmath>
#include <exception>
#include <string>

//#include "../hw1/plugin_api.h"
#include "plugin_api.h"

namespace {

    int sin_call(const double* args, double* result, const char** error) {
        static thread_local std::string message;
        try {
            double x = std::fmod(args[0], 360.0); // (-360, 360)
            if (x > 180.0) x -= 360.0;
            else if (x < -180.0) x += 360.0; // [-180, 180]
            if (x > 90.0) x = 180.0 - x;
            else if (x < -90.0) x = -180.0 - x; // [-90, 90]

            *result = std::sin(x * M_PI / 180.0);
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

    const PluginFunction kFunction = { "sin", 1, &sin_call };

} // namespace

PLUGIN_EXPORT const PluginFunction* get_plugin_function() { return &kFunction; }