cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Aya VERSION 0.1 LANGUAGES CXX)

include_directories("C:\\lib\\gsl\\include" "C:\\lib\\gsl\\include\\gsl")
include_directories("C:\\lib\\boost_1_62_0")

find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})



set(CXX_FLAGS
	-std=c++1z
	-target x86_64-w64-windows-gnu
	-Wall
)

set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -DNDEBUG")


add_subdirectory(aya)
#add_subdirectory(sandbox)