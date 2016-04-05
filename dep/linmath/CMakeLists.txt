johnny_get_dep(URL https://github.com/datenwolf/linmath.h.git
               DIR ${CMAKE_CURRENT_LIST_DIR}/linmath.git)
add_library(linmath INTERFACE)
target_include_directories(linmath INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/linmath.git>)