
include_directories (${PROJECT_SOURCE_DIR}/src)

 set(CMAKE_C_FLAGS "-Wall")

add_executable (stack_allocator_test stack_allocator_test.c)
target_link_libraries (stack_allocator_test libluax)
add_test (NAME stack_allocator COMMAND stack_allocator_test)

add_executable (parser_using_stack_allocator_test parser_using_stack_allocator_test.c)
target_link_libraries (parser_using_stack_allocator_test libluax)
add_test (NAME parser_using_stack_allocator 
    # WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test 
    COMMAND parser_using_stack_allocator_test
)

add_executable (generate_bytecode_test generate_bytecode_test.c)
target_link_libraries (generate_bytecode_test libluax)
add_test (NAME generate_bytecode
    COMMAND generate_bytecode_test
)