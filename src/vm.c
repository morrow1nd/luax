#include "vm.h"
#include "base.h"
#include "mem.h"
#include "opcode.h"

static int _vm_run_opcodes(lx_vm* vm, lx_object_function* func_obj, lx_object_table* _env);

static void _op_pop_to_tag(lx_vm_stack* s)
{
    lx_object* obj;
    for (obj = lx_vm_stack_pop(s); obj && obj->type != LX_OBJECT_TAG; obj = lx_vm_stack_pop(s))
        ;
    if (obj == NULL) {
        //todo
        printf("VM ERROR: can't find the tag of pop_to_tag in stack\n");
        assert(false);
    }
}

static int _op_call(lx_vm* vm, lx_object* obj) // obj -> called_obj(function or table)
{
    if (obj->type == LX_OBJECT_FUNCTION) {
        lx_object_function* obj_func = (lx_object_function*)obj;
        if (obj_func->func_opcodes) {
            _vm_run_opcodes(vm, obj_func, lx_create_object_env_table_with_father_env(obj_func->env_creator));
        } else if (obj_func->func_ptr) {
            obj_func->func_ptr(vm->stack, obj);
        } else {
            assert(false && "error: lx_object_function no callable ptr");//todo
        }
        return 0;
    }
    if (obj->type == LX_OBJECT_TABLE) {
        lx_vm_stack_push(vm->stack, obj); // `_call` needs this
        return _op_call(vm, CAST_O lx_meta_function_get((lx_object_table*)obj, "_call"));
    }

    assert(false && "error: _op_call: obj is not callable");//todo
    return -1;
}


//
// inside functions achieved in C
//

// return the meta table of tab
// luax example:
//      m = meta_table(tab);
//      m = meta_table(func_ret_mutli_value());
void _meta_table(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_table* tab = (lx_object_table*)lx_vm_stack_pop(s);
    lx_object* tag = lx_vm_stack_pop(s);

    for (; tag->type != LX_OBJECT_TAG; tag = lx_vm_stack_pop(s)) {
        tab = (lx_object_table*)tag;
    }

    if (tab->base.type != LX_OBJECT_TABLE) {
        lx_vm_stack_push(s, LX_OBJECT_nil());
    } else {
        lx_vm_stack_push(s, CAST_O lx_object_table_get_meta_table(tab)); // push it's meta table
    }
}
// set meta table
// luax example:
//      set_meta_table(tab, new_meta_tab);
//      set_meta_table(tab1, tab2, tab3, new_meta_tab);
void _set_meta_table(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_table* new_meta_tab = (lx_object_table*)lx_vm_stack_pop(s);
    lx_object_table* tab = (lx_object_table*)lx_vm_stack_pop(s);
    for (; tab->base.type != LX_OBJECT_TAG; tab = (lx_object_table*)lx_vm_stack_pop(s)) {
        lx_object_table_set_meta_table(tab, new_meta_tab);
    }
}
// raw get, don't use the "_get" function of tab's meta table
// table_get(tab, key)
void _table_get(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* tab = lx_vm_stack_pop(s);
    if(tab->type != LX_OBJECT_TABLE)
        assert(false && "`table_get` need a table as it's first argument");
    lx_object* key = lx_vm_stack_pop(s);
    if(lx_vm_stack_pop(s)->type != LX_OBJECT_TAG)
        assert(false && "luax function table_get needs 2 arguments"); // todo, we should throw a error
    lx_vm_stack_push(s, lx_object_table_always_found((lx_object_table*)tab, key)->value);
}
// raw set
// table_set(tab, key, value)
void _table_set(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* tab = lx_vm_stack_pop(s);
    if (tab->type != LX_OBJECT_TABLE)
        assert(false && "`table_get` need a table as it's first argument");
    lx_object* key = lx_vm_stack_pop(s);
    lx_object* new_value = lx_vm_stack_pop(s);
    if (lx_vm_stack_pop(s)->type != LX_OBJECT_TAG)
        assert(false && "luax function table_get needs 3 arguments"); // todo, we should throw a error
    lx_object_table_replace((lx_object_table*)tab, key, new_value);
}
// create a table using the provided meta table
// new_table(specify_meta_table)
// new_table() -- use the default meta table
void _new_table(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* obj = lx_vm_stack_pop(s);
    if(obj->type == LX_OBJECT_TAG)
        lx_vm_stack_push(s, CAST_O lx_create_object_table());
    else{
        lx_object* meta_table = lx_vm_stack_pop(s);
        if(meta_table->type != LX_OBJECT_TABLE)
            assert(false && "new_table needs a table"); // todo, we should throw a error
        lx_vm_stack_push(s, CAST_O lx_create_object_table_with_meta_table(CAST_T meta_table));
        if (lx_vm_stack_pop(s)->type != LX_OBJECT_TAG) {
            assert(false && "new_table can't accept more than one argument"); // todo
            _op_pop_to_tag(s);
        }
    }
}
// print(tab)
// print('this is debug info')
void _print(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* obj;
    while ((obj = lx_vm_stack_pop(s)) && obj->type != LX_OBJECT_TAG) {
        lx_dump_object(obj, stdout);
        printf("\n");
    }
    if (obj == NULL) {
        assert(false && "can't find the TAG of function arguments");
    }
}
void _dump_stack(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* obj;
    while ((obj = lx_vm_stack_pop(s)) && obj->type != LX_OBJECT_TAG)
        ;
    if (obj == NULL) {
        assert(false && "can't find the TAG of function arguments");
    }
    lx_dump_vm_stack(s);
}
void _emit_VS_breakpoint(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* obj;
    while ((obj = lx_vm_stack_pop(s)) && obj->type != LX_OBJECT_TAG)
        ;
    if (obj == NULL) {
        assert(false && "can't find the TAG of function arguments");
    }
    int a = 0; /* set this line a breakpoint in VS */
}

