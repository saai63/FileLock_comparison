cmake_minimum_required(VERSION 3.10)
set(CXX_STANDARD 14)

add_executable(test_std_shared_lock test_std_shared_lock.cpp)

find_package(Threads REQUIRED)
target_link_libraries(test_std_shared_lock Threads::Threads)