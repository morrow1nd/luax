#include <stdio.h>

#include "luax.h"

void usage(char* argv[])
{
    printf("usage: %s [option] [script]\n"
        "Available options are:\n"
        "  -e stat    execute string 'stat'\n"
        "  -i         enter interaction mode after executing 'script'\n"
        "  -v         show version information\n"
        "  -          execute stdin and stop handling options\n"
        "  -h         show help info\n"
        "\n"
        , argv[0]
    );
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        
    }
    return 0;
}