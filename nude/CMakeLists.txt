file(GLOB_RECURSE nude_SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" src/*.cpp)
file(GLOB_RECURSE nude_HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.h)
file(GLOB_RECURSE nude_HEADERPP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.hpp)

set(nude_SRC
        ${nude_SRC_FILES}
        ${nude_HEADER_FILES}
        ${nude_HEADERPP_FILES}
)

set(nude_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
CACHE INTERNAL "")

add_library(nude STATIC ${nude_SRC})

target_include_directories(nude PUBLIC ${nude_INCLUDES})
target_link_libraries(nude PRIVATE)