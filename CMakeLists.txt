cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0)

#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
#SET(CMAKE_UNITY_BUILD TRUE)
#SET(CMAKE_UNITY_BUILD_BATCH_SIZE 16)

set(OpenGL_GL_PREFERENCE GLVND)
find_package( OpenGL REQUIRED )

find_package(OpenMP)

add_subdirectory(lib/GLFW)
include_directories(lib/GLFW/include)

add_executable(engine lib/glew.c src/engine.cpp src/graphics.cpp src/shaders.cpp src/geometry.cpp src/event.cpp src/2d.cpp src/legacy/term.cpp src/init.cpp src/skybox.cpp src/programs/boidmode.cpp)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(engine ${MATH_LIBRARY})
endif()

target_link_libraries(engine OpenGL::GL glfw)

if(OpenMP_C_FOUND)
    target_link_libraries(engine OpenMP::OpenMP_CXX )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/programs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/hlslpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/assets)

add_custom_command(TARGET engine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

#include(./Harden.cmake)