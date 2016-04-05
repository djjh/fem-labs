# download at generation time
johnny_get_dep(URL https://github.com/glfw/glfw.git
               DIR ${CMAKE_CURRENT_LIST_DIR}/glfw.git)

# import targets from glfw
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" ON)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glfw.git)

# need to fix rpath for our purposes...
if (APPLE)
    set_target_properties(glfw PROPERTIES INSTALL_NAME_DIR "@rpath")

    # glfw doesn't provide these when building shared libs...
    find_library(OPENGL_FRAMEWORK OpenGL)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
    find_library(CARBON_FRAMEWORK Carbon)

    set(glfw glfw
             "${OPENGL_FRAMEWORK}"
             "${COCOA_FRAMEWORK}"
             "${IOKIT_FRAMEWORK}"
             "${CORE_FOUNDATION_FRAMEWORK}"
             "${CORE_VIDEO_FRAMEWORK}"
             "${CARBON_FRAMEWORK}"
             PARENT_SCOPE)
endif()
