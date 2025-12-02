cmake_minimum_required(VERSION 3.11)
project(2025-2_RedesII_GB_GraphAnalyzer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ativa o FetchContent
include(FetchContent)

# Baixa e compila o SDL3. Pode ser substituído por uma instalação local do SDL3 como no exemplo comentado abaixo:
### find_package(SDL3 REQUIRED CONFIG HINTS "${CMAKE_SOURCE_DIR}/SDL/lib/cmake/SDL3")
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG origin/main
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(SDL3)
find_package(SDL3 REQUIRED)

# Baixa o ImGui com suporte a docking
FetchContent_Populate(imgui
  URL https://github.com/ocornut/imgui/archive/docking.zip
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/imgui
)

# Find OpenGL
if(WIN32)
    # On Windows, link directly to opengl32 
    set(OPENGL_LIBRARY opengl32)
else()
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBRARY OpenGL::OpenGL)
endif()
# Set SDL3 library variable
set(SDL3_LIBRARY SDL3::SDL3)

# Paths
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui")
set(OUT_APP "${CMAKE_SOURCE_DIR}/src")

# Collect ImGui sources
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# Collect ImGui backends for SDL3 + OpenGL3
set(IMGUI_BACKENDS_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Build SDL3 + OpenGL3 example
add_executable(graph_analyzer
    ${OUT_APP}/main.cpp
    ${IMGUI_SOURCES}
    ${IMGUI_BACKENDS_SOURCES}
)

target_include_directories(graph_analyzer PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(graph_analyzer PRIVATE
    ${SDL3_LIBRARY}
    ${OPENGL_LIBRARY}
)

# Copy SDL3.dll to build directory after build
add_custom_command(TARGET graph_analyzer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_BINARY_DIR}/_deps/sdl3-build/SDL3.dll"
    "${CMAKE_BINARY_DIR}/SDL3.dll"
    COMMENT "Copying SDL3.dll to build directory"
)

# Print configuration summary
message(STATUS "")
message(STATUS "ImGui Examples Build Summary:")
message(STATUS "  SDL3 found: TRUE")
message(STATUS "  OpenGL found: TRUE")
message(STATUS "")
message(STATUS "Targets to build:")
message(STATUS "  - graph_analyzer (always)")

add_custom_command(TARGET graph_analyzer POST_BUILD
    COMMAND ${CMAKE_BINARY_DIR}/graph_analyzer
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} # Optional: set working directory
    COMMENT "Executando..."
)