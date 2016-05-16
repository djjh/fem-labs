johnny_get_dep(URL https://github.com/Dav1dde/glad.git
               DIR ${CMAKE_CURRENT_LIST_DIR}/glad.git)
find_package(Git REQUIRED)

# apply custom patch to ignore ssl errors from urllib2
execute_process(COMMAND ${GIT_EXECUTABLE}
                        --git-dir=${CMAKE_CURRENT_LIST_DIR}/glad.git/.git
                        --work-tree=${CMAKE_CURRENT_LIST_DIR}/glad.git
                        apply  ${CMAKE_CURRENT_LIST_DIR}/0001-Ignoring-SSL-certificates.patch
                        --ignore-space-change
                        --ignore-whitespace
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/glad.git
                RESULT_VARIABLE RESULT
                OUTPUT_VARIABLE OUTPUT
                ERROR_VARIABLE ERROR)

# if (${RESULT})
#     message("ERROR apply ssl ignore certificates patch to glad.git")
#     message("STDOUT OUTPUT: ${OUTPUT}")
#     message("STDERR OUTPUT ${ERROR}")
#     message(FATAL_ERROR "RETURN CODE: ${RESULT}")
# endif()

add_subdirectory(glad.git)
if (WIN32)
    target_include_directories(glad PUBLIC $<TARGET_FILE_DIR:glad>/../include)
else()
    target_include_directories(glad PUBLIC $<TARGET_FILE_DIR:glad>/include)
endif()