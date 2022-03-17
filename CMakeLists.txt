cmake_minimum_required (VERSION 3.13)

set (CMAKE_CXX_STANDARD 20)

option (ENABLE_TESTING "Enable the testing of the project" ON)
project (qtimeline)

add_subdirectory (src)

if (ENABLE_TESTING)
    enable_testing ()
    add_subdirectory (test)
endif (ENABLE_TESTING)