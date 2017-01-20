#ifndef __OPCODE__H_
#define __OPCODE__H_

#include "base.h"


enum LX_OPCODE_TYPE {
    OP_LABEL = 1,
    OP_LABEL_WHILE_BEGIN,
    OP_LABEL_WHILE_END,
    OP_LABEL_FOR_BEGIN,
    OP_LABEL_FOR_END,

    OP_BREAK = 10,
    OP_CONTINUE,
    OP_CALL,
    OP_RETURN,
    OP_JMP,
    OP_JZ,

    OP_TAG,
    OP_POP_TO_TAG,

    OP_LOCAL,
    OP_LOCAL_INIT,
    OP_PUSH_ENV,
    OP_POP_ENV,
    
    OP_PUSHC_NIL,
    OP_PUSHC_FALSE,
    OP_PUSHC_TRUE,
    OP_G_TABLE_KEY,
    OP_TABLE_KEY,
    OP_PUSHC_EMPTY_TABLE,
    OP_PUSHC_STR,
    OP_PUSHC_NUMBER,
    OP_PUSHC_TABLE,
    OP_FUNC_DEF_BEGIN,
    OP_FUNC_DEF_END,
    OP_PUSHC_FUNC,
    OP_ASSIGN,
    OP_ADD_ASSIGN,
    OP_SUB_ASSIGN,
    OP_MUL_ASSIGN,
    OP_DIV_ASSIGN,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_LESS,
    OP_GREATER,
    OP_LESS_EQL,
    OP_GREATER_EQL,
    OP_EQL_EQL,
    OP_NOT_EQL,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_INVERST,
};

enum LX_OP_EXTRA_INFO {
    OPINFO_tag_for_expr_stmt = 1,
    OPINFO_tag_for_function_define_argc_end,
    OPINFO_tag_for_function_call_argc,
    OPINFO_tag_for_function_call_argc_empty,
    OPINFO_tag_for_assign_stmt_lvalue,
    OPINFO_tag_for_assign_stmt_rvalue,
    OPINFO_tag_for_return_stmt,
    OPINFO_tag_for_immediate_table,
    OPINFO_tag_for_local_declare,
    OPINFO_tag_for_local_declare_with_init,
    OPINFO_tag_for_table_index_ML_expr_MR,
};

typedef struct lx_opcode {
    unsigned char type;
    int extra_info; // used to store extra help info for generate bytecode with good-look comment
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
const char* lx_opcode_type_to_string(unsigned char type);
const char* lx_opcode_to_string(lx_opcode_x* op, char* str);


#endif