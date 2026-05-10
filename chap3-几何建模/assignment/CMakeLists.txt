cmake_minimum_required(VERSION 3.10)

project(chap3-assignment C CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMMON_LIBS glut GL GLU)

add_executable(task1 平面三次多项式.cpp)
add_executable(bezier bezier.cpp)
add_executable(loop loop细分.cpp)

target_link_libraries(task1 PRIVATE ${COMMON_LIBS})
target_link_libraries(bezier PRIVATE ${COMMON_LIBS})
target_link_libraries(loop PRIVATE ${COMMON_LIBS})
