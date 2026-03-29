cmake_minimum_required(VERSION 3.10)

project(task1)
project(bezier)
project(loop)

set(CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLUT_LIBRARIES "C:/packages/freeglut/lib/x64/freeglut.lib")

# Include paths
include_directories("C:/packages/freeglut/include/GL")

add_executable(task1 平面三次多项式.cpp)
add_executable(bezier bezier.cpp)
add_executable(loop loop细分.cpp)

target_link_libraries(task1 PRIVATE ${GLUT_LIBRARIES})
target_link_libraries(bezier PRIVATE ${GLUT_LIBRARIES})
target_link_libraries(loop PRIVATE ${GLUT_LIBRARIES})
