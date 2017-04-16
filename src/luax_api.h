#ifndef __LUAX_API__H_
#define __LUAX_API__H_

/*
 standard lib achiever only call function declared in this file.

*/

#include "./base.h"
#include "vm.h"


void lx_push_number(lx_vm* vm, ...);
void lx_push_string();
void lx_push_bool();
void lx_push_nil(lx_object_stack* stack);
void lx_push_table();

void lx_push_tag(lx_object_stack* stack);


lx_object* lx_pop(lx_object_stack* stack);

/*
 * i: 0  -> the top of the stack
 *    -1 -> ...
 *    -N -> ...
 */
lx_object* lx_stack(lx_vm* vm, int i);

/* clean the func_obj in the stack */
// void lx_function_return(lx_object_stack* stack);


//
// Standard Library: Basic Functions
//

lx_object_table* lx_require();


#endif // !__LUAX_API__H_