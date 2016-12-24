#ifndef __LX_BASE__H_
#define __LX_BASE__H_

#include <stdlib.h> // malloc, free, strtod
#include <string.h> // memset, memcmp, memcpy
#include <stddef.h> // size_t, NULL
#include <ctype.h> // isdigit, isalpha, isalnum
#include <stdbool.h> // bool, false, true
#include <assert.h> // assert


#include "./config.h"


#if(LX_DEBUG)
# include <stdio.h> // printf, sprintf
#endif


void debug(const char * info);
// log out one line
void debuglog(const char * one_line);
void debuglog_l(int linenum, const char * one_line);

#endif // end of __LX_BASE__H_