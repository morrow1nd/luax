#include "./mem.h"

#if LX_PRINT_MALLOC_INFO
int lx_call_lx_malloc_number = 0;
int lx_call_lx_free_number = 0;
#endif
#if LX_MALLOC_STATISTICS
int lx_memory_usage = 0; /* byte */
int lx_memory_max_usage = 0; /* byte */
int lx_memory_malloc_times = 0;
int lx_memory_free_times = 0;
void lx_dump_memory_usage()
{
    printf("=== dump memory usage:\n");
    printf("\tmemory usage: %d bytes\n", lx_memory_usage);
    printf("\tmemory max usage: %d bytes\n", lx_memory_max_usage);
    int sub = lx_memory_malloc_times - lx_memory_free_times;
    if(sub >= 0)
        printf("\tmalloc times:%d, free times:%d. (+%d)\n", lx_memory_malloc_times, lx_memory_free_times, sub);
    else
        printf("\tmalloc times:%d, free times:%d. (%d)\n", lx_memory_malloc_times, lx_memory_free_times, sub);
}
#endif


void * lx_malloc(size_t len)
{
    int * ret = NULL;
#if LX_MALLOC_STATISTICS
    ++lx_memory_malloc_times;
    lx_memory_usage += len;
    if(lx_memory_usage > lx_memory_max_usage)
        lx_memory_max_usage = lx_memory_usage;
    ret = (int*)malloc(len + sizeof(int));
    *ret = len;
    ret = (int*)((char*)ret + sizeof(int));
#else
    ret = (int*)malloc(len);
#endif
#if(LX_PRINT_MALLOC_INFO)
    ++lx_call_lx_malloc_number;
    printf("lx_malloc:%d %p size:%d\n", lx_call_lx_malloc_number, ret, (int)len);
#endif
    return ret;
}
void lx_free(void * ptr)
{
#if(LX_PRINT_MALLOC_INFO)
    ++lx_call_lx_free_number;
    printf("lx_free:%d %p\n", lx_call_lx_free_number, ptr);
#endif
#if LX_MALLOC_STATISTICS
    ++lx_memory_free_times;
    ptr = (char*)ptr - sizeof(int);
    lx_memory_usage -= *((int*)ptr);
#endif
    free(ptr);
}


typedef struct lx_stack_allocator_block {
    size_t block_size;

    size_t curr;
    struct lx_stack_allocator_block *prev;
    struct lx_stack_allocator_block *next;
} lx_stack_allocator_block;
static lx_stack_allocator_block* __new_block(size_t block_size)
{
    char * buf = lx_malloc(sizeof(lx_stack_allocator_block) + block_size);
    lx_stack_allocator_block * b = (lx_stack_allocator_block *)buf;
    b->block_size = block_size;
    b->prev = NULL;
    b->next = NULL;
    b->curr = 0;

    return b;
}
static void __delete_block(lx_stack_allocator_block* ptr)
{
    lx_free(ptr);
}


lx_stack_allocator* lx_create_stack_allocator(size_t expected_capacity)
{
    lx_stack_allocator *salloc = LX_NEW(lx_stack_allocator);
    salloc->block = __new_block(expected_capacity);
    salloc->auto_remove_unused_block = false;
    return salloc;
}
void lx_delete_stack_allocator(lx_stack_allocator *sallocator)
{
    lx_stack_allocator_block* curr = sallocator->block;
    for (lx_stack_allocator_block* next = curr->prev; next != NULL; ) {
        curr = next->prev;
        lx_free(next);
        next = curr;
    }
    curr = sallocator->block;
    for (lx_stack_allocator_block* next = curr->next; next != NULL; ) {
        curr = next->next;
        lx_free(next);
        next = curr;
    }
    lx_free(sallocator->block);
    lx_free(sallocator);
}

const int LX_STACK_ALLOCATOR_PREFIX_SIZE = 8;

void * lx_stack_allocator_alloc(lx_stack_allocator *sallocator, size_t size)
{
    // enarge this area
    if ((size / 8) * 8 != size) {
        size = (size / 8 + 1) * 8;
    }
    // check whether there is enough area
    if (sallocator->block->curr + size > sallocator->block->block_size) {
        // we need to add a new block
        if (sallocator->block->next != NULL && sallocator->block->next->block_size >= size) {
            ;
        } else {
            size_t newblock_size = sallocator->block->block_size;
            while (newblock_size - LX_STACK_ALLOCATOR_PREFIX_SIZE < size) {
                newblock_size *= 2;
            }
            lx_stack_allocator_block *newblock = __new_block(newblock_size);
            newblock->prev = sallocator->block;
            if (sallocator->block->next) {
                newblock->next = sallocator->block->next;
                sallocator->block->next->prev = newblock;
            }
            sallocator->block->next = newblock;
        }
        sallocator->block = sallocator->block->next;
    }
    *(size_t *)((char*)(sallocator->block) + sizeof(lx_stack_allocator_block) + sallocator->block->curr) = size;
    sallocator->block->curr += size + LX_STACK_ALLOCATOR_PREFIX_SIZE;
    return (char*)(sallocator->block) + sizeof(lx_stack_allocator_block) + sallocator->block->curr - size;
}
void lx_stack_allocator_free(lx_stack_allocator *sallocator, void *ptr)
{
    if (sallocator->block->curr == 0) {
        if (sallocator->auto_remove_unused_block == true) {
            // we need to free all these blocks after curr block
            lx_stack_allocator_block* i = NULL;
            for (lx_stack_allocator_block* next = sallocator->block->next; next != NULL; ) {
                i = next->next;
                __delete_block(next);
                next = i;
            }
            i = sallocator->block->prev;
            __delete_block(sallocator->block);
            sallocator->block = i;
            sallocator->block->next = NULL;
        } else {
            sallocator->block = sallocator->block->prev;
        }
    }
    size_t size = *(size_t*)((char*)ptr - LX_STACK_ALLOCATOR_PREFIX_SIZE);
    if (sallocator->block->curr - size - LX_STACK_ALLOCATOR_PREFIX_SIZE < 0) {
        assert(false && "stack_allocator_free error here!");
        return;
    }
    sallocator->block->curr -= size + LX_STACK_ALLOCATOR_PREFIX_SIZE;
}