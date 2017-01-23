
set (STANDARD_LIB_SRC_LIST 
    standard_lib/lio.c
    standard_lib/lstring.c
    standard_lib/ltable.c
    standard_lib/lmath.c
)
set (STANDARD_LIB_HEADER_LIST 
    standard_lib/lio.h
    standard_lib/lstring.h
    standard_lib/ltable.h
    standard_lib/lmath.h
)


source_group("Header Files\\Standard Lib" FILES ${STANDARD_LIB_HEADER_LIST})
source_group("Source Files\\Standard Lib" FILES ${STANDARD_LIB_SRC_LIST})
