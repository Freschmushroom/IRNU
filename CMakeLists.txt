
cmake_minimum_required(VERSION 2.8)
project(IRNU)

find_package(Threads)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -D__client__")
    #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

add_subdirectory(src)