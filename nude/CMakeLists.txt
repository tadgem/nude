file(GLOB_RECURSE nude_SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" src/*.cpp)
file(GLOB_RECURSE nude_HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.h)
file(GLOB_RECURSE nude_HEADERPP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.hpp)

set(nude_SRC
        ${nude_SRC_FILES}
        ${nude_HEADER_FILES}
        ${nude_HEADERPP_FILES}
        ${THIRD_PARTY_DIR}/imgui/imgui.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdlrenderer3.cpp
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdl3.cpp
)

set(nude_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SUBMODULE_DIR}/sdl/include
        ${SUBMODULE_DIR}/zep/include
        ${THIRD_PARTY_DIR}/imgui
        ${THIRD_PARTY_DIR}/clip
CACHE INTERNAL "")

add_library(nude STATIC ${nude_SRC})

target_include_directories(nude PUBLIC ${nude_INCLUDES})
target_link_libraries(nude PRIVATE SDL3-static Zep clip)