//
// meta functions for default table
//
static void default_meta_func__get(lx_vm_stack* s, lx_object* _called_obj)
{
    _table_get(s, _called_obj);
}
static void default_meta_func__set(lx_vm_stack* s, lx_object* _called_obj)
{
    _table_set(s, _called_obj);
}
// tab[tab]._call = function(tab, arg1, arg2) ... end; tab(arg1, arg2);
static void default_meta_func__call(lx_vm_stack* s, lx_object* _called_obj)
{
    /*
     todo
     default table can't be called. it would throw a error.
     But try-catch has not been achieved.
    */
    _op_pop_to_tag(s);
}
static void default_meta_func__delete(lx_vm_stack* s, lx_object* _called_obj)
{
    _op_pop_to_tag(s); // DO nothing
}


//
// meta functions for env table
//

// get(tab, key)
static void default_env_meta_func__get(lx_vm_stack* s, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_table* _env = CAST_T lx_vm_stack_pop(s);
    lx_object* key = lx_vm_stack_pop(s);
    if (lx_vm_stack_pop(s)->type != LX_OBJECT_TAG) {
        assert(false && "default_env_meta_func__get argument error");
    }
    _object_table_kv* kv = lx_object_table_find(_env, key);
    if (kv) { // found in the current env
        lx_vm_stack_push(s, kv->value);
        return;
    }
    // not found, search in it's _father_env
    lx_object* _father_env = lx_meta_element_get(_env, "_father_env");
    if (_father_env && _father_env->type != LX_OBJECT_NIL) {
        lx_vm_stack_push(s, LX_OBJECT_tag());
        lx_vm_stack_push(s, key);
        lx_vm_stack_push(s, _father_env);
        //_op_call(vm, NULL, lx_meta_function_get(_father_env, "_get"));
        lx_object_function* _get = lx_meta_function_get(CAST_T _father_env, "_get");
        _get->func_ptr(s, CAST_O _get);
    } else {
        lx_vm_stack_push(s, LX_OBJECT_nil());
    }
}
// set(tab, key, value)
static void default_env_meta_func__set(lx_vm_stack* s, lx_object* _called_obj)
{
    lx_object_table* _curr_env = CAST_T lx_vm_stack_pop(s);
    lx_object_table* _env = _curr_env;
    while(_env && _env->base.type != LX_OBJECT_NIL){
        if (lx_object_table_find(_env, s->arr[s->curr])) {
            lx_vm_stack_push(s, CAST_O _env);
            _table_set(s, _called_obj);
            return; // finished
        } else {
            _env = CAST_T lx_meta_element_get(_env, "_father_env");
        }
    }
    lx_vm_stack_push(s, CAST_O _curr_env);
    _table_set(s, _called_obj);
}
static void default_env_meta_func__call(lx_vm_stack* s, lx_object* _called_obj)
{
    default_meta_func__call(s, _called_obj);
}
static void default_env_meta_func__delete(lx_vm_stack* s, lx_object* _called_obj)
{
    _op_pop_to_tag(s); // DO nothing
}


lx_object* LX_OBJECT_nil()
{
    static lx_object o =
    {
        .type = LX_OBJECT_NIL
    };
    return &o;
}
lx_object* LX_OBJECT_true()
{
    static lx_object o =
    {
        .type = LX_OBJECT_BOOL,
        .fnumber = 1.0f
    };
    return &o;
}
lx_object* LX_OBJECT_false()
{
    static lx_object o =
    {
        .type = LX_OBJECT_BOOL,
        .fnumber = 0.0f
    };
    return &o;
}
lx_object* LX_OBJECT_tag()
{
    static lx_object o =
    {
        .type = LX_OBJECT_TAG,
        //.inumber = 0
    };
    return &o;
}


