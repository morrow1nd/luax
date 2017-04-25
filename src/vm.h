#ifndef __VM__H_
#define __VM__H_

#include "base.h"
#include "object.h"

#include <setjmp.h> /* setjmp, longjmp */


/* Garbage Collection */
typedef struct lx_gc_info {
    lx_object_stack* arr; /* used to store the pointers of all managed objects */

    lx_object_stack* always_in_mem; /* a continer used to store these objects which will never be collected by GC */
} lx_gc_info;

lx_gc_info* lx_create_gc_info();
void lx_delete_gc_info(lx_gc_info* gc);
lx_object* managed_with_gc(lx_gc_info* gc, lx_object* obj); /* manage the lifecycle of this object in GC */
void lx_gc_collect(lx_vm* vm); /* collect garbage */


typedef struct lx_vm {
    lx_object_stack* stack; /* runtime stack */
    lx_object_stack* call_stack; /* environment changing stack */
    jmp_buf* curr_jmp_buf; /* needed by longjmp */
    lx_gc_info* gc;
} lx_vm;


lx_vm* lx_create_vm();
/*
** run luax code gived by a luax string `str` in VM
**   str: managed luax string
**   env: the environment table. After finish, you can get useful info from this table
**   return: a managed object. Returning a nil means success
*/
lx_object* lx_dostring(lx_vm* vm, lx_object_string* str, lx_object_table* env);
void lx_delete_vm(lx_vm* vm);
void lx_throw_s(lx_vm* vm, const char* str); /* throw a string object as a exception */
void lx_throw(lx_vm* vm, lx_object* e); /* throw a exception */


/* managed object operations */
lx_object_table* lx_create_table(lx_vm* vm);
lx_object_table* lx_create_table_with_meta_table(lx_vm* vm, lx_object_table* meta_table);
lx_object* lx_table_replace_s(lx_vm* vm, lx_object_table* tab, const char* str, lx_object* value);
lx_object_table* lx_create_env_table(lx_vm* vm);
lx_object_table* lx_create_env_table_with_father_env(lx_vm* vm, lx_object_table* _father_env);
lx_object_table* lx_create_env_table_with_inside_function(lx_vm* vm);
lx_object_string* lx_create_string_t(lx_vm* vm, const char* text, int text_len);
lx_object_string* lx_create_string_s(lx_vm* vm, const char* str);
lx_object_string* lx_create_string_t_copy(lx_vm* vm, const char* text, int text_len);
lx_object_string* lx_create_string_s_copy(lx_vm* vm, const char* str);
lx_object* lx_create_number(lx_vm* vm, float number);
lx_object_function* lx_create_function_p(lx_vm* vm, lx_object_function_ptr_handle func_ptr, lx_object_table *env_creator);
lx_object_function* lx_create_function_ops_copy(lx_vm* vm, const lx_opcode** func_opcodes, int func_opcodes_size, lx_object_table *env_creator);

lx_object_table* lx_create_default_meta_table(lx_vm* vm);
lx_object_table* lx_create_default_env_meta_table(lx_vm* vm);


/* debug helper function */
void lx_dump_object_stack(lx_object_stack* stack);
void lx_dump_vm_status(lx_vm* vm);
void lx_dump_vm_gc_status(lx_vm* vm);


#endif