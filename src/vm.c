#include "vm.h"
#include "base.h"
#include "mem.h"
#include "opcode.h"


//
// inner function
//
static int __vm_run(lx_vm* vm, lx_object* func_obj)
{
    if (func_obj->type != LX_OBJECT_FUNCTION) {
        // todo
        printf("VM ERROR: func_obj->type != LX_OBJECT_FUNCTION\n");
        return -1;
    }
    lx_opcodes* ops = func_obj->func_opcodes;
    for (int i = 0; i < ops->size && i >= 0; ++i) {
        switch (ops->arr[i]->type)
        {
        case OP_LABEL:
        case OP_LABEL_WHILE_BEGIN:
        case OP_LABEL_WHILE_END:
        case OP_LABEL_FOR_BODY:
        case OP_LABEL_FOR_BEGIN:
        case OP_LABEL_FOR_END:
            continue;

        case OP_BREAK: {
            int f = i;
            for (; f < ops->size; ++f) {
                if(ops->arr[f]->type == OP_LABEL_WHILE_END || ops->arr[f]->type == OP_LABEL_FOR_END)
                    break;
            }
            if (f < ops->size) {
                i = f - 1;
                continue;
            } else {
                // todo
                printf("VM ERROR: can't find a while_end or for_end to break\n");
                assert(false);
            }
            continue;
        }
        case OP_CONTINUE: {
            // todo
            continue;
        }
        case OP_CALL: {
            // todo
            continue;
        }
        case OP_RETURN: {
            // todo
            continue;
        }
        case OP_JMP: {
            int label_count = ((lx_opcode_x *)ops->arr[i])->inumber;
            int direction = label_count / abs(label_count);
            label_count = abs(label_count);
            int count = 0;
            int f = i;
            for(; count < label_count; f += direction){
                if (f < 0 || f > ops->size) {
                    // todo
                    printf("VM ERROR: can't find the distantion of jmp\n");
                    assert(false);
                }
                if(lx_opcode_is_label(ops->arr[f]->type))
                    count++;
            }
            i = f - 1;
            continue;
        }
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
        case OP_TABLE_KEY_IMM: return "table_key_imm";
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
            // todo
            printf("VM ERROR: vm_run comes to default\n");
            assert(false);
        }
    }
}



lx_vm_stack* lx_vm_stack_create()
{
    lx_vm_stack* s = LX_NEW(lx_vm_stack);
    s->capacity_grain = 32;
    s->size = 32;
    s->curr = 0;
    s->arr = (lx_object**)lx_malloc(sizeof(lx_object*) * s->capacity_grain);
    return s;
}
void lx_vm_stack_delete(lx_vm_stack* s)
{
    lx_free(s->arr);
    lx_free(s);
}
lx_object* lx_vm_stack_push(lx_vm_stack* s, lx_object* obj)
{
    if (s->curr == s->size) {
        /* enarge it */
        lx_object** objs = (lx_object**)lx_malloc(sizeof(lx_object*) * (s->size + s->capacity_grain));
        memcpy(objs, s->arr, s->size);
        s->size += s->capacity_grain;
        lx_free(s->arr);
        s->arr = objs;
    }
    s->curr ++;
    s->arr[s->curr] = obj;
    return obj;
}
lx_object* lx_vm_stack_pop(lx_vm_stack* s)
{
    if (s->curr < 0 || s->curr >= s->size)
        return NULL;
    s->curr --;
#if (defined _DEBUG) || (defined DEBUG) 
    lx_object* ret = s->arr[s->curr + 1];
    s->arr[s->curr + 1] = NULL; // clean it
    return ret;
#endif
    return s->arr[s->curr + 1];
}


lx_vm* lx_vm_create ()
{
    lx_vm* vm = LX_NEW(lx_vm);
    
    return vm;
}

int lx_vm_run (lx_vm* vm, lx_object* func_obj)
{
    int ret = __vm_run(vm, func_obj);

    return ret;
}

void lx_vm_delete (lx_vm* vm)
{

    lx_free(vm);
}