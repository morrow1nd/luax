#ifndef __VM__H_
#define __VM__H_

#include "base.h"


typedef enum LX_OBJECT_TYPE {
    LX_OBJECT_TABLE_KEY,
    LX_OBJECT_TABLE,
    LX_OBJECT_FUNCTION,
    LX_OBJECT_NUMBER,
    LX_OBJECT_STRING,
    LX_OBJECT_BOOL,
    LX_OBJECT_NIL,
} lx_object_type;

typedef struct lx_opcodes lx_opcodes;

typedef struct lx_object {
    lx_object_type type;
    union {
        float fnumber;
        int inumber;
        bool bool_value;
        lx_opcodes* func_opcodes;
    };
} lx_object;

//
// luax vm stack
//
typedef struct lx_vm_stack {
    int capacity_grain; /* default to 32 */
    int size;

    int curr; /* set to -1 after init */
    lx_object** arr;
} lx_vm_stack;
lx_vm_stack* lx_vm_stack_create(int capacity_grain);
void lx_vm_stack_delete(lx_vm_stack* stack);
/* return the pushed obj */
lx_object* lx_vm_stack_push(lx_vm_stack* stack, lx_object* obj);
lx_object* lx_vm_stack_pop(lx_vm_stack* stack);


typedef struct lx_vm {
    lx_object* obj;
} lx_vm;


lx_vm* lx_vm_create();
int lx_vm_run(lx_vm* vm, lx_object* func_obj);
void lx_vm_delete(lx_vm* vm);


#endif