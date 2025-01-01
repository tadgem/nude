CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
project(nude_project)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(submodules/sdl)
add_subdirectory(submodules/zep)

add_subdirectory(nude)
add_subdirectory(nude_app)
add_subdirectory(nude_tests)
