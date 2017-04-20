#include "lmath.h"
#include "luax_api.h"

#include <math.h>

static lx_object_string Sabs = {
    .base.type = LX_OBJECT_STRING,
    .base.is_singleton = true,
    .need_free = false,
    .text = "abs",
    .text_len = 3
};
static lx_object_string Sint = {
    .base.type = LX_OBJECT_STRING,
    .base.is_singleton = true,
    .need_free = false,
    .text = "int",
    .text_len = 3
};

void lmath__abs(lx_vm* vm, lx_object* called_obj)
{
    UNUSED_ARGUMENT(called_obj);
    int i = vm->stack->curr;
    while (vm->stack->arr[i]->type != LX_OBJECT_TAG) {
        vm->stack->arr[i]->fnumber = fabsf(vm->stack->arr[i]->fnumber);
        i--;
    }
    lx_object_stack_remove(vm->stack, i);
}
void lmath__int(lx_vm* vm, lx_object* called_obj)
{
    UNUSED_ARGUMENT(called_obj);
    int i = vm->stack->curr;
    while (vm->stack->arr[i]->type != LX_OBJECT_TAG) {
        vm->stack->arr[i]->fnumber = (float)((int)(vm->stack->arr[i]->fnumber));
        i--;
    }
    lx_object_stack_remove(vm->stack, i);
}

lx_object_table* lx_lmath_load(lx_vm* vm)
{
    lx_object_table* tab = lx_create_env_table(vm);
    object_table_replace(tab, CAST_O &Sabs, managed_with_gc(vm->gc, CAST_O create_object_function_p(lmath__abs, lx_create_env_table(vm))));
    object_table_replace(tab, CAST_O &Sint, managed_with_gc(vm->gc, CAST_O create_object_function_p(lmath__int, lx_create_env_table(vm))));
    
    return tab;
}