// run a luax function achieved in luax code
static int _vm_run_opcodes(lx_vm* vm, lx_object_function* func_obj, lx_object_table* _env)
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
        case OP_CONTINUE: {// todo
            
            continue;
        }
        case OP_CALL: {// todo
            int tagi = stack->curr;
            while (tagi >= 0 && stack->arr[tagi]->type != LX_OBJECT_TAG) {
                tagi--;
            }
            if (tagi <= 0) {
                //todo
                printf("VM ERROR: call error");
                assert(false);
            }
            while(stack->arr[tagi]->type == LX_OBJECT_TAG)
                tagi--;
            // now tagi points to the called object
            lx_object* called_obj = lx_vm_stack_remove(stack, tagi); // remove the called object
            _op_call(vm, called_obj);
            continue;
        }
        case OP_RETURN: {
            return 0; // end this luax function
        }
        case OP_FUNC_RET_VALUE_SHIFT_TO_1: {
            lx_object *value = NULL;
            for (lx_object* obj = lx_vm_stack_pop(stack); obj->type != LX_OBJECT_TAG; obj = lx_vm_stack_pop(stack)) {
                value = obj;
            }
            if(value == NULL)
                lx_vm_stack_push(stack, LX_OBJECT_nil());
            else
                lx_vm_stack_push(stack, value);
            continue;
        }
        case OP_JMP: { // todo: consider env push and pop
            int label_count = ((lx_opcode_x *)ops->arr[i])->inumber;
            int direction = label_count / abs(label_count);
            label_count = abs(label_count);
            int count = 0;
            int f = i;
            for(; count < label_count; f += direction){
                if (f < 0 || f > ops->size) {
                    assert(false && "VM ERROR: can't find the destination of jmp\n"); // todo
                }
                if(lx_opcode_is_label(ops->arr[f]->type)){
                    count++;
                    if(count == label_count)
                        break;
                }
            }
            i = f;
            continue;
        }
        case OP_JZ:{ // todo: consider env push and pop
            lx_object* condition = lx_vm_stack_pop(stack);
            if (lx_object_is_jz_zero(condition)) {
                int label_count = ((lx_opcode_x *)ops->arr[i])->inumber;
                int direction = label_count / abs(label_count);
                label_count = abs(label_count);
                int count = 0;
                int f = i;
                for (; count < label_count; f += direction) {
                    if (f < 0 || f > ops->size) {
                        assert(false && "VM ERROR: can't find the destination of jz\n"); // todo
                    }
                    if(lx_opcode_is_label(ops->arr[f]->type)){
                        count++;
                        if(count == label_count)
                            break;
                    }
                }
                i = f;
            }
            continue;
        }

        case OP_TAG:{
            lx_vm_stack_push(stack, LX_OBJECT_tag());
            continue;
        }
        case OP_POP_TO_TAG: {
            _op_pop_to_tag(stack);
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
                //lx_object_table_replace(_env, obj, LX_OBJECT_nil()); /* init to nil */
                lx_vm_stack_push(stack, LX_OBJECT_tag());
                lx_vm_stack_push(stack, LX_OBJECT_nil());
                lx_vm_stack_push(stack, obj);
                lx_vm_stack_push(stack, CAST_O _env);
                _op_call(vm, CAST_O lx_meta_function_get(_env, "_set"));
            }
            if (obj == NULL) {
                //todo
                printf("VM ERROR: can't find the tag of `local` in stack\n");
                assert(false);
            }
            continue;
        }
        case OP_LOCAL_INIT: {
            int tag_v = stack->curr;
            while (stack->arr[tag_v]->type != LX_OBJECT_TAG) {
                tag_v--;
                if (tag_v < 0) {
                    assert(false && "VM ERROR: can't find the tag of local_init in stack\n");//todo
                }
            }
            int value = tag_v - 1;
            int key = stack->curr;
            while (stack->arr[key]->type != LX_OBJECT_TAG) {
                if (stack->arr[key]->type != LX_OBJECT_STRING) {
                    //todo
                    printf("VM ERROR: `local` var should be a string (2)\n");
                    assert(false);
                }
                lx_vm_stack_push(stack, LX_OBJECT_tag()); // tag for calling _env's meta function _set
                if(stack->arr[value]->type != LX_OBJECT_TAG)
                    lx_vm_stack_push(stack, stack->arr[value--]);
                else
                    lx_vm_stack_push(stack, LX_OBJECT_nil());
                lx_vm_stack_push(stack, stack->arr[key--]);
                lx_vm_stack_push(stack, CAST_O _env);
                _op_call(vm, CAST_O lx_meta_function_get(_env, "_set"));
            }
            while(stack->arr[value]->type != LX_OBJECT_TAG)
                value--;
            stack->curr = value - 1; /* pop_to_tag */
            continue;
        }
        case OP_PUSH_ENV: {
            lx_object_table* new_env = lx_create_object_env_table_with_father_env(_env);
            _env = new_env;
            continue;
        }
        case OP_POP_ENV: {
            lx_object_table* prev_env = CAST_T lx_meta_element_get(_env, "_father_env");
            lx_meta_element_set(_env, "_father_env", LX_OBJECT_nil()); // let the GC to collect it
            _env = prev_env;
            continue;
        }

        case OP_PUSHC_NIL: {
            lx_vm_stack_push(stack, LX_OBJECT_nil());
            continue;
        }
        case OP_PUSHC_FALSE: {
            lx_vm_stack_push(stack, LX_OBJECT_false());
            continue;
        }
        case OP_PUSHC_TRUE: {
            lx_vm_stack_push(stack, LX_OBJECT_true());
            continue;
        }
        case OP_PUSHC_EMPTY_TABLE: {
            // todo: vm_debuglog("op_pushc_empty_table");
            lx_object_table* table = lx_create_object_table();
            lx_vm_stack_push(stack, CAST_O table);
            continue;
        }
        case OP_PUSHC_STR: {
            lx_opcode_x* op = (lx_opcode_x*) ops->arr[i];
            lx_vm_stack_push(stack, CAST_O lx_create_object_string_s(op->text, op->text_len));
            continue;
        }
        case OP_PUSHC_NUMBER: {
            lx_vm_stack_push(stack, lx_create_object(LX_OBJECT_NUMBER))->fnumber = ((lx_opcode_x *)(ops->arr[i]))->fnumber;
            continue;
        }
        case OP_PUSHC_TABLE: {
            //todo
            continue;
        }
        case OP_FUNC_DEF_BEGIN: {
            int func_begin = i + 1;
            int layer = 1;
            for(++i; i < ops->size; ++i){
                if (ops->arr[i]->type == OP_FUNC_DEF_END) {
                    --layer;
                    if(layer == 0)
                        break;
                }else if (ops->arr[i]->type == OP_FUNC_DEF_BEGIN) {
                    ++layer;
                }
            }
            if(i == ops->size)
                assert(false && "can't find OP_FUNC_DEF_END"); // todo
            lx_opcodes* opcodes = LX_NEW(lx_opcodes);
            opcodes->size = i - func_begin;
            opcodes->capacity = opcodes->size;
            opcodes->arr = ops->arr + func_begin;
            lx_object_function* func = lx_create_object_function_ops(opcodes, _env);
            lx_vm_stack_push(stack, CAST_O func);
            continue;
        }
        case OP_FUNC_DEF_END: {
            assert(false && "you shouldn't come here!"); // todo
            continue;
        }
        case OP_PUSHC_FUNC: {
            // Do nothing
            continue;
        }

        case OP_TABLE_GET: {
            lx_object* key = lx_vm_stack_pop(stack); // in tab[1, "key2"], `key` is 1
            lx_object* o;
            for (o = lx_vm_stack_pop(stack); o && o->type != LX_OBJECT_TAG;)
                o = lx_vm_stack_pop(stack);
            if (o == NULL) {
                assert(false && "VM Error: stack out of range");// todo
            }
            lx_object_table* tab = (lx_object_table*)lx_vm_stack_pop(stack);

            lx_vm_stack_push(stack, LX_OBJECT_tag());
            lx_vm_stack_push(stack, key);
            lx_vm_stack_push(stack, CAST_O tab);

            _op_call(vm, CAST_O lx_meta_function_get(tab, "_get"));
            continue;
        }
        case OP_TABLE_SET_TKT: {
            lx_object* key = lx_vm_stack_pop(stack);
            lx_object* o;
            for (o = lx_vm_stack_pop(stack); o && o->type != LX_OBJECT_TAG;)
                o = lx_vm_stack_pop(stack);
            if (o == NULL) {
                assert(false && "VM Error: stack out of range");// todo
            }
            lx_object_table* tab = (lx_object_table*)lx_vm_stack_pop(stack);

            //lx_vm_stack_push(stack, LX_OBJECT_tag());
            lx_vm_stack_push(stack, key);
            lx_vm_stack_push(stack, CAST_O tab);
            continue;
        }
        case OP_TABLE_GET_IMM: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object_table* tab = (lx_object_table*)lx_vm_stack_pop(stack);
            lx_vm_stack_push(stack, LX_OBJECT_tag());
            lx_vm_stack_push(stack, CAST_O lx_create_object_string_s(op->text, op->text_len));
            lx_vm_stack_push(stack, CAST_O tab);
            _op_call(vm, CAST_O lx_meta_function_get(tab, "_get"));
            continue;
        }
        case OP_TABLE_IMM_SET_TKT: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object* tab = lx_vm_stack_pop(stack);
            //lx_vm_stack_push(stack, LX_OBJECT_tag());
            lx_vm_stack_push(stack, CAST_O lx_create_object_string_s(op->text, op->text_len));
            lx_vm_stack_push(stack, tab);
            continue;
        }
        case OP_G_TABLE_GET: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_vm_stack_push(stack, LX_OBJECT_tag());
            lx_vm_stack_push(stack, CAST_O lx_create_object_string_s(op->text, op->text_len));
            lx_vm_stack_push(stack, CAST_O _env);
            _op_call(vm, CAST_O lx_meta_function_get(_env, "_get"));
            continue;
        }
        case OP_G_TABLE_SET_TKT: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            //lx_vm_stack_push(stack, LX_OBJECT_tag());
            lx_vm_stack_push(stack, CAST_O lx_create_object_string_s(op->text, op->text_len));
            lx_vm_stack_push(stack, CAST_O _env);
            continue;
        }

        case OP_ASSIGN: {
            int rvalue = stack->curr;
            int opi = rvalue;
            while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
                opi--;
            }
            int left_tkt = opi - 1;
            while (left_tkt >= 0 && stack->arr[left_tkt]->type != LX_OBJECT_TAG) {
                lx_vm_stack_push(stack, LX_OBJECT_tag()); // tag

                if(stack->arr[rvalue]->type != LX_OBJECT_TAG){ // value
                    lx_vm_stack_push(stack, stack->arr[rvalue]);
                    rvalue--;
                }else
                    lx_vm_stack_push(stack, LX_OBJECT_nil());

                lx_vm_stack_push(stack, stack->arr[left_tkt - 1]); // key
                lx_vm_stack_push(stack, stack->arr[left_tkt]); // tab
                _op_call(vm, CAST_O lx_meta_function_get(CAST_T stack->arr[left_tkt], "_set"));

                left_tkt -= 2;
            }
            stack->curr = left_tkt - 1;
            continue;
        }
        case OP_ADD_ASSIGN: { //todo
            //int rvalue_begin = stack->curr;
            //int opi = rvalue_begin;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    opi--;
            //}
            //int left_key_begin = opi - 1;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
            //        _op_add_assign((_object_table_kv *)(stack->arr[opi]), stack->arr[rvalue_begin]);
            //        rvalue_begin--;
            //    } else
            //        _op_add_assign((_object_table_kv *)(stack->arr[opi]), &LX_OBJECT_nil);

            //    opi--;
            //}
            //stack->curr = opi - 1;
            continue;
        }
        case OP_SUB_ASSIGN: { //todo
            //int rvalue_begin = stack->curr;
            //int opi = rvalue_begin;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    opi--;
            //}
            //int left_key_begin = opi - 1;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
            //        _op_sub_assign((_object_table_kv *)(stack->arr[opi]), stack->arr[rvalue_begin]);
            //        rvalue_begin--;
            //    } else
            //        _op_sub_assign((_object_table_kv *)(stack->arr[opi]), &LX_OBJECT_nil);

            //    opi--;
            //}
            //stack->curr = opi - 1;
            continue;
        }
        case OP_MUL_ASSIGN: { //todo
            //int rvalue_begin = stack->curr;
            //int opi = rvalue_begin;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    opi--;
            //}
            //int left_key_begin = opi - 1;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
            //        _op_mul_assign((_object_table_kv *)(stack->arr[opi]), stack->arr[rvalue_begin]);
            //        rvalue_begin--;
            //    } else
            //        _op_mul_assign((_object_table_kv *)(stack->arr[opi]), &LX_OBJECT_nil);

            //    opi--;
            //}
            //stack->curr = opi - 1;
            continue;
        }
        case OP_DIV_ASSIGN: { //todo
            //int rvalue_begin = stack->curr;
            //int opi = rvalue_begin;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    opi--;
            //}
            //int left_key_begin = opi - 1;
            //while (opi >= 0 && stack->arr[opi]->type != LX_OBJECT_TAG) {
            //    if (stack->arr[rvalue_begin]->type != LX_OBJECT_TAG) {
            //        _op_div_assign((_object_table_kv *)(stack->arr[opi]), stack->arr[rvalue_begin]);
            //        rvalue_begin--;
            //    } else
            //        _op_div_assign((_object_table_kv *)(stack->arr[opi]), &LX_OBJECT_nil);

            //    opi--;
            //}
            //stack->curr = opi - 1;
            continue;
        }
        case OP_AND: { //todo
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber != 0.0f && b->fnumber != 0.0f) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_OR: { //todo
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber != 0.0f || b->fnumber != 0.0f) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_NOT: {
            lx_object* a = lx_vm_stack_pop(stack);
            if (a) {
                lx_object* out = (a->fnumber == 0.0f) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_LESS: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber < b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_GREATER: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber > b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_LESS_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber <= b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_GREATER_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber >= b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_EQL_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber == b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_NOT_EQL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber != b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_ADD: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = lx_create_object(LX_OBJECT_NUMBER);
                out->fnumber = a->fnumber + b->fnumber;
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_SUB: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = lx_create_object(LX_OBJECT_NUMBER);
                out->fnumber = a->fnumber - b->fnumber;
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_MUL: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                lx_object* out = lx_create_object(LX_OBJECT_NUMBER);
                out->fnumber = a->fnumber * b->fnumber;
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
            }
            continue;
        }
        case OP_DIV: {
            lx_object* b = lx_vm_stack_pop(stack);
            lx_object* a = lx_vm_stack_pop(stack);
            if (a && b) {
                if (b->fnumber == 0.0f) {
                    // todo
                    printf("VM ERROR: div by 0 error\n");
                    assert(false);
                }
                lx_object* out = lx_create_object(LX_OBJECT_NUMBER);
                out->fnumber = a->fnumber / b->fnumber;
                lx_vm_stack_push(stack, out);
            } else {
                assert(false && "VM ERROR: no item in stack\n");// todo
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
    return 0;
}


lx_object* lx_create_object(short type)
{
    lx_object* obj = LX_NEW(lx_object);
    obj->type = type;
    return obj;
}
void lx_delete_object(lx_object* obj)
{
    lx_free(obj);
}
bool lx_object_is_jz_zero(lx_object* obj)
{
    if(obj->type == LX_OBJECT_NIL 
        || (obj->type == LX_OBJECT_BOOL && obj->fnumber == 0.0f)
        || (obj->type == LX_OBJECT_NUMBER && obj->fnumber == 0.0f)
        )
        return true;
    else
        return false;
}



lx_object_function* lx_create_object_function(lx_object_table* env_creator)
{
    lx_object_function* ret = LX_NEW(lx_object_function);
    ret->base.type = LX_OBJECT_FUNCTION;
    ret->func_opcodes = NULL;
    ret->func_ptr = NULL;
    ret->env_creator = env_creator;
    return ret;
}
lx_object_function* lx_create_object_function_p(lx_object_function_ptr_handle func_ptr, lx_object_table* env_creator)
{
    lx_object_function* ret = lx_create_object_function(env_creator);
    ret->func_ptr = func_ptr;
    ret->func_opcodes = NULL;
    return ret;
}
lx_object_function* lx_create_object_function_ops(lx_opcodes* func_opcodes, lx_object_table* env_creator)
{
    lx_object_function* ret = lx_create_object_function(env_creator);
    ret->func_opcodes = func_opcodes;
    ret->func_ptr = NULL;
    return ret;
}
void lx_delete_object_function(lx_object_function* obj_func)
{
    lx_free(obj_func);
}

lx_object_table* lx_create_object_table_raw()
{
    lx_object_table* tab = LX_NEW(lx_object_table);
    tab->base.type = LX_OBJECT_TABLE;
    tab->keyvalue_map = NULL; /* important! initialize to NULL (needed by hash lib) */
    return tab;
}
lx_object_table* lx_create_object_table()
{
    lx_object_table* tab = lx_create_object_table_raw();
    lx_object_table_set_meta_table(tab, lx_create_default_meta_table());
    return tab;
}
lx_object_table* lx_create_object_table_with_meta_table(lx_object_table* meta_table)
{
    lx_object_table* tab = lx_create_object_table_raw();
    lx_object_table_set_meta_table(tab, meta_table);
    return tab;
}
lx_object_table* lx_create_object_env_table()
{
    lx_object_table* tab = lx_create_object_table_raw();
    lx_object_table_set_meta_table(tab, lx_create_default_env_meta_table());
    return tab;
}
lx_object_table* lx_create_object_env_table_with_father_env(lx_object_table* _father_env)
{
    lx_object_table* env_table = lx_create_object_env_table();
    lx_object_table_replace_s(env_table, "_E", 2, CAST_O env_table); // store `_E` to itself

    lx_meta_element_set(env_table, "_father_env", CAST_O _father_env);
    return env_table;
}
lx_object_table* lx_create_env_table_with_inside_function()
{
    lx_object_table* env_table = lx_create_object_env_table();
    // inside functions
    lx_object_table_replace_s(env_table, "meta_table", 10, CAST_O lx_create_object_function_p(_meta_table, lx_create_object_env_table()));
    lx_object_table_replace_s(env_table, "set_meta_table", 14, CAST_O lx_create_object_function_p(_set_meta_table, lx_create_object_env_table()));
    lx_object_table_replace_s(env_table, "table_get", 9, CAST_O lx_create_object_function_p(_table_get, lx_create_object_env_table()));
    lx_object_table_replace_s(env_table, "table_set", 9, CAST_O lx_create_object_function_p(_table_set, lx_create_object_env_table()));
    lx_object_table_replace_s(env_table, "new_table", 9, CAST_O lx_create_object_function_p(_new_table, lx_create_object_env_table()));

    // template debug functions
    lx_object_table_replace_s(env_table, "print", 5, CAST_O lx_create_object_function_p(_print, lx_create_object_env_table()));
    lx_object_table_replace_s(env_table, "dump_stack", 10, CAST_O lx_create_object_function_p(_dump_stack, lx_create_object_env_table()));
    lx_object_table_replace_s(env_table, "emit_VS_breakpoint", 18, CAST_O lx_create_object_function_p(_emit_VS_breakpoint, lx_create_object_env_table()));
    
    return env_table;
}
void lx_delete_object_table(lx_object_table* tab)
{
    _object_table_kv *current, *tmp;

    HASH_ITER(hh, tab->keyvalue_map, current, tmp) {
        HASH_DEL(tab->keyvalue_map, current);  /* delete it (users advances to next) */
        //free(current);             /* free it */ // todo?
    }
    lx_free(tab);
}
lx_object_table* lx_create_default_meta_table()
{
    lx_object_table* default_meta_table = lx_create_object_table_raw();
    lx_object_table_replace_s(default_meta_table, "_get", 4, CAST_O lx_create_object_function_p(default_meta_func__get, CAST_T LX_OBJECT_nil()));
    lx_object_table_replace_s(default_meta_table, "_set", 4, CAST_O lx_create_object_function_p(default_meta_func__set, CAST_T LX_OBJECT_nil()));
    lx_object_table_replace_s(default_meta_table, "_call", 5, CAST_O lx_create_object_function_p(default_meta_func__call, CAST_T LX_OBJECT_nil()));
    lx_object_table_replace_s(default_meta_table, "_delete", 7, CAST_O lx_create_object_function_p(default_meta_func__delete, CAST_T LX_OBJECT_nil()));
    return default_meta_table;
}
lx_object_table* lx_create_default_env_meta_table()
{
    lx_object_table* default_env_meta_table = lx_create_object_table_raw();
    lx_object_table_replace_s(default_env_meta_table, "_get", 4, CAST_O lx_create_object_function_p(default_env_meta_func__get, CAST_T LX_OBJECT_nil()));
    lx_object_table_replace_s(default_env_meta_table, "_set", 4, CAST_O lx_create_object_function_p(default_env_meta_func__set, CAST_T LX_OBJECT_nil()));
    lx_object_table_replace_s(default_env_meta_table, "_call", 5, CAST_O lx_create_object_function_p(default_env_meta_func__call, CAST_T LX_OBJECT_nil()));
    lx_object_table_replace_s(default_env_meta_table, "_delete", 7, CAST_O lx_create_object_function_p(default_env_meta_func__delete, CAST_T LX_OBJECT_nil()));
    // .._find(tab, "_father_env") return NULL
    lx_object_table_replace_s(default_env_meta_table, "_father_env", 11, LX_OBJECT_nil() /* LX_OBJECT_ENV_TABLE_empty() */);
    return default_env_meta_table;
}

static char* lx_object_get_id(lx_object* obj, char id[])
{
    id[0] = (char)obj->type;
    if (obj->type == LX_OBJECT_NIL) {
        id[1] = '\0';
        return id;
    }
    if (obj->type == LX_OBJECT_NUMBER) {
        sprintf((char*)id + 1, "%f\0", obj->fnumber);
        return id;
    }
    if (obj->type == LX_OBJECT_BOOL) {
        sprintf((char*)id + 1, "%s\0", (obj->fnumber == 0.0f) ? "false" : "true");
        return id;
    }
    if (obj->type == LX_OBJECT_FUNCTION) {
        sprintf((char*)id + 1, "%p\0", obj);
        return id;
    }
    if (obj->type == LX_OBJECT_TABLE) {
        sprintf((char*)id + 1, "%p\0", obj);
        return id;
    }
    if (obj->type == LX_OBJECT_STRING) {
        memcpy((char*)id + 1, ((lx_object_string *)obj)->text, ((lx_object_string *)obj)->text_len);
        id[1 + ((lx_object_string *)obj)->text_len] = '\0';
        return id;
    }
    assert(false && "VM ERROR: ...\n"); // todo
    return "";
}

lx_object_table* lx_object_table_get_meta_table(lx_object_table* tab)
{
    return CAST_T lx_object_table_find(tab, CAST_O tab)->value;
}
void lx_object_table_set_meta_table(lx_object_table* tab, lx_object_table* new_meta_table)
{
    lx_object_table_replace(tab, CAST_O tab, CAST_O new_meta_table);
}
lx_object* lx_meta_element_get(lx_object_table* tab, const char* str)
{
    return lx_object_table_find_s(lx_object_table_get_meta_table(tab), str, strlen(str))->value;
}
void lx_meta_element_set(lx_object_table* tab, const char* str, lx_object* _element)
{
    lx_object_table_replace_s(lx_object_table_get_meta_table(tab), str, strlen(str), _element);
}
lx_object_function* lx_meta_function_get(lx_object_table* tab, const char* str)
{
    return CAST_F lx_object_table_find_s(lx_object_table_get_meta_table(tab), str, strlen(str))->value;
}
void lx_meta_function_set(lx_object_table* tab, const char* str, lx_object_function* _functor)
{
    lx_object_table_replace_s(lx_object_table_get_meta_table(tab), str, strlen(str), CAST_O _functor);
}

_object_table_kv* lx_object_table_find(lx_object_table* tab, lx_object* k)
{
    static char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
    _object_table_kv* result = NULL;
    lx_object_get_id(k, id);
    HASH_FIND_STR(tab->keyvalue_map, id, result);
    return result; // return NULL when not found
}
_object_table_kv* lx_object_table_find_s(lx_object_table* tab, const char* text, int text_len)
{
    static lx_object_string obj = {
        .base.type = LX_OBJECT_STRING,
        .text = NULL,
        .text_len = 0
    };
    obj.text = text;
    obj.text_len = text_len;
    return lx_object_table_find(tab, CAST_O &obj);
}
_object_table_kv* lx_object_table_always_found(lx_object_table* tab, lx_object* k)
{
    _object_table_kv* kv = lx_object_table_find(tab, k);
    if (kv == NULL) {
        //lx_object_table_replace(tab, k, &LX_OBJECT_nil);
        char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
        lx_object_get_id(k, id);
        int id_len = strlen(id);

        kv = LX_NEW(_object_table_kv);
        kv->key = lx_malloc(id_len + 1);
        memcpy(kv->key, id, id_len + 1);
        kv->value = LX_OBJECT_nil();

        HASH_ADD_KEYPTR(hh, tab->keyvalue_map, kv->key, id_len, kv);
    }
    return kv;
}
lx_object* lx_object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v)
{
    if(v == NULL)
        return NULL;

    if(k->type == LX_OBJECT_NIL)
        return LX_OBJECT_nil();

    lx_object* old;
    _object_table_kv* result = NULL;
    _object_table_kv* kv;
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
        old = LX_OBJECT_nil(); // no old
        kv = LX_NEW(_object_table_kv);
        kv->key = lx_malloc(id_len + 1);
    }
    memcpy(kv->key, id, id_len + 1);
    kv->value = v;

    HASH_ADD_KEYPTR( hh, tab->keyvalue_map, kv->key, id_len, kv);
    return old;
}
lx_object* lx_object_table_replace_s(lx_object_table* tab, const char* text, int text_len, lx_object* v)
{
    if(text_len == -1)
        text_len = strlen(text);
    lx_object_string key;
    key.base.type = LX_OBJECT_STRING;
    key.text = text;
    key.text_len = text_len;
    return lx_object_table_replace(tab, CAST_O &key, v);
}

