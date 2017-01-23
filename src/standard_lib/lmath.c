#include "./lmath.h"
#include "luax.h"
#include "luax_api.h"

#include <math.h>


void lmath__abs(lx_vm_stack* s)
{
    lx_object* obj = lx_pop(s);
    if (obj->fnumber < 0.0f) {
        obj->fnumber = - obj->fnumber;
    }
    lx_function_return(s);
}