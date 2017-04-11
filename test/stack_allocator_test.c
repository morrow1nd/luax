#include <stdio.h>
#include <string.h>

#include "mem.h"

char * test_data[] = {
    "1etstteunahuahsu",
    "2eunhuuhuhuuhnue",
    "375597u9e7u97e9u79",
    "4nuenuenunehueuneurbxtrb uhenhu uhaauhuhuhuuhuuhuuhuuuuuuuuuuuuuuuuuuuuuuuuuuurheruhrruhrnkbeeueuhahnuhnuheuh",
    "uuu"
};


int main(int argc, char *argv[])
{
    UNUSED_ARGUMENT(argc);
    UNUSED_ARGUMENT(argv);
#ifndef __LX_MEM__H_
    printf("__LX_MEM__H_ not defined\n");
    return -4;
#endif

    printf("This is test for stack_allocator\n");
    lx_stack_allocator* allocator = lx_create_stack_allocator(25);

    char * p[5];
    for(int i = 0; i < 5; ++i){
        p[i] = lx_stack_allocator_alloc(allocator, strlen(test_data[i]) + 1);
        strcpy(p[i], test_data[i]);
    }

    char *a, *b, *c;
    a = lx_stack_allocator_alloc(allocator, 5);
    b = lx_stack_allocator_alloc(allocator, 759759);
    c = lx_stack_allocator_alloc(allocator, 15);
    lx_stack_allocator_free(allocator, c);
    lx_stack_allocator_free(allocator, b);
    b = lx_stack_allocator_alloc(allocator, 42);
    lx_stack_allocator_free(allocator, b);
    lx_stack_allocator_free(allocator, a);

    for(int i = 4; i >= 0; --i){
        if(strcmp(p[i], test_data[i]) != 0){
            // not same: fail
            return -1;
        }
        lx_stack_allocator_free(allocator, p[i]);
    }

    // if(allocator->block->curr != 0){
    //     return -2;
    // }
    // if(allocator->block->next != NULL || allocator->block->prev != NULL){
    //     return -3;
    // }

    lx_delete_stack_allocator(allocator);

    return 0;
}