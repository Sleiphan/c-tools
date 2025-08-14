
set(EXECUTABLE "io_uring_test")
add_executable(${EXECUTABLE} io_uring_test.c)
target_link_libraries(${EXECUTABLE} ${LIBRARY} uring)
set_target_properties(${EXECUTABLE}
    PROPERTIES 
    LINKER_LANGUAGE C
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin
)

set(EXECUTABLE "io_uring_server_test")
add_executable(${EXECUTABLE} io_uring_server_test.c)
target_link_libraries(${EXECUTABLE} ${LIBRARY} uring)
set_target_properties(${EXECUTABLE}
    PROPERTIES 
    LINKER_LANGUAGE C
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin
)
