#include "./opcode.h"
#include "./base.h"


bool lx_opcode_is_label(unsigned char type)
{
    return 1U <= type && type <= 9U;
}
bool lx_opcode_is_table_get(unsigned char type)
{
    return (type == OP_TABLE_GET || type == OP_TABLE_GET_IMM || type == OP_G_TABLE_GET);
}

const char* lx_opcode_type_to_string(unsigned char type)
{
    switch ((enum LX_OPCODE_TYPE)type) {
    case OP_LABEL: return "label:";
    case OP_LABEL_WHILE_BEGIN: return "label_while_begin:";
    case OP_LABEL_WHILE_END: return "label_while_end:";
    case OP_LABEL_FOR_BODY: return "label_for_body:";
    case OP_LABEL_FOR_BEGIN: return "label_for_begin:";
    case OP_LABEL_FOR_END: return "label_for_end:";

    case OP_BREAK: return "break";
    case OP_CONTINUE: return "continue";
    case OP_CALL: return "call";
    case OP_RETURN: return "return";
    case OP_FUNC_RET_VALUE_SHIFT_TO_1: return "func_ret_value_shift_to_1";
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
    case OP_PUSHC_EMPTY_TABLE: return "pushc_empty_table";
    case OP_PUSHC_STR: return "pushc_str";
    case OP_PUSHC_NUMBER: return "pushc_number";
    case OP_PUSHC_TABLE: return "pushc_table";
    case OP_FUNC_DEF_BEGIN: return "func_def_begin";
    case OP_FUNC_DEF_END: return "func_def_end";
    case OP_PUSHC_FUNC: return "pushc_func";

    case OP_TABLE_GET: return "table_get";
    case OP_TABLE_SET_TKT: return "table_set_tkt";
    case OP_G_TABLE_GET: return "g_table_get";
    case OP_G_TABLE_SET_TKT: return "g_table_set_tkt";
    case OP_TABLE_GET_IMM: return "table_get_imm";
    case OP_TABLE_IMM_SET_TKT: return "table_imm_set_tkt";

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
    case OP_LABEL_FOR_BODY: return "label_for_body:";
    case OP_LABEL_FOR_BEGIN: return "label_for_begin:";
    case OP_LABEL_FOR_END: return "label_for_end:";

    case OP_BREAK: return "break";
    case OP_CONTINUE: return "continue";
    case OP_CALL: return "call";
    case OP_RETURN: return "return";
    case OP_FUNC_RET_VALUE_SHIFT_TO_1: return "func_ret_value_shift_to_1";
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
    case OP_PUSHC_EMPTY_TABLE: return "pushc_empty_table";
    case OP_PUSHC_STR: strcpy(str, "pushc_str "); memcpy(str + 10, op->text, op->text_len); *(str + 10 + op->text_len) = '\0'; return str;
    case OP_PUSHC_NUMBER: sprintf(str, "pushc_number %f", op->fnumber); return str;
    case OP_PUSHC_TABLE: return "pushc_table";
    case OP_FUNC_DEF_BEGIN: return "func_def_begin";
    case OP_FUNC_DEF_END: return "func_def_end";
    case OP_PUSHC_FUNC: return "pushc_func";

    case OP_TABLE_GET: return "table_get";
    case OP_TABLE_SET_TKT: return "table_set_tkt";
    case OP_G_TABLE_GET: strcpy(str, "g_table_get "); memcpy(str + 12, op->text, op->text_len); *(str + 12 + op->text_len) = '\0'; return str;
    case OP_G_TABLE_SET_TKT: strcpy(str, "g_table_set_tkt "); memcpy(str + 16, op->text, op->text_len); *(str + 16 + op->text_len) = '\0'; return str;
    case OP_TABLE_GET_IMM: strcpy(str, "table_get_imm "); memcpy(str + 14, op->text, op->text_len); *(str + 14 + op->text_len) = '\0'; return str;
    case OP_TABLE_IMM_SET_TKT: strcpy(str, "table_imm_set_tkt "); memcpy(str + 18, op->text, op->text_len); *(str + 18 + op->text_len) = '\0'; return str;

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
    return "lx_opcode_to_string error";
}

const char* lx_opcode_expr_info_to_string(int type)
{
    switch ((enum LX_OP_EXTRA_INFO)type) {
    case OPINFO_tag_for_expr_stmt: return "expr_stmt";
    //case OPINFO_tag_for_function_define_argc_end: return "func define argc";
    case OPINFO_tag_for_function_call_argc: return "func call argc";
    case OPINFO_tag_for_function_call_argc_empty: return "func call argc(empty)";
    case OPINFO_tag_for_assign_stmt_lvalue: return "assign_stmt lvalue";
    case OPINFO_tag_for_assign_stmt_rvalue: return "assign_stmt rvalue";
    case OPINFO_tag_for_return_stmt: return "return stmt";
    case OPINFO_tag_for_immediate_table: return "immediate table";
    case OPINFO_tag_for_local_declare: return "local declare";
    case OPINFO_tag_for_local_declare_with_init: return "local declare(init part)";
    case OPINFO_tag_for_table_index_ML_expr_MR: return "`tab[expr]`: label for table index";
    case OPINFO_tag_for_function_return_values_shift_to_1: return "shift func ret value to one value";
    default: assert(false); return "lx_opcode_expr_info_to_string error";
    }
}

void lx_helper_dump_opcode(lx_opcodes* ops, FILE* fp)
{
    char tem[1024];
    char tem2[1024];
    fprintf(fp, ";============ show readable opcode ===============\n");
    for (int i = 0; i < ops->size; ++i) {
        tem[0] = '\0';
        if (!lx_opcode_is_label(ops->arr[i]->type))
            sprintf(tem, "    "); // use 4 spaces to replace \t
        sprintf(tem + strlen(tem), "%s", lx_opcode_to_string((lx_opcode_x*)(ops->arr[i]), tem2));
        if (ops->arr[i]->extra_info != -1) {
            if (strlen(tem) < 40) {
                memset((char*)tem + strlen(tem), ' ', 40 - strlen(tem));
                tem[40] = '\0';
            }
            fprintf(fp, "%s; %s\n", tem, lx_opcode_expr_info_to_string(ops->arr[i]->extra_info));
            //fprintf(fp, "%-6d%s; %s\n", i + 1, tem, lx_opcode_expr_info_to_string(ops->arr[i]->extra_info));
        } else {
            fprintf(fp, "%s\n", tem);
            //fprintf(fp, "%-6d%s\n", i + 1, tem);
        }
    }
    fprintf(fp, ";=========== readable opcode end =====================\n");
}
