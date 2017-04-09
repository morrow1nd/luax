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
    LX_OBJECT_TABLE_KV,
};


typedef struct lx_object {
    int type;
    union {
        float fnumber;
        int inumber;
    };
} lx_object;
bool lx_object_is_jz_zero(lx_object* obj);


// lx_object is used to store constant. we also need a base class to function as a managed-object. 
// Our Garbage collection store info in this class.
//typedef struct lx_managed_object {
//} lx_managed_object;


typedef struct lx_opcodes lx_opcodes;
typedef struct lx_object_table lx_object_table;
typedef struct lx_vm_stack lx_vm_stack;
// called_obj:
//   `tab(1, 2, 3)`                                 called_obj is a table
//   `tab.func = function() end; tab.func(1)`       called_obj is not a table
typedef void (*lx_object_function_ptr_handle)(lx_vm_stack* stack, lx_object* called_obj, lx_object_table* _env);
typedef struct _vm_env _vm_env;

// luax function(achieved in C or luax)
typedef struct lx_object_function {
    lx_object base;

    lx_object_table* env_creator; // the _env table when this function was created
    _vm_env* backup_env_stack; // function running environment, used to store variable scope info
    union {
        lx_object_function_ptr_handle func_ptr; // achieved in C
        lx_opcodes* func_opcodes; // achieved in luax code
    };
} lx_object_function;
lx_object_function* lx_create_object_function(lx_object_table *env_creator);
lx_object_function* lx_create_object_function_p(lx_object_function_ptr_handle func_ptr, lx_object_table *env_creator);
lx_object_function* lx_create_object_function_ops(lx_opcodes* func_opcodes, lx_object_table *env_creator);
void lx_delete_object_function(lx_object_function* obj_func);


// luax table's key and value pair
typedef struct lx_object_table_kv {
    lx_object base;

    char * key;
    lx_object* value;

    UT_hash_handle hh; /* makes this structure hashable */
} lx_object_table_kv;

// luax table
typedef struct lx_object_table {
    lx_object base;
    
    lx_object_table_kv* keyvalue_map; // entry of this hash table
} lx_object_table;
lx_object_table* lx_create_object_table_raw(); // create a table with no meta table
lx_object_table* lx_create_object_table(); // create a normal table, such as this table `tab = {}`
lx_object_table* lx_create_object_table_with_meta_table(lx_object_table* meta_table);
lx_object_table* lx_create_object_env_table(); // create empty environmet table with no inside functions
lx_object_table* lx_create_object_env_table_with_father_env(lx_object_table* _father_env);
lx_object_table* lx_create_base_env_table(); /* environment table, only include inside function, no standard library */
void lx_delete_object_table(lx_object_table* tab); // todo: basicily, we shouldn't call this function, it's GC's job

lx_object_table* lx_object_table_get_meta_table(lx_object_table* tab);
void lx_object_table_set_meta_table(lx_object_table* tab, lx_object_table* new_meta_table);

// wrapper function to access table's meta function
lx_object* lx_meta_element_get(lx_object_table* tab, const char* str);
void lx_meta_element_set(lx_object_table* tab, const char* str, lx_object* _element);
lx_object_function* lx_meta_function_get(lx_object_table* tab, const char* str);
void lx_meta_function_set(lx_object_table* tab, const char* str, lx_object_function* _functor);
//lx_object_function* lx_meta_function_get__get(lx_object_table* tab);
//lx_object_function* lx_meta_function_get__set(lx_object_table* tab);
//lx_object_function* lx_meta_function_get__call(lx_object_table* tab);
//lx_object_function* lx_meta_function_get__delete(lx_object_table* tab);
//void lx_meta_function_set__get(lx_object_table* tab, lx_object_function* _get);
//void lx_meta_function_set__set(lx_object_table* tab, lx_object_function* _set);
//void lx_meta_function_set__call(lx_object_table* tab, lx_object_function* _call);
//void lx_meta_function_set__delete(lx_object_table* tab, lx_object_function* _delete);

lx_object_table_kv* lx_object_table_find(lx_object_table* tab, lx_object* k); /* return NULL when didn't find */
lx_object_table_kv* lx_object_table_find_s(lx_object_table* tab, const char* text, int text_len);
lx_object_table_kv* lx_object_table_always_found(lx_object_table* tab, lx_object* k); /* return a new key_to_nil when didn't find */
lx_object* lx_object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v);
lx_object* lx_object_table_replace_s(lx_object_table* tab, const char* text, int text_len, lx_object* v);


// luax string(string length stored in text_len)
typedef struct lx_object_string {
    lx_object base;

    char * text;
    int text_len;
} lx_object_string;

lx_object* LX_OBJECT_nil();
lx_object* LX_OBJECT_true();
lx_object* LX_OBJECT_false();
lx_object* LX_OBJECT_tag();
lx_object_table* LX_OBJECT_TABLE_DEFAULT_META_TABLE();
lx_object_table* LX_OBJECT_TABLE_DEFAULT_ENV_META_TABLE();
lx_object_table* LX_OBJECT_TABLE_empty();
lx_object_table* LX_OBJECT_ENV_TABLE_empty();
lx_object_table_kv* LX_OBJECT_TABLE_KV_nil_to_nil();

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
lx_object* lx_vm_stack_pop_no_kv(lx_vm_stack* s); // if this obj is a lx_object_table_kv, return it's `value` rather than itself.
lx_object* lx_vm_stack_remove(lx_vm_stack* stack, int index);

/* used in `push_env` and `pop_env` */
typedef struct _vm_env {
    lx_object_table* _env;

    struct _vm_env* prev;
} _vm_env;


enum lx_vm_running_mode {
    LX_VM_RUNNING_MODE_NORMAL = 0,
    LX_VM_RUNNING_MODE_TABLE_SET,
};

typedef struct lx_vm {
    /* runtime stack */
    lx_vm_stack* stack;
    // record the table of every key.
    lx_vm_stack* table_set_mode_stack;
    int mode;
} lx_vm;


lx_vm* lx_create_vm();
int lx_vm_run(lx_vm* vm, lx_object_function* func_obj);
void lx_delete_vm(lx_vm* vm);


// helper function
const char* lx_object_to_string(lx_object* obj, char str[]);

// debug helper function
const char* lx_object_inner_to_string(lx_object* obj, char str[]); /* for debug show */
void lx_dump_vm_stack(lx_vm* vm);
void lx_dump_vm_status(lx_vm* vm);

#endif