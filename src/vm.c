#include "vm.h"
#include "base.h"
#include "mem.h"
#include "opcode.h"


const lx_object LX_OBJECT_nil = 
{
    .type = LX_OBJECT_NIL
};
const lx_object LX_OBJECT_true = 
{
    .type = LX_OBJECT_BOOL,
    .fnumber = 1.0f
};
const lx_object LX_OBJECT_false = 
{
    .type = LX_OBJECT_BOOL,
    .fnumber = 0.0f
};
const lx_object_table_key LX_OBJECT_TABLE_key_nil_to_nil = 
{
    .key = NULL,
    .value = (lx_object*)&LX_OBJECT_nil
};


//
// inner function
//
static lx_object* _new_object(int type)
{
    lx_object* obj = LX_NEW(lx_object);
    obj->type = type;
    return obj;
}
static int aaa(lx_vm* vm, char* str)
{

}

int _op_add_assign(lx_object_table_key* lvalue, lx_object* rvalue)
{
    lx_object* l = lvalue->value;
    // todo
    l->fnumber += rvalue->fnumber;
}
int _op_sub_assign(lx_object_table_key* lvalue, lx_object* rvalue)
{
    lx_object* l = lvalue->value;
    // todo
    l->fnumber -= rvalue->fnumber;
}
int _op_mul_assign(lx_object_table_key* lvalue, lx_object* rvalue)
{
    lx_object* l = lvalue->value;
    // todo
    l->fnumber *= rvalue->fnumber;
}
int _op_div_assign(lx_object_table_key* lvalue, lx_object* rvalue)
{
    lx_object* l = lvalue->value;
    // todo
    l->fnumber /= rvalue->fnumber;
}

