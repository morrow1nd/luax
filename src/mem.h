#ifndef __LX_MEM__H_
#define __LX_MEM__H_

#include "./base.h"

void * lx_malloc(size_t len);
void lx_free(void * ptr);


struct lx_stack_allocator_block;

typedef struct {
    // removing happens in calling lx_stack_allocator_free()
    bool auto_remove_unused_block;

    struct lx_stack_allocator_block *block;
} lx_stack_allocator;

lx_stack_allocator* lx_stack_allocator_create(size_t expected_capacity);
void lx_stack_allocator_delete(lx_stack_allocator *sallocator);

void* lx_stack_allocator_alloc(lx_stack_allocator *sallocator, size_t size);
void lx_stack_allocator_free(lx_stack_allocator *sallocator, void *ptr);


#define LX_NEW(__struct_t) ((__struct_t *)lx_malloc(sizeof(__struct_t)))


#endif // end of __LX_MEM__H_