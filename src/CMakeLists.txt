cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(aya VERSION 0.1 LANGUAGES CXX)

find_package(Boost REQUIRED)
#add_library(boost INTERFACE IMPORTED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories("/usr/include/gsl")


set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++1z")

set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -DNDEBUG")

add_subdirectory(aya)
add_subdirectory(sandbox)

find_package(GTest REQUIRED)
if (GTEST_FOUND)
    enable_testing()
    add_subdirectory(test)
endif()

