cmake_minimum_required(VERSION 3.10)

project(chap4-assignment C CXX)

set(CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

add_executable(smooth smooth.cpp)
add_executable(simplify simplify.cpp)

# target_link_libraries(smooth PRIVATE ${GLUT_LIBRARIES})
# target_link_libraries(simplify PRIVATE ${GLUT_LIBRARIES})
# target_link_libraries(smooth PRIVATE OpenMeshCore OpenMeshTools)
# target_link_libraries(simplify PRIVATE OpenMeshCore OpenMeshTools)
target_link_libraries(smooth PRIVATE glut)
target_link_libraries(simplify PRIVATE glut)
target_link_libraries(smooth PRIVATE OpenMeshCore)
target_link_libraries(simplify PRIVATE OpenMeshCore)
target_link_libraries(smooth PRIVATE GL)
target_link_libraries(simplify PRIVATE GL)
