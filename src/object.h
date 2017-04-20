#ifndef __OBJECT__H_
#define __OBJECT__H_

#include "base.h"
#include "hash/src/uthash.h" /* we use hash lib from http://troydhanson.github.com/uthash/ */


enum LX_OBJECT_TYPE {
    LX_OBJECT_NIL = 48,  /* ascii code '0', 0x30 */
    LX_OBJECT_BOOL,
    LX_OBJECT_NUMBER,
    LX_OBJECT_STRING,
    LX_OBJECT_FUNCTION,
    LX_OBJECT_TABLE,

    LX_OBJECT_TAG, /* inner use */
};


/* type forward declaration */
typedef struct lx_opcodes lx_opcodes;
typedef struct lx_object_table lx_object_table;
typedef struct lx_object lx_object;
typedef struct lx_vm lx_vm;
typedef void(*lx_object_function_ptr_handle)(lx_vm* vm, lx_object* called_obj);


typedef struct lx_object {
    short type;
    char marked;
    char is_singleton;
    float fnumber;
} lx_object;

lx_object* create_object(short type);
void delete_object(lx_object* obj);
bool object_is_jz_zero(lx_object* obj);


/* luax function(achieved in C or luax) */
typedef struct lx_object_function {
    lx_object base; /* simulate class inheritance */

    lx_object_table* env_creator; /* the environment table when this function was created */
    lx_object_function_ptr_handle func_ptr; /* achieved in C */
    const lx_opcodes* func_opcodes; /* achieved in luax code */
} lx_object_function;

lx_object_function* create_object_function_p(lx_object_function_ptr_handle func_ptr, lx_object_table *env_creator);
lx_object_function* create_object_function_ops(const lx_opcodes* func_opcodes, lx_object_table *env_creator);
void delete_object_function(lx_object_function* obj_func);


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

    _object_table_kv* keyvalue_map; /* entry of this hash table */
} lx_object_table;

lx_object_table* create_object_table_raw(); /* create a table with no meta table */
void delete_object_table(lx_object_table* tab);

/* table key-value get and set */
_object_table_kv* object_table_find(lx_object_table* tab, lx_object* k); /* return NULL when didn't find */
_object_table_kv* object_table_find_t(lx_object_table* tab, const char* text, int text_len);
_object_table_kv* object_table_always_found(lx_object_table* tab, lx_object* k); /* return a new key_to_nil when didn't find */
lx_object* object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v);

/* wrapper function for meta table */
lx_object_table* table_get_meta_table(lx_object_table* tab);
void table_set_meta_table(lx_object_table* tab, lx_object_table* new_meta_table);
lx_object* table_meta_element_get(lx_object_table* tab, const char* str);
void table_meta_element_set(lx_object_table* tab, lx_object* k, lx_object* v);
lx_object_function* table_meta_function_get(lx_object_table* tab, const char* str);
void table_meta_function_set(lx_object_table* tab, lx_object* k, lx_object_function* v);


/* luax string(string length stored in text_len) */
typedef struct lx_object_string {
    lx_object base;

    char need_free; /* todo: maybe we could use text_len to store this info, when text_len == -1, it means text is a C type string */
    const char * text;
    int text_len;
} lx_object_string;

/* need_free set to false */
lx_object_string* create_object_string_t(const char * text, int text_len);
/* need_free set to true, we copy the `str` innerlly */
lx_object_string* create_object_string_s(const char* str);
void delete_object_string(lx_object_string* obj);


/* global constant luax object instances */
lx_object* LX_OBJECT_nil();
lx_object* LX_OBJECT_true();
lx_object* LX_OBJECT_false();
lx_object* LX_OBJECT_tag();


/* helper function */
const char* lx_object_type_to_string(int type);
const char* lx_object_to_string(lx_object* obj, char str[]);
void lx_dump_object(lx_object* obj, FILE* fp); /* dump all of this obj. For a table, it would dump it's every key-value. */
void lx_object_inner_to_string(lx_object* obj, char str[]); /* for debug show */
void delete_object_by_type(lx_object* obj);


/* helper macro for type casting */
#define CAST_O (lx_object*)
#define CAST_S (lx_object_string*)
#define CAST_T (lx_object_table*)
#define CAST_F (lx_object_function*)


/* a stack containing lx_object*  */
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


#endif // end of __OBJECT__H_