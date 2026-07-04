cmake_minimum_required(VERSION 3.20)

project(RePlex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(plugin SHARED 
    plugin/plugin.cpp
)

target_include_directories(plugin PUBLIC plugin)

add_executable(host
    host/main.cpp
    host/plugin_loader.cpp
)

target_link_libraries(host PRIVATE dl)