add_executable(nude_TESTS nude_tests.cpp)

target_link_libraries(nude_TESTS PRIVATE nude)
target_include_directories(nude_TESTS PUBLIC ${nude_INCLUDES})