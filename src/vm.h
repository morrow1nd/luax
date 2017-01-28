#ifndef __VM__H_
#define __VM__H_

#include "base.h"
#include "hash/src/uthash.h"


enum LX_OBJECT_TYPE {
    LX_OBJECT_NIL = 48,  // ascii code '0'
    LX_OBJECT_BOOL,
    LX_OBJECT_NUMBER,
    LX_OBJECT_STRING,
    LX_OBJECT_FUNCTION,
    LX_OBJECT_TABLE,

    LX_OBJECT_TAG,
    LX_OBJECT_TABLE_KEY,
};


typedef struct lx_object {
    int type;
    union {
        float fnumber;
        int inumber;
    };
} lx_object;
bool lx_object_is_jz_zero(lx_object* obj);

typedef struct lx_opcodes lx_opcodes;
typedef struct lx_object_table lx_object_table;
typedef struct lx_vm_stack lx_vm_stack;
typedef void (*lx_object_function_ptr_handle)(lx_vm_stack* stack, lx_object* called_obj);
typedef struct lx_object_function {
    lx_object base;

    lx_object_table* _E;
    lx_object_table* _G;
    /* function running environment, used to store variable scope info */
    lx_vm_env* backup_env_stack;

    lx_object_function_ptr_handle func_ptr;
    lx_opcodes* func_opcodes;
} lx_object_function;
lx_object_function* lx_create_object_function();
lx_object_function* lx_create_object_function_p(lx_object_function_ptr_handle func_ptr);
lx_object_function* lx_create_object_function_ops(lx_opcodes* func_opcodes);
void lx_delete_object_function(lx_object_function* obj_func);

typedef struct lx_object_table_key {
    lx_object base;

    char * key;
    lx_object* value;

    UT_hash_handle hh; /* makes this structure hashable */
} lx_object_table_key;
typedef struct lx_object_table {
    lx_object base;
    
    lx_object_table_key* keyvalue_map; // entry of this hash table
} lx_object_table;
lx_object_table* lx_create_object_table();
void lx_delete_object_table(lx_object_table* tab);
lx_object_table_key* lx_object_table_find(lx_object_table* tab, lx_object* k); /* return NULL when didn't find */
lx_object_table_key* lx_object_table_always_found(lx_object_table* tab, lx_object* k); /* return a new key_to_nil when didn't find */
//lx_object_table_key* lx_object_table_find_s(lx_object_table* tab, char* k);
lx_object* lx_object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v);
lx_object* lx_object_table_replace_s(lx_object_table* tab, const char* text, int text_len, lx_object* v);

typedef struct lx_object_string {
    lx_object base;

    char * text;
    int text_len;
} lx_object_string;


extern const lx_object LX_OBJECT_nil;
extern const lx_object LX_OBJECT_true;
extern const lx_object LX_OBJECT_false;
extern const lx_object LX_OBJECT_tag;
extern const lx_object_table_key LX_OBJECT_TABLE_key_nil_to_nil;

//
// luax vm stack
//
typedef struct lx_vm_stack {
    int capacity_enlarge_grain; /* default to 32 */
    int capacity;

    int curr; /* set to -1 after init */
    lx_object** arr;
} lx_vm_stack;
lx_vm_stack* lx_create_vm_stack(int capacity_enlarge_grain);
void lx_delete_vm_stack(lx_vm_stack* stack);
/* return the pushed obj */
lx_object* lx_vm_stack_push(lx_vm_stack* stack, lx_object* obj);
lx_object* lx_vm_stack_pop(lx_vm_stack* stack);

/* used in `push_env` and `pop_env` */
typedef struct lx_vm_env {
    lx_object_table* _E;
    lx_object_table* _G;

    struct lx_vm_env* prev;
} lx_vm_env;

typedef struct lx_vm {
    /* runtime stack */
    lx_vm_stack* stack;
} lx_vm;


lx_vm* lx_create_vm();
int lx_vm_run(lx_vm* vm, lx_object* func_obj);
void lx_delete_vm(lx_vm* vm);


// helper function
/* environment table used as func_obj's _E, only include base function, no full standard library */
lx_object_table* lx_create_base_env_table();
const char* lx_object_to_string(lx_object* obj, char str[]);
// debug helper function
const char* lx_object_inner_to_string(lx_object* obj, char str[]); /* for debug show */
void lx_dump_vm_stack(lx_vm* vm);
void lx_dump_vm_status(lx_vm* vm);

#endif