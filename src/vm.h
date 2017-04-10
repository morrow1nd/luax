#ifndef __VM__H_
#define __VM__H_

#include "base.h"
#include "hash/src/uthash.h"


enum LX_OBJECT_TYPE {
    LX_OBJECT_NIL = 48,  // ascii code '0', 0x30
    LX_OBJECT_BOOL,
    LX_OBJECT_NUMBER,
    LX_OBJECT_STRING,
    LX_OBJECT_FUNCTION,
    LX_OBJECT_TABLE,

    LX_OBJECT_TAG,

    //LX_OBJECT_NIL = '_', // todo
    //LX_OBJECT_BOOL = 'B',
    //LX_OBJECT_NUMBER = 'N',
    //LX_OBJECT_STRING = 'S',
    //LX_OBJECT_FUNCTION = 'F',
    //LX_OBJECT_TABLE = 'T',

    //LX_OBJECT_TAG = '#',
};


typedef struct lx_object {
    short type;
    //char float_or_int; // no use now
    //char read_only; // 0 or 1 (useful in table)

    //union {
    //    float fnumber;
    //    int inumber;
    //};
    float fnumber;
} lx_object;
lx_object* lx_create_object(short type);
void lx_delete_object(lx_object* obj);
bool lx_object_is_jz_zero(lx_object* obj);


// lx_object is used to store constant. we also need a base class to function as a managed-object. 
// Our Garbage collection store info in this class.
//typedef struct lx_managed_object {
//} lx_managed_object;


typedef struct lx_opcodes lx_opcodes;
typedef struct lx_object_table lx_object_table;
typedef struct lx_vm_stack lx_vm_stack;
// called_obj:
//      `tab(1, 2, 3)`                                 called_obj is a table
//      `tab.func = function() end; tab.func(1)`       called_obj is not a table
//typedef void (*lx_object_function_ptr_handle)(lx_vm_stack* stack, lx_object* called_obj, lx_object_table* _env);
typedef void(*lx_object_function_ptr_handle)(lx_vm_stack* stack, lx_object* called_obj);
typedef struct _vm_env _vm_env;

// luax function(achieved in C or luax)
typedef struct lx_object_function {
    lx_object base; /* simulate class inheritance */

    lx_object_table* env_creator; // the _env table when this function was created
    lx_object_function_ptr_handle func_ptr; // achieved in C
    lx_opcodes* func_opcodes; // achieved in luax code
} lx_object_function;
lx_object_function* lx_create_object_function(lx_object_table *env_creator);
lx_object_function* lx_create_object_function_p(lx_object_function_ptr_handle func_ptr, lx_object_table *env_creator);
lx_object_function* lx_create_object_function_ops(lx_opcodes* func_opcodes, lx_object_table *env_creator);
void lx_delete_object_function(lx_object_function* obj_func);


// [inner]: table's key and value pair
typedef struct _object_table_kv {
    char * key;
    lx_object* value;

    UT_hash_handle hh; /* hash lib: makes this structure hashable */
} _object_table_kv;

// luax table
typedef struct lx_object_table {
    lx_object base;
    
    _object_table_kv* keyvalue_map; // entry of this hash table
} lx_object_table;
lx_object_table* lx_create_object_table_raw(); // create a table with no meta table
lx_object_table* lx_create_object_table(); // create a normal table, such as this table `tab = {}`
lx_object_table* lx_create_object_table_with_meta_table(lx_object_table* meta_table);
lx_object_table* lx_create_object_env_table(); // create empty environmet table with no inside functions
lx_object_table* lx_create_object_env_table_with_father_env(lx_object_table* _father_env);
lx_object_table* lx_create_env_table_with_inside_function(); /* environment table, only include inside function, no standard library */
void lx_delete_object_table(lx_object_table* tab); // todo: basicily, we shouldn't call this function, it's GC's job

lx_object_table* lx_create_default_meta_table();
lx_object_table* lx_create_default_env_meta_table();

// table key-value get and set
_object_table_kv* lx_object_table_find(lx_object_table* tab, lx_object* k); /* return NULL when didn't find */
_object_table_kv* lx_object_table_find_s(lx_object_table* tab, const char* text, int text_len);
_object_table_kv* lx_object_table_always_found(lx_object_table* tab, lx_object* k); /* return a new key_to_nil when didn't find */
lx_object* lx_object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v);
lx_object* lx_object_table_replace_s(lx_object_table* tab, const char* text, int text_len, lx_object* v);

// wrapper function for meta table
lx_object_table* lx_object_table_get_meta_table(lx_object_table* tab);
void lx_object_table_set_meta_table(lx_object_table* tab, lx_object_table* new_meta_table);

lx_object* lx_meta_element_get(lx_object_table* tab, const char* str);
void lx_meta_element_set(lx_object_table* tab, const char* str, lx_object* _element);
lx_object_function* lx_meta_function_get(lx_object_table* tab, const char* str);
void lx_meta_function_set(lx_object_table* tab, const char* str, lx_object_function* _functor);



// luax string(string length stored in text_len)
typedef struct lx_object_string {
    lx_object base;

    char * text;
    int text_len;
} lx_object_string;
lx_object_string* lx_create_object_string_s(char * text, int text_len);
lx_object_string* lx_create_object_string(const char * str); // string ended by '\0'


// global constant luax object
lx_object* LX_OBJECT_nil();
lx_object* LX_OBJECT_true();
lx_object* LX_OBJECT_false();
lx_object* LX_OBJECT_tag();


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
lx_object* lx_vm_stack_push(lx_vm_stack* stack, lx_object* obj); /* return the pushed obj */
lx_object* lx_vm_stack_pop(lx_vm_stack* stack);
lx_object* lx_vm_stack_remove(lx_vm_stack* stack, int index);



typedef struct lx_vm {
    lx_vm_stack* stack; // runtime stack
} lx_vm;


lx_vm* lx_create_vm();
int lx_vm_run(lx_vm* vm, lx_object_function* func_obj);
void lx_delete_vm(lx_vm* vm);


// helper function
const char* lx_object_to_string(lx_object* obj, char str[]);
// dump all of this obj. For a table, it would dump it's every key-value.
void lx_dump_object(lx_object* obj, FILE* fp);

// debug helper function
void lx_object_inner_to_string(lx_object* obj, char str[]); /* for debug show */
void lx_dump_vm_stack(lx_vm_stack* stack);
void lx_dump_vm_status(lx_vm* vm);

#endif