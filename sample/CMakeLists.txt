find_package(Qt6 REQUIRED Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(drawsample sample.cpp)

target_link_libraries(drawsample qtimeline Qt::Gui)
