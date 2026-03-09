cmake_minimum_required(VERSION 3.10)

project(DDA)
project(Bresenham)
project(liang-b)

# set(CXX_STANDARD 14)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include paths for GLFW/GLEW (adjust if installed elsewhere)
include_directories("C:/packages/glew-2.3.1/include")
include_directories("C:/packages/glfw-3.2.1.bin.WIN64/include")
include_directories("C:/packages/glad/include")
include_directories("C:/packages/glm")

# Use the MinGW import library for GLFW (libglfw3.a)
set(GLFW_LIBRARIES "C:/packages/glfw-3.2.1.bin.WIN64/lib-mingw-w64/libglfw3.a")
# NOTE: the prebuilt GLEW under C:/packages contains MSVC .lib files which are
# not compatible with MinGW. If you have a MinGW-compatible GLEW import library
# (e.g. libglew32.a), set GLEW_LIBRARIES to its full path. Leave empty otherwise.
set(GLEW_LIBRARIES "C:/packages/glew-2.3.1/lib/Release/x64/glew32s.lib")

# 添加可执行文件
add_executable(DDA DDA.cpp)

# 链接：GLFW + (optional) GLEW + system OpenGL
target_link_libraries(DDA PRIVATE ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} opengl32)

add_executable(Bresenham Bresenham.cpp)
target_link_libraries(Bresenham PRIVATE ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} opengl32)

add_executable(liang-b liang-b.cpp)
target_link_libraries(liang-b PRIVATE ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} opengl32)