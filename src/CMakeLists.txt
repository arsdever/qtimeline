set (SOURCES timeline.cpp)
set (HEADERS timeline.hpp)

find_package (Qt6Widgets REQUIRED)

add_library (timeline STATIC ${HEADERS} ${SOURCES})

target_link_libraries (timeline Qt::Widgets)
target_include_directories (timeline
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)