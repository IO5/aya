project(aya VERSION 0.1 LANGUAGES CXX)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.hpp)

add_executable(tests ${SOURCES})

include_directories ("../aya")
include_directories (${GTEST_INCLUDE_DIRS})

target_link_libraries(tests ${GTEST_BOTH_LIBRARIES} aya)

gtest_add_tests(tests "" ${HEADERS})

