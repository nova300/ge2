cmake_minimum_required(VERSION 3.5.0)
project(engine VERSION 0.1.0)

#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
#SET(CMAKE_UNITY_BUILD TRUE)
#SET(CMAKE_UNITY_BUILD_BATCH_SIZE 16)

set(OpenGL_GL_PREFERENCE GLVND)
find_package( OpenGL REQUIRED )

find_package(OpenMP)

add_subdirectory(lib/GLFW)
include_directories(lib/GLFW/include)

add_library(fixmath lib/libfixmath/fix16.c lib/libfixmath/fix16_exp.c lib/libfixmath/fix16_sqrt.c lib/libfixmath/fix16_trig.c lib/libfixmath/fixarray.c lib/libfixmath/fixmatrix.c lib/libfixmath/fixquat.c lib/libfixmath/fixstring.c lib/libfixmath/fixvector2d.c lib/libfixmath/fixvector3d.c lib/libfixmath/fract32.c lib/libfixmath/uint32.c)

add_library(engine lib/glew.c src/engine.cpp src/graphics.cpp src/shaders.cpp src/geometry.cpp src/event.cpp src/2d.cpp src/legacy/term.cpp src/init.cpp src/skybox.cpp)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(engine ${MATH_LIBRARY})
endif()

target_link_libraries(engine OpenGL::GL glfw fixmath)

if(OpenMP_CXX_FOUND)
    target_link_libraries(engine OpenMP::OpenMP_CXX )
endif()

add_executable(program1 src/programs/boidmode.cpp src/programs/selftest.cpp src/programs/testmode.cpp)
target_link_libraries(program1 engine)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/hlslpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/libfixmath)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/programs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/assets)

add_custom_command(TARGET engine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

#include(./Harden.cmake)