cmake_minimum_required(VERSION 3.10)
set(CXX_STANDARD 11)

add_executable(test_atomic test_atomic.cpp)

find_package(Threads REQUIRED)
target_link_libraries(test_atomic Threads::Threads)