#include "./lmath.h"
#include "luax.h"
#include "luax_api.h"

#include <math.h>


void lmath__abs(lx_vm_stack* s, lx_object* call_obj)
{
    //lx_object* obj = NULL;
    //int tagi = s->curr;
    //while ((obj = s->arr[tagi])->type != LX_OBJECT_TAG) {
    //    if(obj->type == LX_OBJECT_TABLE_KEY)
    //        obj = ((lx_object_table_key*)obj)->value;
    //    if (obj->type != LX_OBJECT_NUMBER) {
    //        //todo
    //        printf("error: math.abs can only accept number type");
    //        assert(false);
    //    }
    //    if(obj->fnumber < 0.0f)
    //        obj->fnumber = - obj->fnumber;
    //    --tagi;
    //}
    ////... todo
}

lx_object_table* lx_lmath_load()
{
    lx_object_table* io = lx_create_object_table();
    //lx_object_table_replace_s(io, "print", -1, lx_create_object_function_p(&lmath__abs));

    return io;
}