cmake_minimum_required(VERSION 3.10)

project(chap2-assignment C CXX)

set(CMAKE_CXX_STANDARD 14)

# Minimal: assume system packages (glfw, glew, glm, OpenGL) are installed
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/liang-b-others)

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/liang-b-others/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/liang-b-others)

add_executable(DDA DDA.cpp)
add_executable(Bresenham Bresenham.cpp)
add_executable(liang-b liang-b.cpp)

# Link against common Linux names. Map: opengl32 -> GL
set(COMMON_LIBS glad glfw GLEW GL)

target_link_libraries(DDA PRIVATE ${COMMON_LIBS})
target_link_libraries(Bresenham PRIVATE ${COMMON_LIBS})
target_link_libraries(liang-b PRIVATE ${COMMON_LIBS})
