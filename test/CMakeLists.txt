
include_directories (${PROJECT_SOURCE_DIR}/src)

 set(CMAKE_C_FLAGS "-g -Wall")

add_executable (stack_allocator_test stack_allocator_test.c)
target_link_libraries (stack_allocator_test libluax)
add_test (NAME stack_allocator COMMAND stack_allocator_test)

add_executable (parser_using_stack_allocator_test parser_using_stack_allocator_test.c)
target_link_libraries (parser_using_stack_allocator_test libluax)
add_test (NAME parser_using_stack_allocator 
    # WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test 
    COMMAND parser_using_stack_allocator_test
)
