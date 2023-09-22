cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0 LANGUAGES C)

#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
#SET(CMAKE_UNITY_BUILD TRUE)
#SET(CMAKE_UNITY_BUILD_BATCH_SIZE 16)

set(OpenGL_GL_PREFERENCE GLVND)
find_package( OpenGL REQUIRED )

find_package(OpenMP)

add_subdirectory(lib/GLFW)
include_directories(lib/GLFW/include)

add_executable(engine lib/glew.c src/engine.c src/graphics.c src/shaders.c src/geometry.c src/event.c src/2d.c src/legacy/term.c src/behaviour.c src/init.c src/programs/boidmode.c src/programs/testmode.c src/programs/selftest.c src/skybox.c)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(engine ${MATH_LIBRARY})
endif()

target_link_libraries(engine OpenGL::GL glfw)

if(OpenMP_C_FOUND)
    target_link_libraries(engine OpenMP::OpenMP_C )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/assets)

add_custom_command(TARGET engine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

#include(./Harden.cmake)