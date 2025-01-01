CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
project(nude_project)
set(CMAKE_CXX_STANDARD 17)

set(SUBMODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules CACHE INTERNAL "")
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third-party CACHE INTERNAL "")

## Vendor Options
set(SDL_SHARED OFF CACHE INTERNAL "")
set(SDL_STATIC TRUE CACHE INTERNAL TRUE)

add_subdirectory(submodules/sdl)
add_subdirectory(submodules/zep)

add_subdirectory(nude)
add_subdirectory(nude_app)
add_subdirectory(nude_tests)
