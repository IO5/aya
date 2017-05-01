project(aya VERSION 0.1 LANGUAGES CXX)

file(GLOB_RECURSE SOURCES *.cpp)

add_library(aya ${SOURCES})

include_directories (${PROJECT_SOURCE_DIR})

