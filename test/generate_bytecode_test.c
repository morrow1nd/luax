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

#include <stdlib.h>

#include "luax.h"

int main(int argc, char * argv[])
{
    char * const filepath = TEST_BINARY_DIR "generate_bytecode_test.luax";
    FILE * fp = NULL;
    fp = fopen(filepath, "r");
    if (!fp) {
        printf("Error: can't open file:%s\n", filepath);
        return -1;
    }
    int ret = -1;
    fseek(fp, 0, SEEK_END);
    int filelength = ftell(fp);
    printf("filelength:%d\n", filelength);

    char* data = (char*)lx_malloc(filelength + 1);
    fseek(fp, 0, SEEK_SET);
    if ((ret = fread(data, 1, filelength, fp)) <= 0) {
        printf("Error: can't read file:%s\n", filepath);
        return -1;
    }
    *(data + ret) = '\0';
    fclose(fp);

    lx_parser * p = lx_genBytecode(data, ret);
    lx_helper_dump_bytecode(p->opcodes);
    lx_free(data);
    if (p != NULL) {
        printf("success\n");
    }
    lx_delete_parser(p);

    printf("----- program end -------\n");
#ifdef _WIN32
    system("pause");
#endif
    return 0;
}