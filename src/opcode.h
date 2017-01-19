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

bool lx_opcode_is_label(unsigned char type);


typedef struct lx_opcode {
    unsigned char type;
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


#endif