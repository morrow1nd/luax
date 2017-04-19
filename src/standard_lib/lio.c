#include "./lio.h"
#include "luax_api.h"


void lio__print(lx_object_stack* s, lx_object* called_obj)
{
}


lx_object_table* lx_lio_load()
{
    lx_object_table* io = lx_create_object_table(NULL);
    // lx_object_table_replace_s(io, "print", -1, lx_create_object_function_p(&lio__print));

    return io;
}