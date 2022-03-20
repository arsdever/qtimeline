cmake_minimum_required (VERSION 3.13)

set (CMAKE_CXX_STANDARD 20)

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)

include ($ENV{VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)

option (ENABLE_TESTING "Enable the testing of the project" ON)
project (timeline)

add_subdirectory (src)
add_subdirectory (sample)

if (ENABLE_TESTING)
    enable_testing ()
    add_subdirectory (test)
endif (ENABLE_TESTING)