static int _vm_run(lx_vm* vm, lx_object_function* func_obj)
{
    if (func_obj->base.type != LX_OBJECT_FUNCTION) {
        // todo
        printf("VM ERROR: func_obj->type != LX_OBJECT_FUNCTION\n");
        return -1;
    }
    lx_vm_stack* stack = vm->stack;
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
        case OP_JZ:{
            lx_object* condition = lx_vm_stack_pop(stack);
            if (lx_object_is_jz_zero(condition)) {
                int label_count = ((lx_opcode_x *)ops->arr[i])->inumber;
                int direction = label_count / abs(label_count);
                label_count = abs(label_count);
                int count = 0;
                int f = i;
                for (; count < label_count; f += direction) {
                    if (f < 0 || f > ops->size) {
                        // todo
                        printf("VM ERROR: can't find the distantion of jz\n");
                        assert(false);
                    }
                    if(lx_opcode_is_label(ops->arr[f]->type))
                        count++;
                }
                i = f - 1;
            }
            continue;
        }

        case OP_TAG:{
            lx_vm_stack_push(stack, _new_object(LX_OBJECT_TAG));
            continue;
        }
        case OP_POP_TO_TAG: {
            lx_object* obj;
            for(obj = lx_vm_stack_pop(stack); obj && obj->type != LX_OBJECT_TAG; obj = lx_vm_stack_pop(stack))
                ;
            if (obj == NULL) {
                //todo
                printf("VM ERROR: can't find the tag of pop_to_tag in stack\n");
                assert(false);
            }
            continue;
        }

        case OP_LOCAL: {
            lx_object* obj;
            for (obj = lx_vm_stack_pop(stack); obj && obj->type != LX_OBJECT_TAG; obj = lx_vm_stack_pop(stack)){
                if (obj->type != LX_OBJECT_STRING) {
                    //todo
                    printf("VM ERROR: `local` var should be a string\n");
                    assert(false);
                }
                lx_object_table_replace(func_obj->_G, obj, &LX_OBJECT_nil); /* init to nil */
            }
            if (obj == NULL) {
                //todo
                printf("VM ERROR: can't find the tag of local in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_LOCAL_INIT: {
            int tag_v = stack->curr;
            while (stack->arr[tag_v]->type != LX_OBJECT_TAG) {
                tag_v--;
                if (tag_v < 0) {
                    //todo
                    printf("VM ERROR: can't find the tag of local_init in stack\n");
                    assert(false);
                }
            }
            int key = tag_v - 1;
            int value = stack->curr;
            while (stack->arr[key]->type != LX_OBJECT_TAG) {
                if (stack->arr[key]->type != LX_OBJECT_STRING) {
                    //todo
                    printf("VM ERROR: `local` var should be a string(2)\n");
                    assert(false);
                }
                if(stack->arr[value]->type != LX_OBJECT_TAG){
                    lx_object_table_replace(func_obj->_G, stack->arr[key], stack->arr[value]);
                    --value;
                }else
                    lx_object_table_replace(func_obj->_G, stack->arr[key], &LX_OBJECT_nil);

                --key;
            }
            stack->curr = key - 1; /* pop_to_tag */
            continue;
        }
        case OP_PUSH_ENV: {
            // todo
            continue;
        }
        case OP_POP_ENV: {
            //tod
            continue;
        }

        case OP_PUSHC_NIL: {
            lx_vm_stack_push(stack, _new_object(LX_OBJECT_NIL));
            continue;
        }
        case OP_PUSHC_FALSE: {
            lx_vm_stack_push(stack, _new_object(LX_OBJECT_BOOL))->fnumber = false;
            continue;
        }
        case OP_PUSHC_TRUE: {
            lx_vm_stack_push(stack, _new_object(LX_OBJECT_BOOL))->fnumber = true;
            continue;
        }
        case OP_G_TABLE_KEY: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object_string key;
            key.base.type = LX_OBJECT_STRING;
            key.text = op->text;
            key.text_len = op->text_len;
            lx_object_table_key* result = lx_object_table_always_found(func_obj->_G, &key);
            lx_vm_stack_push(stack, result);
            continue;
        }
        case OP_TABLE_KEY: {
            lx_object* key = lx_vm_stack_pop(stack);
            lx_object* o;
            for(lx_object* o = lx_vm_stack_pop(stack); o && o->type != LX_OBJECT_TAG;)
                o = lx_vm_stack_pop(stack);
            if (o == NULL) {
                // todo
                printf("VM Error: stack out of range");
                assert(false);
            }
            lx_object* tab = lx_vm_stack_pop(stack);
            lx_vm_stack_push(stack, lx_object_table_always_found(tab, key));
            continue;
        }
        case OP_TABLE_KEY_IMM: {
            lx_object_table* tab = lx_vm_stack_pop(stack);
            if (tab == NULL) {
                //todo
                printf("erro; out of range");
                assert(false);
            }
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object_string key;
            key.base.type = LX_OBJECT_STRING;
            key.text = op->text;
            key.text_len = op->text_len;
            lx_object_table_key* result = lx_object_table_always_found(tab, &key);
            lx_vm_stack_push(stack, result);
            continue;
        }
        case OP_PUSHC_EMPTY_TABLE: {
            lx_object_table* table = lx_create_object_table();
            lx_vm_stack_push(stack, table);
            continue;
        }
        case OP_PUSHC_STR: {
            lx_object_string* o_str = LX_NEW(lx_object_string);
            lx_opcode_x* op = ops->arr[i];
            o_str->text = op->text;
            o_str->text_len = op->text_len;
            o_str->base.type = LX_OBJECT_STRING;
            continue;
        }
        case OP_PUSHC_NUMBER: {
            lx_vm_stack_push(stack, _new_object(LX_OBJECT_NUMBER))->fnumber = ((lx_opcode_x *)(ops->arr[i]))->fnumber;
            continue;
        }
        case OP_PUSHC_TABLE: {
            //todo
            continue;
        }
        case OP_FUNC_DEF_BEGIN: {
            //todo
            continue;
        }
        case OP_FUNC_DEF_END: {
            //todo
            continue;
        }
        case OP_PUSHC_FUNC: {
            //todo
            continue;
        }
        case OP_ASSIGN: {
            int rvalue_begin = stack->curr;
            int opi = rvalue_begin;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                opi--;
            }
            int left_key_begin = opi - 1;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                if(stack->arr[rvalue_begin]->type != LX_OBJECT_TAG){
                    ((lx_object_table_key *)(stack->arr[opi]))->value = stack->arr[rvalue_begin];
                    rvalue_begin--;
                }else
                    ((lx_object_table_key *)(stack->arr[opi]))->value = &LX_OBJECT_nil;

                opi--;
            }
            stack->curr = opi-1;
            continue;
        }
        case OP_ADD_ASSIGN: {
            int rvalue_begin = stack->curr;
            int opi = rvalue_begin;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                opi--;
            }
            int left_key_begin = opi - 1;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
                    _op_add_assign((lx_object_table_key *)(stack->arr[opi]), stack->arr[rvalue_begin]);
                    rvalue_begin--;
                } else
                    _op_add_assign((lx_object_table_key *)(stack->arr[opi]), &LX_OBJECT_nil);

                opi--;
            }
            stack->curr = opi - 1;
            continue;
        }
        case OP_SUB_ASSIGN: {
            int rvalue_begin = stack->curr;
            int opi = rvalue_begin;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                opi--;
            }
            int left_key_begin = opi - 1;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
                    _op_sub_assign((lx_object_table_key *)(stack->arr[opi]), stack->arr[rvalue_begin]);
                    rvalue_begin--;
                } else
                    _op_sub_assign((lx_object_table_key *)(stack->arr[opi]), &LX_OBJECT_nil);

                opi--;
            }
            stack->curr = opi - 1;
            continue;
        }
        case OP_MUL_ASSIGN: {
            int rvalue_begin = stack->curr;
            int opi = rvalue_begin;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                opi--;
            }
            int left_key_begin = opi - 1;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
                    _op_mul_assign((lx_object_table_key *)(stack->arr[opi]), stack->arr[rvalue_begin]);
                    rvalue_begin--;
                } else
                    _op_mul_assign((lx_object_table_key *)(stack->arr[opi]), &LX_OBJECT_nil);

                opi--;
            }
            stack->curr = opi - 1;
            continue;
        }
        case OP_DIV_ASSIGN: {
            int rvalue_begin = stack->curr;
            int opi = rvalue_begin;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                opi--;
            }
            int left_key_begin = opi - 1;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
                    _op_div_assign((lx_object_table_key *)(stack->arr[opi]), stack->arr[rvalue_begin]);
                    rvalue_begin--;
                } else
                    _op_div_assign((lx_object_table_key *)(stack->arr[opi]), &LX_OBJECT_nil);

                opi--;
            }
            stack->curr = opi - 1;
            continue;
        }
        case OP_AND: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber && b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_OR: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber || b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_NOT: {
            lx_object* a = lx_vm_stack_pop(stack);
            if (a) {
                a->fnumber = ! a->fnumber;
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_LESS: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber < b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_GREATER: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber > b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_LESS_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber <= b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_GREATER_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber >= b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_EQL_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber == b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_NOT_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = (float)(a->fnumber != b->fnumber);
                a->type = LX_OBJECT_BOOL;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_ADD: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = a->fnumber + b->fnumber;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_SUB: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = a->fnumber - b->fnumber;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_MUL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                a->fnumber = a->fnumber * b->fnumber;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_DIV: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b){
                if (b->fnumber == 0.0f) {
                    // todo
                    printf("VM ERROR: div by 0 error\n");
                    assert(false);
                }
                a->fnumber = a->fnumber / b->fnumber;
                lx_vm_stack_push(stack, a);
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_INVERST: {
            if (0 <= stack->curr && stack->curr < stack->capacity) {
                stack->arr[stack->curr]->fnumber = - stack->arr[stack->curr]->fnumber;
            } else {
                // todo
                printf("VM ERROR: no item in stack\n");
                assert(false);
            }
            continue;
        }
        default:
            // todo
            printf("VM ERROR: vm_run comes to default\n");
            assert(false);
        }
    }
}



