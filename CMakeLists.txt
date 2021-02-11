cmake_minimum_required(VERSION 3.10)

project(FileLocking)

if (LINUX)
    add_subdirectory(flock)
endif()
add_subdirectory(std_shared_lock)