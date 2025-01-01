file(GLOB_RECURSE library_name_SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" src/*.cpp)
file(GLOB_RECURSE library_name_HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.h)
file(GLOB_RECURSE library_name_HEADERPP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.hpp)

set(library_name_SRC
        ${library_name_SRC_FILES}
        ${library_name_HEADER_FILES}
        ${library_name_HEADERPP_FILES}
)

set(library_name_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
CACHE INTERNAL "")

add_library(library_name STATIC ${library_name_SRC})

target_include_directories(library_name PUBLIC ${library_name_INCLUDES})
target_link_libraries(library_name STATIC a)