bool lx_object_is_jz_zero(lx_object* obj)
{
    if(obj->type == LX_OBJECT_NIL 
        || (obj->type == LX_OBJECT_BOOL && obj->fnumber == 0)
        || (obj->type == LX_OBJECT_NUMBER && obj->fnumber == 0)
        )
        return true;
    else
        return false;
}


lx_object_table* lx_create_object_table()
{
    lx_object_table* tab = LX_NEW(lx_object_table);
    tab->base.type = LX_OBJECT_TABLE;
    tab->keyvalue_map = NULL; /* important! initialize to NULL */
    return tab;
}
void lx_delete_object_table(lx_object_table* tab)
{
    lx_object_table_key *current, *tmp;

    HASH_ITER(hh, tab->keyvalue_map, current, tmp) {
        HASH_DEL(tab->keyvalue_map, current);  /* delete it (users advances to next) */
        //free(current);             /* free it */
    }
    lx_free(tab);
}
char* lx_object_get_id(lx_object* obj, char id[])
{
    id[0] = (char)obj->type;
    if (obj->type == LX_OBJECT_NIL) {
        id[1] = '\0';
        return id;
    }
    if (obj->type == LX_OBJECT_NUMBER) {
        memcpy((char*)(id + 1), &(obj->fnumber), sizeof(float));
        id[1 + sizeof(float)] = '\0';
        return id;
    }
    if (obj->type == LX_OBJECT_BOOL) {
        id[1] = obj->fnumber ? 1 : 0;
        id[2] = '\0';
        return id;
    }
    if (obj->type == LX_OBJECT_FUNCTION) {
        memcpy((char*)(id + 1), ((lx_object_function *)obj)->func_opcodes, sizeof(lx_opcodes*));
        id[1 + sizeof(lx_opcodes*)] = '\0';
        return id;
    }
    if (obj->type == LX_OBJECT_TABLE) {
        memcpy((char*)(id + 1), obj, sizeof(lx_object_table*));
        id[1 + sizeof(lx_object_table*)] = '\0';
        return id;
    }
    if (obj->type == LX_OBJECT_STRING) {
        memcpy((char*)(id + 1), ((lx_object_string *)obj)->text, ((lx_object_string *)obj)->text_len);
        id[1 + ((lx_object_string *)obj)->text_len] = '\0';
        return id;
    }
    // todo
    printf("VM ERROR: ...\n");
    assert(false);
    return "";
}
lx_object_table_key* lx_object_table_find(lx_object_table* tab, lx_object* k)
{
    lx_object_table_key* result = NULL;
    char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
    lx_object_get_id(k, id);
    HASH_FIND_STR(tab->keyvalue_map, id, result);
    return result; // return NULL when not found
}
lx_object_table_key* lx_object_table_always_found(lx_object_table* tab, lx_object* k)
{
    lx_object_table_key* key = lx_object_table_find(tab, k);
    if (!key) {
        //lx_object_table_replace(tab, k, &LX_OBJECT_nil);
        char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
        lx_object_get_id(k, id);
        int id_len = strlen(id);

        key = LX_NEW(lx_object_table_key);
        key->key = lx_malloc(id_len + 1);
        memcpy(key->key, id, id_len + 1);
        key->value = &LX_OBJECT_nil;

        HASH_ADD_KEYPTR(hh, tab->keyvalue_map, id, id_len, key);
    }
    return key;
}
//lx_object_table_key* lx_object_table_find_s(lx_object_table* tab, char* k)
//{
//    lx_object_table_key* result = NULL;
//    HASH_FIND_STR(tab->keyvalue_map, k, result);
//    return result;
//}
lx_object* lx_object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v)
{
    if (k->type == LX_OBJECT_TABLE_KEY) {
        k = ((lx_object_table_key *)k)->value;
    }

    if(k->type == LX_OBJECT_NIL)
        return &LX_OBJECT_nil;

    lx_object* old;
    lx_object_table_key* result = NULL;
    lx_object_table_key* kv;
    char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
    lx_object_get_id(k, id);
    size_t id_len = strlen(id);
    HASH_FIND_STR(tab->keyvalue_map, id, result);
    if (result) {
        HASH_DEL(tab->keyvalue_map, result);
        if(id_len > strlen(result->key)){
            lx_free(result->key);
            result->key = lx_malloc(id_len + 1);
        }
        kv = result;
        old = result->value;
    } else {
        old = &LX_OBJECT_nil; // no old
        kv = LX_NEW(lx_object_table_key);
    }
    memcpy(kv->key, id, id_len + 1);
    kv->value = v;

    HASH_ADD_KEYPTR( hh, tab->keyvalue_map, id, id_len, kv);
    return old;
}
lx_object* lx_object_table_replace_s(lx_object_table* tab, const char* text, int text_len, lx_object* v)
{
    lx_object_string key;
    key.base.type = LX_OBJECT_STRING;
    key.text = text;
    key.text_len = text_len;
    return lx_object_table_replace(tab, &key, v);
}


