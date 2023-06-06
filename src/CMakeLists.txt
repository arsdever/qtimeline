set(SOURCES qtimeline_view.cpp qtimeline_item_delegate.cpp)
set(HEADERS qtimeline_view.hpp qtimeline_item_delegate.hpp)

find_package(Qt6 REQUIRED Widgets)

add_library(qtimeline STATIC ${HEADERS} ${SOURCES})

target_link_libraries(qtimeline Qt::Widgets)
target_include_directories(qtimeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
