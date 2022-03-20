find_package (Qt6Gui REQUIRED)

add_executable (sample sample.cpp data_provider_sample.hpp data_provider_sample.cpp)

target_link_libraries (sample timeline Qt::Gui)