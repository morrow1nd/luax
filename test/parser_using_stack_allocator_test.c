#include <stdio.h>
#include <stdlib.h>

#define LX_PRINT_MALLOC_INFO 1
extern int lx_call_lx_malloc_number;
extern int lx_call_lx_free_number;

#include "luax.h"


void lx_delete_token_scanner(lx_token_scanner * s);
lx_token_scanner* lx_scan_token(char *source_code, const int source_code_length);

int main(int argc, char * argv[])
{
    char * const filepath = TEST_BINARY_DIR ".tem_test_case.luax";
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

    char* data = (char*)malloc(filelength);
    fseek(fp, 0, SEEK_SET);
    if ((ret = fread(data, 1, filelength, fp)) <= 0) {
        printf("Error: can't read file:%s\n", filepath);
        return -1;
    }
    *(data + ret) = '\0';
    fclose(fp);

    lx_parser * p = lx_genBytecode(data, ret);
    if(p != NULL){
        printf("success\n");
    }
    lx_delete_parser(p);

    printf("----- program end -------\n");
    printf("- Curr call_lx_malloc_times:%d\n", lx_call_lx_malloc_number);
    printf("- Curr call_lx_free_times:%d\n", lx_call_lx_free_number);

    return 0;
}