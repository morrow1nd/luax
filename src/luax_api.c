#include "./luax_api.h"
#include "./base.h"


void lx_push_tag(lx_vm_stack* stack)
{
    lx_vm_stack_push(stack, &LX_OBJECT_tag);
}

lx_object* lx_pop(lx_vm_stack* stack)
{
    return lx_vm_stack_pop(stack);
}

//void lx_function_return(lx_vm_stack* s)
//{
//    int tagi = s->curr;
//    while (tagi >= 0 && s->arr[tagi]->type != LX_OBJECT_TAG) {
//        tagi--;
//    }
//    if (tagi < 0) {
//        //todo
//        printf("error: lx_function_return: stack is broken");
//        assert(false);
//    }
//    if (tagi == 0) {
//        //todo
//        printf("error: lx_function_return: can't get func_obj");
//        assert(false);
//    }
//    tagi--; // now tagi points to the func_obj
//    memcpy((char*)(s->arr) + sizeof(lx_object*) * tagi, (char*)(s->arr) + sizeof(lx_object*) * (tagi + 1), sizeof(lx_object*) * (s->curr - tagi));
//    s->curr--;
//}