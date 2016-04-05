cmake_minimum_required(VERSION 3.2)
project(arc C CXX)

# custom helper modules
include(${CMAKE_SOURCE_DIR}/gen/dep-johnny/dep-johnny.cmake)

# resolve dependencies
add_subdirectory(dep/glfw)
add_subdirectory(dep/glad)
add_subdirectory(dep/linmath)
add_subdirectory(dep/eigen)

# TODO: add_librar(fem)...

# specify executable: sources, include directories, and library dependencies
add_executable(arc ${CMAKE_SOURCE_DIR}/src/bin/arc.cpp
                   ${CMAKE_SOURCE_DIR}/src/gui/gui.cpp
                   ${CMAKE_SOURCE_DIR}/src/graphics/shader.cpp
                   ${CMAKE_SOURCE_DIR}/src/graphics/window.cpp)
target_include_directories(arc PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/adhoc>
                                      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/control>
                                      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/events>
                                      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/fem>
                                      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/graphics>
                                      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/gui>)
target_link_libraries(arc glfw glad linmath eigen)

if (WIN32)
    # copy dlls to executable directory for running in build tree
    add_custom_command(TARGET arc POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:glfw> $<TARGET_FILE_DIR:arc>)
endif()
