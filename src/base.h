#ifndef __LX_BASE__H_
#define __LX_BASE__H_

#include <stdlib.h> // malloc, free, strtod, strtof
#include <string.h> // memset, memcmp, memcpy, memmove
#include <stddef.h> // size_t, NULL
#include <ctype.h> // isdigit, isalpha, isalnum
#include <stdbool.h> // bool, false, true
#include <assert.h> // assert
# include <stdio.h> // printf, sprintf, fprintf

#include "./config.h"
#include "./gen_config.h.in"


#ifdef _WIN32
#pragma warning(disable:4996) // sprintf
#pragma warning(disable:4255) // no function prototype given: converting '()' to '(void)'
#pragma warning(disable:4820) // '3' bytes padding added after data member 'xxx'
#endif

void version();

void debug(const char * info);
// log out one line
void debuglog(const char * one_line);
void debuglog_l(int linenum, const char * one_line);
void debuglog_luax_str(int text_len, char * ptr);

#define UNUSED_ARGUMENT(argv) (argv)


#endif // end of __LX_BASE__H_