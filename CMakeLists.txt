cmake_minimum_required(VERSION 3.10)

project(FileLocking)

if (NOT MSVC AND NOT MINGW)
    add_subdirectory(flock)
endif()
add_subdirectory(std_shared_lock)
