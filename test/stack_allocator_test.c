#include <parser.h>
#include <mem.h>

#include <stdio.h>


int main(int argc, char *argv[])
{
    printf("This is test for stack_allocator\n");

    // test for the luax lib
    lx_free(lx_malloc(4));

    return 0;
}