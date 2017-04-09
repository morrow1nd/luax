#ifndef __LX_BASE__H_
#define __LX_BASE__H_

#include <stdlib.h> // malloc, free, strtod, strtof
#include <string.h> // memset, memcmp, memcpy
#include <stddef.h> // size_t, NULL
#include <ctype.h> // isdigit, isalpha, isalnum
#include <stdbool.h> // bool, false, true
#include <assert.h> // assert


#include "./config.h"
#include "./gen_config.h.in"


#if(LX_DEBUG)
# include <stdio.h> // printf, sprintf, fprintf
#endif
# include <stdio.h> // todo:remove this


void debug(const char * info);
// log out one line
void debuglog(const char * one_line);
void debuglog_l(int linenum, const char * one_line);
void debuglog_luax_str(int text_len, char * ptr);


#define UNUSED_ARGUMENT(argv)


#ifdef _WIN32
#pragma warning(disable:4996)
#endif

#endif // end of __LX_BASE__H_