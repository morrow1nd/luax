include ("standard_lib/CMakeLists.txt")
#add_subdirectory (standard_lib)

SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set (SRC_LIST 
    base.c
    mem.c
    opcode.c
    parser.c
    parser_callback.c
    object.c
    vm.c
    luax_api.c
    ${STANDARD_LIB_SRC_LIST}
)
set (HEADER_LIST
    luax.h
    base.h
    config.h
    mem.h
    opcode.h
    parser.h
    parser_callback.h
    object.h
    vm.h
    luax_api.h
    ${STANDARD_LIB_HEADER_LIST}
)

add_library (libluax STATIC ${SRC_LIST} ${HEADER_LIST})
set_target_properties (libluax PROPERTIES OUTPUT_NAME "luax")


include_directories (${PROJECT_SOURCE_DIR}/src)

add_executable (luax_exec luax_exec.c)
target_link_libraries (luax_exec libluax)
