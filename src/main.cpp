#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>

#include "CalcError.h"
#include "Evaluator.h"
#include "PluginManager.h"

namespace fs = std::filesystem;

static fs::path executableDir() {
    std::wstring buf(MAX_PATH, L'\0');
    for (;;) {
        DWORD n = ::GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
        if (n == 0) throw CalcError("cannot determine executable path");
        if (n < buf.size()) { buf.resize(n); break; }
        buf.resize(buf.size() * 2);
    }
    return fs::path(buf).parent_path();
}

int main() {
    try {
        ::SetErrorMode(SEM_FAILCRITICALERRORS); // no system dialogs for broken DLLs

        PluginManager plugins;
        plugins.loadFrom(executableDir() / "plugins");

        if (plugins.size() == 0) {
            std::cerr << "[warning] no plugins loaded: only + - * / () are available\n";
        }
        else {
            std::cout << "Loaded plugins:";
            for (const auto& n : plugins.names()) std::cout << ' ' << n;
            std::cout << '\n';
        }
        std::cout << "Enter an expression ('exit' to quit)\n";
        std::cout << std::setprecision(10);

        std::string line;
        while (std::cout << "> ", std::getline(std::cin, line)) {
            if (line.find_first_not_of(" \t\r\n") == std::string::npos) continue;
            if (line == "exit" || line == "quit") break;
            try {
                std::cout << Evaluator(plugins, line).evaluate() << '\n';
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "Fatal: unknown error\n";
        return 1;
    }
    return 0;
}