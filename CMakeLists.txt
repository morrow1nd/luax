cmake_minimum_required (VERSION 2.8)

project (luax)
message (STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message (STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})

add_subdirectory (src)

enable_testing ()
add_subdirectory (test)


FILE(WRITE ${PROJECT_SOURCE_DIR}/src/gen_config.h.in "// Cmake auto generate\n")
FILE(APPEND ${PROJECT_SOURCE_DIR}/src/gen_config.h.in 
    "#define TEST_BINARY_DIR \"${PROJECT_SOURCE_DIR}/test/\"\n"
)
