#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "Plugin.h"

class PluginManager {
public:
    void loadFrom(const std::filesystem::path& dir);

    const Plugin* find(const std::string& name) const;
    bool isOperator(char c) const { return find(std::string(1, c)) != nullptr; }
    std::size_t size() const { return plugins_.size(); }
    std::vector<std::string> names() const;

private:
    static constexpr int kMaxArity = 8;

    static void warn(const std::string& msg);
    static bool isIdentifier(const std::string& s);
    static bool isOperatorName(const std::string& s);

    void loadOne(const std::filesystem::path& path);

    std::unordered_map<std::string, Plugin> plugins_;
};