lx_vm_stack* lx_create_vm_stack(int capacity_enlarge_grain)
{
    lx_vm_stack* s = LX_NEW(lx_vm_stack);
    s->capacity_enlarge_grain = capacity_enlarge_grain;
    s->capacity = capacity_enlarge_grain;
    s->curr = -1;
    s->arr = (lx_object**)lx_malloc(sizeof(lx_object*) * s->capacity_enlarge_grain);
    return s;
}
void lx_delete_vm_stack(lx_vm_stack* s)
{
    lx_free(s->arr);
    lx_free(s);
}
lx_object* lx_vm_stack_push(lx_vm_stack* s, lx_object* obj)
{
    if (s->curr == s->capacity) {
        /* enarge it */
        lx_object** objs = (lx_object**)lx_malloc(sizeof(lx_object*) * (s->capacity + s->capacity_enlarge_grain));
        memcpy(objs, s->arr, s->capacity);
        s->capacity += s->capacity_enlarge_grain;
        lx_free(s->arr);
        s->arr = objs;
    }
    s->curr ++;
    s->arr[s->curr] = obj;
    return obj;
}
lx_object* lx_vm_stack_pop(lx_vm_stack* s)
{
    if (s->curr < 0 || s->curr >= s->capacity)
        return NULL;
    s->curr --;
#if (defined _DEBUG) || (defined DEBUG) 
    lx_object* ret = s->arr[s->curr + 1];
    s->arr[s->curr + 1] = NULL; // clean it
    return ret;
#endif
    return s->arr[s->curr + 1];
}


