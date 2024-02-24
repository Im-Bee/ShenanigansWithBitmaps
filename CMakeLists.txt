cmake_minimum_required(VERSION "3.5")

project(SWB)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)


include_directories(ShenanigansWithBitmaps/Source)
add_executable(SWB 
    ShenanigansWithBitmaps/Source/Core/Application.cpp 
    ShenanigansWithBitmaps/Source/Core/Bitmap.cpp 
    ShenanigansWithBitmaps/Source/Core/HexEditor.cpp 
    ShenanigansWithBitmaps/Source/EntryPoint/Win32/Entry.cpp)

target_precompile_headers(SWB PRIVATE ShenanigansWithBitmaps/Source/Pch.h)
target_link_libraries(SWB lm)

set_property(TARGET SWB PROPERTY CXX_STANDARD 26)

