johnny_get_dep(URL https://github.com/RLovelett/eigen.git
               DIR ${CMAKE_CURRENT_LIST_DIR}/eigen.git)
add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/eigen.git>)