lx_vm* lx_create_vm ()
{
    lx_vm* vm = LX_NEW(lx_vm);
    //todo
    return vm;
}

int lx_vm_run (lx_vm* vm, lx_object* func_obj)
{
    int ret = _vm_run(vm, func_obj);

    return ret;
}

void lx_delete_vm (lx_vm* vm)
{

    lx_free(vm);
}

// helper function
const char* lx_object_to_string(lx_object* obj, char str[])
{
    switch (obj->type) {
    case LX_OBJECT_TAG: return "tag";
    case LX_OBJECT_TABLE_KEY: { lx_object_table_key* k = (lx_object_table_key*)obj; 
        sprintf(str, "TK{%s, %p}", k->key, k->value);  return str;
    }
    case LX_OBJECT_TABLE:
        sprintf(str, "T{%p}", obj); return str;
    case LX_OBJECT_FUNCTION:
        sprintf(str, "F{%p}", obj); return str;
    case LX_OBJECT_NUMBER:
        sprintf(str, "N{%f}", obj->fnumber); return str;
    case LX_OBJECT_STRING:
        strcpy(str, "S{"); lx_object_get_id(obj, str + strlen(str)); strcpy(str + strlen(str), "}"); return str;
    case LX_OBJECT_BOOL:
        sprintf(str, "B{%f}", obj->fnumber); return str;
    case LX_OBJECT_NIL: return "nil";
    default:
        assert(false);
    }
}
void lx_dump_vm_stack(lx_vm* vm)
{
    char tem[1024 * 4]; // todo
    printf("=========== dump stack =============\n");
    printf("            ---------------\n");
    for (int i = 0; i <= vm->stack->curr; ++i) {
        sprintf(tem, "        %-4d| ", i);
        lx_object_to_string(vm->stack->arr[i], tem + strlen(tem));
        printf("%s\n", tem);
    }
    if(vm->stack->curr == -1)
        printf("            | <empty>\n");
    printf("            --------------- (top)\n");
    printf("stack:%p\n", vm->stack);
    printf("=========== dump stack end ===========\n");
}
void lx_dump_vm_status(lx_vm* vm)
{
    printf("=========== dump vm status =============\n");
    printf("vm:%p\n", vm);
   // printf(""\n, )
    printf("=============== dump end ===============\n");
}