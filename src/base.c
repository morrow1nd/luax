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