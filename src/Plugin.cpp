#include "Plugin.h"

#include <cmath>
#include <exception>
#include <utility>

#include "CalcError.h"

Plugin::Plugin(DynamicLibrary&& lib, const PluginFunction* fn, std::string name) : lib_(std::move(lib)), fn_(fn), name_(std::move(name)) { }

double Plugin::call(const std::vector<double>& args) const {
    double result = 0.0;
    const char* error = nullptr;
    int status = 0;
    try {
        status = fn_->call(args.data(), &result, &error);
    }
    catch (const std::exception& e) {
        throw CalcError(name_ + ": " + e.what());
    }
    catch (...) {
        throw CalcError(name_ + ": unknown exception in plugin");
    }
    if (status != 0)
        throw CalcError(name_ + ": " + (error ? error : "plugin reported an error"));
    if (!std::isfinite(result))
        throw CalcError(name_ + ": result is not a finite number");
    return result;
}