add_executable(nude_app nude_app.cpp)

target_link_libraries(nude_app PRIVATE nude)
target_include_directories(nude_app PUBLIC ${nude_INCLUDES})