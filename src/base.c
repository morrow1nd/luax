#include "./base.h"

void debug(const char * info)
{
#if(LX_DEBUG)
    printf("%s", info);
#endif
}

void debuglog(const char * one_line)
{
#if(LX_DEBUG)
    printf("%s\n", one_line);
#endif
}


void debuglog_l(int linenum, const char * one_line)
{
#if(LX_DEBUG)
    printf("%d: %s\n", linenum, one_line);
#endif
}

void debuglog_luax_str(int text_len, char * ptr)
{
#if(LX_DEBUG)
    assert(text_len >= 0 && "debuglog_luax_str: argument text_len must >= 0");
    char backup = *(ptr + text_len);
    *(ptr + text_len) = '\0';
    printf("%s\n", ptr);
    *(ptr + text_len) = backup;
#endif
}