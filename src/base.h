#ifndef __LX_BASE__H_
#define __LX_BASE__H_

#include <stdlib.h> // malloc, free, strtod
#include <string.h> // memset, memcmp, memcpy
#include <stddef.h> // size_t, NULL
#include <ctype.h> // isdigit, isalpha, isalnum
#include <stdbool.h> // bool, false, true
#include <assert.h> // assert

#if(LX_DEBUG)
# include <stdio.h> // printf
#endif

void debug(const char * info)
{
    #if(LX_DEBUG)
    printf("%s", info);
    #endif
}

#endif // end of __LX_BASE__H_