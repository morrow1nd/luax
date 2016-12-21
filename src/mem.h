#ifndef __LX_MEM__H_
#define __LX_MEM__H_

#include "./base.h"

void * lx_malloc(size_t len)
{
    return malloc(len);
}
void lx_free(void * ptr)
{
    free(ptr);
}


#define LX_NEW(__struct_t) ((__struct_t *)lx_malloc(sizeof(__struct_t)))


#endif // end of __LX_MEM__H_