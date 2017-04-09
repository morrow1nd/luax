#include "./lio.h"
#include "luax_api.h"


void lio__print(lx_vm_stack* s, lx_object* called_obj)
{
    char tem[1024 * 4];
    lx_object* obj = NULL;
    while ((obj = lx_pop(s)) != NULL && obj->type != LX_OBJECT_TAG) {
        printf("%s", lx_object_to_string(obj, tem));
    }
    lx_pop(s); // pop the func_obj
}


lx_object_table* lx_lio_load()
{
    lx_object_table* io = lx_create_object_table();
    // lx_object_table_replace_s(io, "print", -1, lx_create_object_function_p(&lio__print));

    return io;
}