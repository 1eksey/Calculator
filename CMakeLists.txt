cmake_minimum_required(VERSION 3.16)
project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(FATAL_ERROR "This project uses the Windows API (LoadLibrary) and builds on Windows only")
endif()

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra)
    if(MINGW)
        add_link_options(-static)   # no dependency on MinGW runtime DLLs
    endif()
endif()

# Shared plugin ABI header
add_library(plugin_api INTERFACE)
target_include_directories(plugin_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Host application
add_executable(calc
    src/main.cpp
    src/Plugin.cpp
    src/PluginManager.cpp
    src/Evaluator.cpp
)
target_link_libraries(calc PRIVATE plugin_api)

# Plugins: every DLL is placed into <dir with calc.exe>/plugins
function(add_calc_plugin name)
    add_library(${name} SHARED plugins/${name}.cpp)
    target_link_libraries(${name} PRIVATE plugin_api)
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:calc>/plugins"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/implibs"
    )
    add_dependencies(calc ${name})
endfunction()

add_calc_plugin(funcsin)
add_calc_plugin(funcdeg)
add_calc_plugin(funcln)