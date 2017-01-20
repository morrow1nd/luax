#include "./opcode.h"
#include "./base.h"


bool lx_opcode_is_label(unsigned char type)
{
    return 1 <= type && type <= 9;
}

const char* lx_opcode_type_to_string(unsigned char type)
{
    switch ((enum LX_OPCODE_TYPE)type) {
    case OP_LABEL: return "label:";
    case OP_LABEL_WHILE_BEGIN: return "label_while_begin:";
    case OP_LABEL_WHILE_END: return "label_while_end:";
    case OP_LABEL_FOR_BEGIN: return "label_for_begin:";
    case OP_LABEL_FOR_END: return "label_for_end:";

    case OP_BREAK: return "break";
    case OP_CONTINUE: return "continue";
    case OP_CALL: return "call";
    case OP_RETURN: return "return";
    case OP_JMP: return "jmp";
    case OP_JZ: return "jz";

    case OP_TAG: return "tag";
    case OP_POP_TO_TAG: return "pop_to_tag";

    case OP_LOCAL: return "local";
    case OP_LOCAL_INIT: return "local_init";
    case OP_PUSH_ENV: return "push_env";
    case OP_POP_ENV: return "pop_env";

    case OP_PUSHC_NIL: return "pushc_nil";
    case OP_PUSHC_FALSE: return "pushc_false";
    case OP_PUSHC_TRUE: return "pushc_true";
    case OP_G_TABLE_KEY: return "g_table_key";
    case OP_TABLE_KEY: return "table_key";
    case OP_PUSHC_EMPTY_TABLE: return "pushc_empty_table";
    case OP_PUSHC_STR: return "pushc_str";
    case OP_PUSHC_NUMBER: return "pushc_number";
    case OP_PUSHC_TABLE: return "pushc_table";
    case OP_FUNC_DEF_BEGIN: return "func_def_begin";
    case OP_FUNC_DEF_END: return "func_def_end";
    case OP_PUSHC_FUNC: return "pushc_func";
    case OP_ASSIGN: return "assign";
    case OP_ADD_ASSIGN: return "add_assign";
    case OP_SUB_ASSIGN: return "sub_assign";
    case OP_MUL_ASSIGN: return "mul_assign";
    case OP_DIV_ASSIGN: return "div_assign";
    case OP_AND: return "and";
    case OP_OR: return "or";
    case OP_NOT: return "not";
    case OP_LESS: return "less";
    case OP_GREATER: return "greater";
    case OP_LESS_EQL: return "less_eql";
    case OP_GREATER_EQL: return "greater_eql";
    case OP_EQL_EQL: return "eql_eql";
    case OP_NOT_EQL: return "not_eql";
    case OP_ADD: return "add";
    case OP_SUB: return "sub";
    case OP_MUL: return "mul";
    case OP_DIV: return "div";
    case OP_INVERST: return "inverst";
    default:
        assert(false);
    }
    return "error";
}


const char* lx_opcode_to_string(lx_opcode_x* op, char* str)
{
    switch ((enum LX_OPCODE_TYPE)op->_.type) {
    case OP_LABEL: return "label:";
    case OP_LABEL_WHILE_BEGIN: return "label_while_begin:";
    case OP_LABEL_WHILE_END: return "label_while_end:";
    case OP_LABEL_FOR_BEGIN: return "label_for_begin:";
    case OP_LABEL_FOR_END: return "label_for_end:";

    case OP_BREAK: return "break";
    case OP_CONTINUE: return "continue";
    case OP_CALL: return "call";
    case OP_RETURN: return "return";
    case OP_JMP: sprintf(str, "jmp %d", op->inumber); return str;
    case OP_JZ: sprintf(str, "jz %d", op->inumber); return str;

    case OP_TAG: return "tag";
    case OP_POP_TO_TAG: return "pop_to_tag";

    case OP_LOCAL: return "local";
    case OP_LOCAL_INIT: return "local_init";
    case OP_PUSH_ENV: return "push_env";
    case OP_POP_ENV: return "pop_env";

    case OP_PUSHC_NIL: return "pushc_nil";
    case OP_PUSHC_FALSE: return "pushc_false";
    case OP_PUSHC_TRUE: return "pushc_true";
    case OP_G_TABLE_KEY: strcpy(str, "g_table_key "); memcpy(str + 12, op->text, op->text_len); *(str + 12 + op->text_len) = '\0'; return str;
    case OP_TABLE_KEY: strcpy(str, "table_key "); memcpy(str + 10, op->text, op->text_len); *(str + 10 + op->text_len) = '\0'; return str;
    case OP_PUSHC_EMPTY_TABLE: return "pushc_empty_table";
    case OP_PUSHC_STR: strcpy(str, "pushc_str "); memcpy(str + 10, op->text, op->text_len); *(str + 10 + op->text_len) = '\0'; return str;
    case OP_PUSHC_NUMBER: sprintf(str, "pushc_number %f", op->fnumber); return str;
    case OP_PUSHC_TABLE: return "pushc_table";
    case OP_FUNC_DEF_BEGIN: return "func_def_begin";
    case OP_FUNC_DEF_END: return "func_def_end";
    case OP_PUSHC_FUNC: return "pushc_func";
    case OP_ASSIGN: return "assign";
    case OP_ADD_ASSIGN: return "add_assign";
    case OP_SUB_ASSIGN: return "sub_assign";
    case OP_MUL_ASSIGN: return "mul_assign";
    case OP_DIV_ASSIGN: return "div_assign";
    case OP_AND: return "and";
    case OP_OR: return "or";
    case OP_NOT: return "not";
    case OP_LESS: return "less";
    case OP_GREATER: return "greater";
    case OP_LESS_EQL: return "less_eql";
    case OP_GREATER_EQL: return "greater_eql";
    case OP_EQL_EQL: return "eql_eql";
    case OP_NOT_EQL: return "not_eql";
    case OP_ADD: return "add";
    case OP_SUB: return "sub";
    case OP_MUL: return "mul";
    case OP_DIV: return "div";
    case OP_INVERST: return "inverst";
    default:
        assert(false);
    }
    return "error";
}