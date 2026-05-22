cmake_minimum_required(VERSION 3.10)

project(chap5-assignment C CXX)

set(CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

add_executable(phong phong光照模型.cpp)
add_executable(phong_cube phong立方体.cpp)
add_executable(texture_mapping 纹理映射/main.cpp)

add_library(glad STATIC glad.c)

target_link_libraries(phong PRIVATE GL)
target_link_libraries(phong_cube PRIVATE GL)
target_link_libraries(texture_mapping PRIVATE GL)
target_link_libraries(phong PRIVATE glfw)
target_link_libraries(phong_cube PRIVATE glfw)
target_link_libraries(texture_mapping PRIVATE glfw)
target_link_libraries(phong PRIVATE glad)
target_link_libraries(phong_cube PRIVATE glad)
target_link_libraries(texture_mapping PRIVATE glad)