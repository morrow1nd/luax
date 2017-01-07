#include "./mem.h"

void * lx_malloc(size_t len)
{
#if(LX_PRINT_MALLOC_INFO)
    static int call_lx_malloc_number = 0;
    ++call_lx_malloc_number;
    char * ret = malloc(len);
    printf("lx_malloc:%d %p size:%d\n", call_lx_malloc_number, ret, len);
    return ret;
#endif
    return malloc(len);
}
void lx_free(void * ptr)
{
#if(LX_PRINT_MALLOC_INFO)
    static int call_lx_free_number = 0;
    ++call_lx_free_number;
    printf("lx_free:%d %p\n", call_lx_free_number, ptr);
#endif
    free(ptr);
}


typedef struct lx_stack_allocator_block {
    char *buff;
    size_t block_size;

    size_t backup_curr;
    struct lx_stack_allocator_block *prev;
} lx_stack_allocator_block;
static lx_stack_allocator_block* __new_block(size_t block_size)
{
    lx_stack_allocator_block* block = LX_NEW(lx_stack_allocator_block);
    block->block_size = block_size;
    block->buff = lx_malloc(block_size);
    block->prev = NULL;
    block->backup_curr = 0;
    return block;
}

lx_stack_allocator* lx_stack_allocator_create(size_t expected_capacity)
{
    lx_stack_allocator *salloc = LX_NEW(lx_stack_allocator);
    salloc->block = __new_block(expected_capacity);
    salloc->curr = 0;
    return salloc;
}
void lx_stack_allocator_delete(lx_stack_allocator *sallocator)
{
    lx_stack_allocator_block* prev;
    for (lx_stack_allocator_block* b = sallocator->block; b != NULL; ) {
        prev = b->prev;
        lx_free(b->buff);
        lx_free(b);
        b = prev;
    }
    lx_free(sallocator);
}

const int LX_STACK_ALLOCATOR_PREFIX_SIZE = 8;

void * lx_stack_allocator_alloc(lx_stack_allocator *sallocator, size_t size)
{
    // check whether there is enough area
    if ((size / 8) * 8 != size) {
        size = (size / 8 + 1) * 8;
    }
    if (sallocator->curr + size > sallocator->block->block_size) {
        // we need to add a new block
        size_t newblock_size = sallocator->block->block_size;
        while (newblock_size - LX_STACK_ALLOCATOR_PREFIX_SIZE < size) {
            newblock_size *= 2;
        }
        lx_stack_allocator_block *newblock = __new_block(newblock_size);
        newblock->prev = sallocator->block;
        sallocator->block->backup_curr = sallocator->curr;
        sallocator->block = newblock;
        sallocator->curr = 0;
    }
    *(size_t *)(sallocator->block->buff + sallocator->curr) = size;
    sallocator->curr += size + LX_STACK_ALLOCATOR_PREFIX_SIZE;
    return sallocator->block->buff + sallocator->curr - size;
}
void lx_stack_allocator_free(lx_stack_allocator *sallocator, void *ptr)
{
    if (sallocator->curr == 0) {
        // we need to free this block first
        lx_stack_allocator_block* prev = sallocator->block->prev;
        lx_free(sallocator->block->buff);
        lx_free(sallocator->block);
        sallocator->block = prev;
        sallocator->curr = sallocator->block->backup_curr;
    }
    size_t size = *(size_t*)((char*)ptr - LX_STACK_ALLOCATOR_PREFIX_SIZE);
    if (sallocator->curr - size - LX_STACK_ALLOCATOR_PREFIX_SIZE < 0) {
        assert(false && "stack_allocator_free error here!");
        return;
    }
    sallocator->curr -= size + LX_STACK_ALLOCATOR_PREFIX_SIZE;
}