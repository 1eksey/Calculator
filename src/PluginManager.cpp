#include "PluginManager.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

#include "CalcError.h"

namespace fs = std::filesystem;

void PluginManager::warn(const std::string& msg) {
    std::cerr << "[warning] " << msg << '\n';
}

bool PluginManager::isIdentifier(const std::string& s) {
    if (s.empty()) return false;
    if (!(std::isalpha(static_cast<unsigned char>(s[0])) || s[0] == '_')) return false;
    return std::all_of(s.begin(), s.end(), [](char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; });
}

bool PluginManager::isOperatorName(const std::string& s) {
    return s.size() == 1 && std::string("^%&|~@#$:?").find(s[0]) != std::string::npos;
}

void PluginManager::loadFrom(const fs::path& dir) {
    if (!fs::is_directory(dir)) {
        warn("plugins directory not found: " + dir.string());
        return;
    }
    std::vector<fs::path> files;
    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (ext == ".dll") files.push_back(entry.path());
        }
    }
    catch (const fs::filesystem_error& e) {
        warn(std::string("cannot read plugins directory: ") + e.what());
    }
    std::sort(files.begin(), files.end());

    for (const auto& file : files) {
        try {
            loadOne(file);
        }
        catch (const std::exception& e) {
            warn(file.filename().string() + ": " + e.what());
        }
        catch (...) {
            warn(file.filename().string() + ": unknown error");
        }
    }
}

void PluginManager::loadOne(const fs::path& path) {
    DynamicLibrary lib(path);

    FARPROC proc = lib.symbol(PLUGIN_EXPORT_NAME);
    if (!proc) throw CalcError("export '" PLUGIN_EXPORT_NAME "' not found");

    const PluginFunction* fn = reinterpret_cast<GetPluginFunctionFn>(proc)();
    if (!fn || !fn->name || !fn->call) throw CalcError("invalid plugin descriptor");

    std::string name = fn->name;
    const bool isOp = isOperatorName(name);
    if (!isOp && !isIdentifier(name)) throw CalcError("invalid function name '" + name + "'");
    if (isOp && fn->arity != 2) throw CalcError("operator '" + name + "' must have arity 2");
    if (fn->arity < 1 || fn->arity > kMaxArity) throw CalcError("unsupported arity");
    if (plugins_.count(name)) throw CalcError("duplicate function name '" + name + "'");

    plugins_.emplace(name, Plugin(std::move(lib), fn, name));
}

const Plugin* PluginManager::find(const std::string& name) const {
    auto it = plugins_.find(name);
    return it == plugins_.end() ? nullptr : &it->second;
}

std::vector<std::string> PluginManager::names() const {
    std::vector<std::string> result;
    for (const auto& kv : plugins_) result.push_back(kv.first);
    std::sort(result.begin(), result.end());
    return result;
}