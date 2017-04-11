#include <stdio.h>
#include <stdlib.h>

#include "luax.h"

#if(LX_PRINT_MALLOC_INFO)
extern int lx_call_lx_malloc_number;
extern int lx_call_lx_free_number;
#endif


int main(int argc, char * argv[])
{
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);
    char * const filepath = TEST_BINARY_DIR "parser_using_stack_allocator_test.luax";
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
    lx_free(data);
    if(p != NULL){
        printf("success\n");
    }
    lx_delete_parser(p);

    printf("----- program end -------\n");
#if(LX_PRINT_MALLOC_INFO)
    printf("- Curr call_lx_malloc_times:%d\n", lx_call_lx_malloc_number);
    printf("- Curr call_lx_free_times:%d\n", lx_call_lx_free_number);
#else
    printf("Please set LX_PRINT_MALLOC_INFO to 1\n");
#endif
#ifdef _WIN32
    system("pause");
#endif
    return 0;
}