#ifndef __LUAX_API__H_
#define __LUAX_API__H_

#include "object.h"


//typedef struct lx_object lx_object;
//typedef struct lx_vm lx_vm;
//typedef struct lx_object_table lx_object_table;



lx_object* lx_push(lx_vm* vm, lx_object* obj);
lx_object* lx_pop(lx_vm* vm);
lx_object* lx_stack_at(lx_vm* vm, int i);

lx_object_table* lx_create_table(); // return managed obj
lx_object_table* lx_create_table_with_meta_table(lx_object_table* meta_table);
lx_object_string* lx_create_string(); // todo
lx_object* lx_create_number(float number);





#endif // !__LUAX_API__H_