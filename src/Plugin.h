#pragma once

#include <string>
#include <vector>

#include "DynamicLibrary.h"
#include "plugin_api.h"

// loaded plugin
class Plugin {
public:
    Plugin(DynamicLibrary&& lib, const PluginFunction* fn, std::string name);

    int arity() const { return fn_->arity; }
    const std::string& name() const { return name_; }

    double call(const std::vector<double>& args) const;

private:
    DynamicLibrary lib_;  // declared first: destroyed last, after fn_ is no longer used
    const PluginFunction* fn_;
    std::string name_;
};