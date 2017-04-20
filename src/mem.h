#ifndef __LX_MEM__H_
#define __LX_MEM__H_

#include "./base.h"

#if LX_PRINT_MALLOC_INFO
extern int lx_call_lx_malloc_number;
extern int lx_call_lx_free_number;
#endif
#if LX_MALLOC_STATISTICS
extern int lx_memory_usage; /* byte */
extern int lx_memory_max_usage;
extern int lx_memory_malloc_times;
extern int lx_memory_free_times;
void lx_dump_memory_usage();
#endif


void * lx_malloc(size_t len);
void lx_free(void * ptr);


struct lx_stack_allocator_block;

typedef struct {
    // removing happens in calling lx_stack_allocator_free()
    bool auto_remove_unused_block;

    struct lx_stack_allocator_block *block;
} lx_stack_allocator;

lx_stack_allocator* lx_create_stack_allocator(size_t expected_capacity);
void lx_delete_stack_allocator(lx_stack_allocator *sallocator);

void* lx_stack_allocator_alloc(lx_stack_allocator *sallocator, size_t size);
void lx_stack_allocator_free(lx_stack_allocator *sallocator, void *ptr);


#define LX_NEW(__struct_t) ((__struct_t *)lx_malloc(sizeof(__struct_t)))


#endif // end of __LX_MEM__H_