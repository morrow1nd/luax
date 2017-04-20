#ifndef __OPCODE__H_
#define __OPCODE__H_

#include "base.h"


enum LX_OPCODE_TYPE {
    /* label: used as aim position for `jmp`, `jz`, `break`, `continue` */
    OP_LABEL = 1,
    OP_LABEL_WHILE_BEGIN,
    OP_LABEL_WHILE_END,
    OP_LABEL_FOR_BODY,
    OP_LABEL_FOR_BEGIN,
    OP_LABEL_FOR_END,

    OP_BREAK = 10,                  // break
    OP_CONTINUE,                    // continue
    OP_CALL,                        // call a luax function
    OP_RETURN,                      // return (expr)
    OP_JMP,                         // jump to someplace
    OP_JZ,                          // if the top element of stack is zero, jump to someplace, and remove the top element in the stack.

    OP_TAG,                         // push a tag to the stack
    OP_POP_TO_TAG,                  // pop elements from the stack, until meet the first tag, pop the tag too.
    OP_VALUES_SHIFT_TO_1,           // shift the values in the runtime stack to one value(ended by a tag, this tag is pushed by caller)
                                    // if there is no value, use a nil to be the one

    OP_LOCAL,                       // used to declare variable(s) in current function's local variable table, these variables are inited to nil
    OP_LOCAL_INIT,                  // declare variable(s) and init them with some values.
    OP_PUSH_ENV,                    // used to achieve if-while-statement's local namespace
    OP_POP_ENV,
    
    OP_PUSHC_NIL,                   // push a `nil` to the stack
    OP_PUSHC_FALSE,                 // push a `false` to the stack
    OP_PUSHC_TRUE,                  // push a `true` to the stack
    OP_PUSHC_EMPTY_TABLE,           // push a new empty table to the stack, example: {}
    OP_PUSHC_STR,                   // create a string from a immediate string and push it to the stack
    OP_PUSHC_NUMBER,                // push a number to the stack
    OP_PUSHC_TABLE,                 // push a inited table to the stack, example: { 'key1' : 123, 'subtab' : {} }
    OP_FUNC_DEF_BEGIN,              // mark the start of a function's opcodes
    OP_FUNC_DEF_END,                // mark the end of a function's opcodes
    OP_PUSHC_FUNC,                  // push a function to the stack

    /* table access */
    OP_TABLE_GET,                   // push the value, got by a specify key in a table, to the stack
    OP_TABLE_SET_TKT,               // modify the stack to meet the requirement of calling `table_set(tab, key, value)`
    OP_G_TABLE_GET,                 // access variable from current environment
    OP_G_TABLE_SET_TKT,
    OP_TABLE_GET_IMM,               // the key is provided as a immediate string
    OP_TABLE_IMM_SET_TKT,

    OP_ASSIGN,                      // Example: `a = 1`, `tab[key] = 1`, `tab.name = 'Tom'`, `func()["key1"] = 1, 2, 3`, `tab[1], a = 1, 2, 3`
    OP_ADD_ASSIGN,                  // `a += 1`
    OP_SUB_ASSIGN,
    OP_MUL_ASSIGN,
    OP_DIV_ASSIGN,

    OP_AND,                         // a != nil and a.name == 'hi'
    OP_OR,                          // a == nil or b > 2
    OP_NOT,                         // not a == nil
    OP_LESS,                        // a < b
    OP_GREATER,                     // a > b
    OP_LESS_EQL,                    // a <= b
    OP_GREATER_EQL,                 // a >= b
    OP_EQL_EQL,                     // a == b
    OP_NOT_EQL,                     // a != b
    OP_ADD,                         // a + b
    OP_SUB,                         // a - b
    OP_MUL,                         // a * b
    OP_DIV,                         // a / b
    OP_INVERST,                     // -a
};

enum LX_OP_EXTRA_INFO {
    OPINFO_tag_for_expr_stmt = 1,
    OPINFO_tag_for_function_call_argc,
    OPINFO_tag_for_function_call_argc_empty,
    OPINFO_tag_for_assign_stmt_lvalue,
    OPINFO_tag_for_assign_stmt_rvalue,
    OPINFO_tag_for_return_stmt,
    OPINFO_tag_for_immediate_table,
    OPINFO_tag_for_local_declare,
    OPINFO_tag_for_local_declare_with_init,
    OPINFO_tag_for_table_index_ML_expr_MR,
    OPINFO_tag_for_values_shift_to_1,
};

typedef struct lx_opcode {
    unsigned char type;
    int extra_info; // stores a LX_OP_EXTRA_INFO, it's used to store extra help info for generate bytecode with good-look comment
} lx_opcode;

typedef struct lx_opcode_x {
    lx_opcode _;
    char * text;
    union {
        int text_len;
        int inumber;
        float fnumber;
    };
} lx_opcode_x;


typedef struct lx_opcodes {
    int size;
    int capacity;
    lx_opcode** arr;
} lx_opcodes;


bool lx_opcode_is_label(unsigned char type);
bool lx_opcode_is_table_get(unsigned char type);
const char* lx_opcode_type_to_string(unsigned char type);
const char* lx_opcode_to_string(lx_opcode_x* op, char* str);
const char* lx_opcode_expr_info_to_string(int type);

void lx_helper_dump_opcode(const lx_opcodes* ops, FILE* fp);

#endif