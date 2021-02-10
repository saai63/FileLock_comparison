cmake_minimum_required(VERSION 3.10)

SET(CXX_STANDARD 14)

add_executable(test_flock test_flock.cpp)
target_compile_options(test_flock PRIVATE "-g")

find_package(Threads REQUIRED)
target_link_libraries(test_flock Threads::Threads)