#include <parser.h>
#include <mem.h>

#include <stdio.h>


int main(int argc, char *argv[])
{
    printf("%s needs to be finished\n", argv[0]);

    // test for the luax lib
    lx_free(lx_malloc(4));

    return 0;
}