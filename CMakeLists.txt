cmake_minimum_required(VERSION 3.20)

project(3D-test
    VERSION 1.0
    DESCRIPTION "3d rendering via a 2d map mango bluds unite"
    LANGUAGES CXX
)

add_executable(3d-test
    src/main.cpp
    src/mesh.cpp
    src/primitives.cpp
		src/renderer.cpp
		src/player.cpp
)

target_include_directories(3d-test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(glfw3 3.3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(3d-test PRIVATE glfw OpenGL::GL)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(3d-test PRIVATE -Wall -Wextra -Wpedantic)
endif()
