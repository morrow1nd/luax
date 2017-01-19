#include "./opcode.h"
#include "./base.h"


bool lx_opcode_is_label(unsigned char type)
{
    return 1 <= type && type <= 9;
}

const char* lx_opcode_type_to_string(unsigned char type)
{
    switch (type) {
    OP_LABEL: return "label:";
    OP_LABEL_WHILE_BEGIN: return "label_while_begin:";
    OP_LABEL_WHILE_END: return "label_while_end:";
    OP_LABEL_FOR_BEGIN: return "label_for_begin:";
    OP_LABEL_FOR_END: return "label_for_end:";

    OP_BREAK: return "break";
    OP_CONTINUE: return "continue";
    OP_CALL: return "call";
    OP_RETURN: return "return";
    OP_JMP: return "jmp";
    OP_JZ: return "jz";

    OP_TAG: return "tag";
    OP_POP_TO_TAG: return "pop_to_tag";

    OP_LOCAL: return "local";
    OP_LOCAL_INIT: return "local_init";
    OP_PUSH_ENV: return "push_env";
    OP_POP_ENV: return "pop_env";

    OP_PUSHC_NIL: return "pushc_nil";
    OP_PUSHC_FALSE: return "pushc_false";
    OP_PUSHC_TRUE: return "pushc_true";
    OP_G_TABLE_KEY: return "g_table_key";
    OP_TABLE_KEY: return "table_key";
    OP_PUSHC_EMPTY_TABLE: return "pushc_empty_table";
    OP_PUSHC_STR: return "pushc_str";
    OP_PUSHC_NUMBER: return "pushc_number";
    OP_PUSHC_TABLE: return "pushc_table";
    OP_FUNC_DEF_BEGIN: return "func_def_begin";
    OP_FUNC_DEF_END: return "func_def_end";
    OP_PUSHC_FUNC: return "pushc_func";
    OP_ASSIGN: return "assign";
    OP_ADD_ASSIGN: return "add_assign";
    OP_SUB_ASSIGN: return "sub_assign";
    OP_MUL_ASSIGN: return "mul_assign";
    OP_DIV_ASSIGN: return "div_assign";
    OP_AND: return "and";
    OP_OR: return "or";
    OP_LESS: return "less";
    OP_GREATER: return "greater";
    OP_LESS_EQL: return "less_eql";
    OP_GREATER_EQL: return "greater_eql";
    OP_EQL_EQL: return "eql_eql";
    OP_NOT_EQL: return "not_eql";
    OP_ADD: return "add";
    OP_SUB: return "sub";
    OP_MUL: return "mul";
    OP_DIV: return "div";
    OP_INVERST: return "inverst";
    default:
        assert(false);
    }
    return "error";
}