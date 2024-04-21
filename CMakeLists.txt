cmake_minimum_required(VERSION 3.25)
project(voxela)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

include_directories(${OpenGL_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/includes")
include_directories("${CMAKE_SOURCE_DIR}/includes/lib")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
set(GLAD_GL "${CMAKE_SOURCE_DIR}/src")

add_subdirectory(.\\glfw-3.4)

file(GLOB_RECURSE sources
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${GLAD_GL}/*.c"
)

add_executable(voxela ${sources})

target_link_libraries(voxela glfw)
target_link_libraries(voxela OpenGL::GL)
