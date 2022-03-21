find_package (Qt6Gui REQUIRED)

add_executable (drawsample sample.cpp data_provider_sample.hpp data_provider_sample.cpp)

target_link_libraries (drawsample timeline Qt::Gui profiler)
target_include_directories (drawsample PUBLIC ${CMAKE_SOURCE_DIR}/prof)