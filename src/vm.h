#ifndef __VM__H_
#define __VM__H_

#include "base.h"
#include "object.h"

#include <setjmp.h> // setjmp, longjmp


typedef struct lx_vm {
    lx_object_stack* stack; // runtime stack
    jmp_buf* curr_jmp_buf;
    lx_gc_info* gc;
    lx_object_stack* call_stack;
} lx_vm;


lx_vm* lx_create_vm();
int lx_vm_run(lx_vm* vm, lx_object_function* func_obj, lx_object** exception);
void lx_delete_vm(lx_vm* vm);
void lx_throw_s(lx_vm* vm, const char* str);


// debug helper function
void lx_dump_object_stack(lx_object_stack* stack);
void lx_dump_vm_status(lx_vm* vm);
void lx_dump_vm_gc_status(lx_vm* vm);

#endif