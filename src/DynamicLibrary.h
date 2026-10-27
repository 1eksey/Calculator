#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <filesystem>
#include <string>
#include <utility>

#include "CalcError.h"

// RAII HMODULE
class DynamicLibrary {
public:
    explicit DynamicLibrary(const std::filesystem::path& path)
        : handle_(::LoadLibraryW(path.c_str())) {
        if (!handle_)
            throw CalcError("cannot load library (Windows error " + std::to_string(::GetLastError()) + ")");
    }
    ~DynamicLibrary() {
        if (handle_) ::FreeLibrary(handle_);
    }

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;
    DynamicLibrary(DynamicLibrary&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) { }
    DynamicLibrary& operator=(DynamicLibrary&&) = delete;

    FARPROC symbol(const char* name) const { return ::GetProcAddress(handle_, name); }

private:
    HMODULE handle_ = nullptr;
};