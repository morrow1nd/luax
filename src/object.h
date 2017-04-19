#ifndef __OBJECT__H_
#define __OBJECT__H_

#include "base.h"
#include "hash/src/uthash.h"


enum LX_OBJECT_TYPE {
    LX_OBJECT_NIL = 48,  // ascii code '0', 0x30
    LX_OBJECT_BOOL,
    LX_OBJECT_NUMBER,
    LX_OBJECT_STRING,
    LX_OBJECT_FUNCTION,
    LX_OBJECT_TABLE,

    LX_OBJECT_TAG, /* inner use */
};

const char* lx_object_type_to_string(int type);

/* type forward declaration */
typedef struct lx_opcodes lx_opcodes;
typedef struct lx_object_table lx_object_table;
typedef struct lx_vm_stack lx_vm_stack;
typedef struct lx_vm lx_vm;
typedef struct _vm_env _vm_env;
typedef struct lx_object lx_object;
typedef void(*lx_object_function_ptr_handle)(lx_vm* vm, lx_object* called_obj);
typedef struct lx_gc_info lx_gc_info;


typedef struct lx_object {
    short type;
    short marked;
    float fnumber;
} lx_object;

lx_object* lx_create_object(short type);
void lx_delete_object(lx_object* obj);
bool lx_object_is_jz_zero(lx_object* obj);


/* 
** lx_object is used to store constant. we also need a base class to function as a managed-object. 
** Our Garbage collection store info in this class.
*/
typedef struct lx_managed_object_info {
    int tem;
} lx_managed_object_info;


/* luax function(achieved in C or luax) */
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


/* [inner use]: table's key and value pair */
typedef struct _object_table_kv {
    char * hash_key;
    lx_object* key;
    lx_object* value;

    UT_hash_handle hh; /* hash lib: makes this structure hashable */
} _object_table_kv;


/* luax table */
typedef struct lx_object_table {
    lx_object base;

    _object_table_kv* keyvalue_map; // entry of this hash table
} lx_object_table;

lx_object_table* lx_create_object_table_raw(); // create a table with no meta table
void lx_delete_object_table(lx_object_table* tab); // todo: basicily, we shouldn't call this function, it's GC's job
// table key-value get and set
_object_table_kv* lx_object_table_find(lx_object_table* tab, lx_object* k); /* return NULL when didn't find */
_object_table_kv* lx_object_table_find_s(lx_object_table* tab, const char* text, int text_len);
_object_table_kv* lx_object_table_always_found(lx_object_table* tab, lx_object* k); /* return a new key_to_nil when didn't find */
lx_object* lx_object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v);
lx_object* lx_object_table_replace_s(lx_object_table* tab, const char* text, int text_len, lx_object* v, lx_gc_info* gc);

lx_object_table* lx_create_object_table(lx_gc_info* gc); // create a normal table, such as this table `tab = {}`
lx_object_table* lx_create_object_table_with_meta_table(lx_object_table* meta_table);
lx_object_table* lx_create_object_env_table(lx_gc_info* gc); // create empty environmet table with no inside functions
lx_object_table* lx_create_object_env_table_with_father_env(lx_object_table* _father_env, lx_gc_info* gc);
lx_object_table* lx_create_env_table_with_inside_function(lx_gc_info* gc); /* environment table, only include inside function, no standard library */

lx_object_table* lx_create_default_meta_table(lx_gc_info* gc);
lx_object_table* lx_create_default_env_meta_table(lx_gc_info* gc);


// wrapper function for meta table
lx_object_table* lx_object_table_get_meta_table(lx_object_table* tab);
void lx_object_table_set_meta_table(lx_object_table* tab, lx_object_table* new_meta_table);

lx_object* lx_meta_element_get(lx_object_table* tab, const char* str);
void lx_meta_element_set(lx_object_table* tab, const char* str, lx_object* _element, lx_gc_info* gc);
lx_object_function* lx_meta_function_get(lx_object_table* tab, const char* str);
void lx_meta_function_set(lx_object_table* tab, const char* str, lx_object_function* _functor, lx_gc_info* gc);


/* luax string(string length stored in text_len) */
typedef struct lx_object_string {
    lx_object base;

    const char * text;
    int text_len;
} lx_object_string;

lx_object_string* lx_create_object_string_s(const char * text, int text_len);
lx_object_string* lx_create_object_string(const char * str); // string ended by '\0'
void lx_delete_object_string(lx_object_string* obj);


/* global constant luax object */
lx_object* LX_OBJECT_nil();
lx_object* LX_OBJECT_true();
lx_object* LX_OBJECT_false();
lx_object* LX_OBJECT_tag();


/* helper function */
const char* lx_object_to_string(lx_object* obj, char str[]);
/* dump all of this obj. For a table, it would dump it's every key-value. */
void lx_dump_object(lx_object* obj, FILE* fp);
void lx_object_inner_to_string(lx_object* obj, char str[]); /* for debug show */


/* helper macro for type casting */
#define CAST_O (lx_object*)
#define CAST_S (lx_object_string*)
#define CAST_T (lx_object_table*)
#define CAST_F (lx_object_function*)


/* a stack containing lx_object pointer  */
typedef struct lx_object_stack {
    int capacity_enlarge_grain; /* default to 32 */
    int capacity;

    int curr; /* set to -1 after init */
    lx_object** arr;
} lx_object_stack;

lx_object_stack* lx_create_object_stack(int capacity_enlarge_grain);
void lx_delete_object_stack(lx_object_stack* stack);
lx_object* lx_object_stack_push(lx_object_stack* stack, lx_object* obj); /* return the pushed obj */
lx_object* lx_object_stack_pop(lx_object_stack* stack);
lx_object* lx_object_stack_remove(lx_object_stack* stack, int index);
lx_object* lx_object_stack_top(lx_object_stack* stack);


/* Garbage Collection */
typedef struct lx_gc_info {
    lx_object_stack* arr; /* used to store the pointers of all managed objects */

    lx_object_stack* runtime_stack;
    lx_object_stack* call_stack;
} lx_gc_info;

lx_gc_info* lx_create_gc_info(lx_object_stack* runtime_stack, lx_object_stack* call_stack);
void lx_delete_gc_info(lx_gc_info* gc);
lx_object* managed_with_gc(lx_gc_info* gc, lx_object* obj); /* manage `obj` in GC */
void lx_gc_collect(lx_gc_info* gc);


#endif // end of __OBJECT__H_