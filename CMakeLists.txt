cmake_minimum_required(VERSION "3.5")

project(SWB)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_executable(SWB 
    Source/Core/Application.cpp
    Source/EntryPoint/Entry.cpp)

target_include_directories(SWB PRIVATE Source)
target_precompile_headers(SWB PRIVATE Source/Pch.h)

set_property(TARGET SWB PROPERTY CXX_STANDARD 26)

if(MSVC)
    target_compile_options(SWB PRIVATE /W4 /WX)
else()
    target_compile_options(SWB PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