lx_object_string* lx_create_object_string_s(const char * text, int text_len)
{
    lx_object_string* str = LX_NEW(lx_object_string);
    str->base.type = LX_OBJECT_STRING;
    str->base.fnumber = 0.0f;
    str->text = text;
    str->text_len = text_len;
    return str;
}
lx_object_string* lx_create_object_string(const char * str)
{
    return lx_create_object_string_s(str, strlen(str));
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
lx_object* lx_vm_stack_remove(lx_vm_stack* stack, int index)
{
    if(index < 0 || index > stack->curr)
        assert(false && "out of range"); // todo
    lx_object* removed = stack->arr[index];
    memmove(stack->arr + index, stack->arr + (index + 1), sizeof(lx_object*) * (stack->curr - index)); // todo: check this
    stack->curr--;
    return removed;
}

lx_vm* lx_create_vm ()
{
    lx_vm* vm = LX_NEW(lx_vm);
    vm->stack = lx_create_vm_stack(32);
    return vm;
}

int lx_vm_run (lx_vm* vm, lx_object_function* func_obj)
{
#if LX_VM_OPCODE_SHOW
    lx_helper_dump_opcode(func_obj->func_opcodes, stdout);
#endif

#if LX_VM_DEBUG_LOG
    printf("~~~~~~~~~~~ VM START ~~~~~~~~~~~~~\n");
#endif

    int ret = _vm_run_opcodes(vm, func_obj, lx_create_object_env_table_with_father_env(func_obj->env_creator));

#if LX_VM_DEBUG_LOG
    printf("~~~~~~~~~~~ VM END(ret:%d) ~~~~~~~~~~~~~\n", ret);
    lx_dump_vm_stack(vm->stack);
#endif

    return ret;
}

void lx_delete_vm (lx_vm* vm)
{
    lx_delete_vm_stack(vm->stack);
    lx_free(vm);
}


// helper function

const char* lx_object_to_string(lx_object* obj, char str[])
{
    switch (obj->type) {
    case LX_OBJECT_BOOL:
        sprintf(str, "%s", (obj->fnumber == 0.0f) ? "false" : "true");
        return str;
    case LX_OBJECT_NUMBER:
        sprintf(str, "%f", obj->fnumber);
        // erase the unused zero, for example: 123.000000  ->  123.0
        int i = strlen(str) - 1;
        while (i > 0 && str[i] == '0' && str[i - 1] == '0') {
            str[i] = '\0';
            --i;
        }
        return str;
    case LX_OBJECT_STRING: {
        lx_object_string* obj_s = (lx_object_string*)obj;
        memcpy(str, obj_s->text, obj_s->text_len);
        str[obj_s->text_len] = '\0';
        return str;
    }
    case LX_OBJECT_NIL:
        strcpy(str, "nil");
        return str;
    case LX_OBJECT_TABLE:
        sprintf(str, "table(%p)", obj);
        return str;
    case LX_OBJECT_FUNCTION:
        sprintf(str, "function(%p)", obj);
        return str;
    default:
        assert(false);
        return "error";
    }
}

static void lx_dump_object_table(lx_object_table* tab, FILE* fp, const char* line_before)
{
    // fprintN(fp, line_before, line_before_num);
    fprintf(fp, "table(%p):\n", tab);
    char new_line_before[1024];
    char tem[1024];
    _object_table_kv *current, *tmp;
    HASH_ITER(hh, tab->keyvalue_map, current, tmp) {
        fprintf(fp, "%s|- %s: ", line_before, current->key);
        if (current->value->type == LX_OBJECT_TABLE) {
            if (current->value == CAST_O tab) {
                fprintf(fp, "<SELF> table(%p)\n", current->value);
            } else {
                int i = strlen(current->key);
                tem[i] = '\0';
                for (--i; i >= 0; --i) {
                    tem[i] = ' ';
                }
                sprintf(new_line_before, "%s   %s  %s", line_before, tem, "    ");
                lx_dump_object_table(CAST_T current->value, fp, new_line_before);
            }
            //if (current->value == LX_OBJECT_TABLE_DEFAULT_META_TABLE()) {
            //    fprintf(fp, "<DEFAULT_META_TABLE>\n");
            //} else {
            //    int i = strlen(current->key);
            //    tem[i] = '\0';
            //    for (--i; i >= 0; --i) {
            //        tem[i] = ' ';
            //    }
            //    sprintf(new_line_before, "%s   %s  %s", line_before, tem, "    ");
            //    lx_dump_object_table(current->value, fp, new_line_before);
            //}
        } else {
            lx_dump_object(current->value, fp);
            fprintf(fp, "\n");
        }
    }
}
void lx_dump_object(lx_object* obj, FILE* fp)
{
    char tem[256];
    switch (obj->type) {
    case LX_OBJECT_BOOL:
        fprintf(fp, "%s", (obj->fnumber == 0.0f) ? "false" : "true");
        break;
    case LX_OBJECT_NUMBER:
        sprintf(tem, "%f", obj->fnumber);
        // erase the unused zero, for example: 123.000000  ->  123.0
        int i = strlen(tem) - 1;
        while (i > 0 && tem[i] == '0' && tem[i - 1] == '0') {
            tem[i] = '\0';
            --i;
        }
        fprintf(fp, "%s", tem);
        break;
    case LX_OBJECT_STRING: {
        lx_object_string* obj_s = (lx_object_string*)obj;
        memcpy(tem, obj_s->text, obj_s->text_len);
        tem[obj_s->text_len] = '\0';
        fprintf(fp, "%s", tem);
        break;
    }
    case LX_OBJECT_NIL:
        fprintf(fp, "nil");
        break;
    case LX_OBJECT_TABLE:
        lx_dump_object_table((lx_object_table*)obj, fp, "    ");
        break;
    case LX_OBJECT_FUNCTION:
        fprintf(fp, "function(%p)", obj);
        break;
    default:
        assert(false);
        return;
    }
}
void lx_object_inner_to_string(lx_object* obj, char str[])
{
    switch (obj->type) {
    case LX_OBJECT_TAG: sprintf(str, "tag"); return;
    case LX_OBJECT_TABLE:
        sprintf(str, "T{%p}", obj); return;
    case LX_OBJECT_FUNCTION:
        sprintf(str, "F{%p}", obj); return;
    case LX_OBJECT_NUMBER:
        sprintf(str, "N{%f}", obj->fnumber); return;
    case LX_OBJECT_STRING:
        strcpy(str, "S{"); lx_object_get_id(obj, str + strlen(str)); strcpy(str + strlen(str), "}"); return;
    case LX_OBJECT_BOOL:
        sprintf(str, "B{%s}", (obj->fnumber == 0.0f) ? "false" : "true"); return;
    case LX_OBJECT_NIL: sprintf(str, "nil"); return;
    default:
        assert(false);
    }
    return; //todo
}
void lx_dump_vm_stack(lx_vm_stack* s)
{
    char tem[1024 * 4]; // todo
    printf("=========== dump stack =============\n");
    printf("            ---------------\n");
    for (int i = 0; i <= s->curr; ++i) {
        sprintf(tem, "        %-4d| ", i);
        lx_object_inner_to_string(s->arr[i], tem + strlen(tem));
        printf("%s\n", tem);
    }
    if(s->curr == -1)
        printf("            | <empty>\n");
    printf("            --------------- (top)\n");
    printf("stack:%p\n", s);
    printf("=========== dump stack end ===========\n");
}
void lx_dump_vm_status(lx_vm* vm)
{
    printf("=========== dump vm status =============\n");
    printf("vm:%p\n", vm);
   // printf(""\n, )
    printf("=============== dump end ===============\n");
}