add_executable(app_template app_template.cpp)

target_link_libraries(app_template STATIC library_name)
target_include_directories(app_template PUBLIC ${library_name_INCLUDES})