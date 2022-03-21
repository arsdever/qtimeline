set (SOURCES timeline.cpp single_timeline.cpp timeline_item_widget.cpp)
set (HEADERS timeline.hpp single_timeline.hpp timeline_item_widget.hpp timeline_item.hpp timeline_data_provider.hpp)

find_package (Qt6Widgets REQUIRED)

add_library (timeline STATIC ${HEADERS} ${SOURCES})

target_link_libraries (timeline Qt::Widgets)
target_include_directories (timeline
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)