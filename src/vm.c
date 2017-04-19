#include "vm.h"
#include "base.h"
#include "mem.h"
#include "opcode.h"


static int _vm_run_opcodes(lx_vm* vm, lx_object_function* func_obj, lx_object_table* _env);

lx_object* _obj_add(lx_object* a, lx_object* b, lx_vm* vm)
{
    return NULL;
}
lx_object* _obj_sub(lx_object* a, lx_object* b, lx_vm* vm)
{
    return NULL;
}
lx_object* _obj_mul(lx_object* a, lx_object* b, lx_vm* vm)
{
    return NULL;
}
lx_object* _obj_div(lx_object* a, lx_object* b, lx_vm* vm)
{
    return NULL;
}

lx_object* _op_add(lx_object* a, lx_object* b)
{
    if (a->type == LX_OBJECT_NUMBER && b->type == LX_OBJECT_NUMBER) {
        lx_object* out = lx_create_object(LX_OBJECT_NUMBER);
        out->fnumber = a->fnumber + b->fnumber;
        return out;
    }
    // todo: add two string
    return LX_OBJECT_nil();
}

void _op_pop_to_tag(lx_vm* vm)
{
    lx_object* obj;
    for (obj = lx_object_stack_pop(vm->stack); obj && obj->type != LX_OBJECT_TAG; obj = lx_object_stack_pop(vm->stack))
        ;
    if (obj == NULL) {
        lx_throw_s(vm, "VM ERROR: can't find the tag of pop_to_tag in stack");
    }
}

int _op_call(lx_vm* vm, lx_object* obj) // obj -> called_obj(function or table)
{
    if (obj->type == LX_OBJECT_FUNCTION) {
        lx_object_function* obj_func = CAST_F obj;
        if (obj_func->func_opcodes) {
            lx_object_table* _env = CAST_T managed_with_gc(vm->gc, CAST_O lx_create_object_env_table_with_father_env(obj_func->env_creator, vm->gc));
            lx_object_stack_push(vm->call_stack, CAST_O _env);
            _vm_run_opcodes(vm, obj_func, _env);
            lx_object_stack_pop(vm->call_stack);
        } else if (obj_func->func_ptr) {
            obj_func->func_ptr(vm, obj);
        } else {
            lx_throw_s(vm, "error: lx_object_function no callable ptr");
        }
        return 0;
    }
    if (obj->type == LX_OBJECT_TABLE) {
        lx_object_stack_push(vm->stack, obj); // `_call` needs this
        return _op_call(vm, CAST_O lx_meta_function_get(CAST_T obj, "_call"));
    }

    lx_throw_s(vm, "error: _op_call: obj is not callable");
    return -1;
}


//
// inside functions achieved in C
//

void _typeof(lx_vm* vm, lx_object* _called_obj)
{
    lx_object* obj = lx_object_stack_pop(vm->stack);
    lx_object* i = obj;
    while (i->type != LX_OBJECT_TAG) {
        i = lx_object_stack_pop(vm->stack);
    }
    if (obj->type != LX_OBJECT_TAG) {
        lx_object_stack_push(vm->stack, managed_with_gc(vm->gc, CAST_O lx_create_object_string(lx_object_type_to_string(obj->type))));
    }
}

// return the meta table of tab
// luax example:
//      m = meta_table(tab);
//      m = meta_table(func_ret_mutli_value());
void _meta_table(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_table* tab = (lx_object_table*)lx_object_stack_pop(vm->stack);
    lx_object* tag = lx_object_stack_pop(vm->stack);

    for (; tag->type != LX_OBJECT_TAG; tag = lx_object_stack_pop(vm->stack)) {
        tab = (lx_object_table*)tag;
    }

    if (tab->base.type != LX_OBJECT_TABLE) {
        lx_object_stack_push(vm->stack, LX_OBJECT_nil());
    } else {
        lx_object_stack_push(vm->stack, CAST_O lx_object_table_get_meta_table(tab)); // push it's meta table
    }
}
// set meta table
// luax example:
//      set_meta_table(tab, new_meta_tab);
//      set_meta_table(tab1, tab2, tab3, new_meta_tab);
void _set_meta_table(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_table* new_meta_tab = CAST_T lx_object_stack_pop(vm->stack);
    lx_object_table* tab = CAST_T lx_object_stack_pop(vm->stack);
    for (; tab->base.type != LX_OBJECT_TAG; tab = CAST_T lx_object_stack_pop(vm->stack)) {
        lx_object_table_set_meta_table(tab, new_meta_tab);
    }
}
// raw get, don't use the "_get" function of tab's meta table
// table_get(tab, key)
void _table_get(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_stack* s = vm->stack;
    lx_object* tab = lx_object_stack_pop(s);
    if(tab->type != LX_OBJECT_TABLE)
        lx_throw_s(vm, "`table_get` need a table as it's first argument");
    lx_object* key = lx_object_stack_pop(s);
    if(lx_object_stack_pop(s)->type != LX_OBJECT_TAG)
        lx_throw_s(vm, "luax function table_get needs 2 arguments");
    lx_object_stack_push(s, lx_object_table_always_found((lx_object_table*)tab, key)->value);
}
// raw set
// table_set(tab, key, value)
void _table_set(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_stack* s = vm->stack;
    lx_object* tab = lx_object_stack_pop(s);
    if (tab->type != LX_OBJECT_TABLE)
        lx_throw_s(vm, "`table_get` need a table as it's first argument");
    lx_object* key = lx_object_stack_pop(s);
    lx_object* new_value = lx_object_stack_pop(s);
    if (lx_object_stack_pop(s)->type != LX_OBJECT_TAG)
        lx_throw_s(vm, "luax function table_get needs 3 arguments");
    lx_object_table_replace((lx_object_table*)tab, key, new_value);
}
// create a table using the provided meta table
// new_table(specify_meta_table)
// new_table() -- use the default meta table
void _new_table(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_stack* s = vm->stack;
    lx_object* obj = lx_object_stack_pop(s);
    if(obj->type == LX_OBJECT_TAG)
        lx_object_stack_push(s, managed_with_gc(vm->gc, CAST_O lx_create_object_table(vm->gc)));
    else{
        lx_object* meta_table = lx_object_stack_pop(s);
        if(meta_table->type != LX_OBJECT_TABLE)
            lx_throw_s(vm, "new_table needs a table");
        lx_object_stack_push(s, managed_with_gc(vm->gc, CAST_O lx_create_object_table_with_meta_table(CAST_T meta_table)));
        if (lx_object_stack_pop(s)->type != LX_OBJECT_TAG) {
            lx_throw_s(vm, "new_table can't accept more than one argument");
        }
    }
}
void _throw(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    int i = vm->stack->curr;
    while(vm->stack->arr[i]->type != LX_OBJECT_TAG)
        --i;
    lx_object_stack_remove(vm->stack, i); // remove arguments tag of function `throw`
    while(vm->stack->arr[i]->type != LX_OBJECT_TAG)
        --i;
    lx_object_stack_remove(vm->stack, i); // remove expr statement tag of `throw(e);`
    longjmp(*(vm->curr_jmp_buf), 1);
}
void _pcall(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* func = lx_object_stack_pop(vm->stack);
    jmp_buf _jmp;
    jmp_buf* backup_jmp_buf;
    int ret = setjmp(_jmp);
    if (ret == 0) {
        backup_jmp_buf = vm->curr_jmp_buf;
        vm->curr_jmp_buf = &_jmp;
        _op_call(vm, func);
        lx_object_stack_push(vm->stack, LX_OBJECT_nil()); // this nil means this function finished successfully
    } else {
        
    }
    vm->curr_jmp_buf = backup_jmp_buf;
}
void _collectgarbage(lx_vm* vm, lx_object* _called_obj)
{
    lx_object* opt = lx_object_stack_pop(vm->stack);
    if(opt->type == LX_OBJECT_TAG){
        lx_gc_collect(vm->gc);
        return;
    }
    lx_object* arg = lx_object_stack_pop(vm->stack);
    lx_object* o = arg;
    while(o && o->type != LX_OBJECT_NIL)
        o = lx_object_stack_pop(vm->stack);
    // we haven't achieved other functions
}
void _require(lx_vm* vm, lx_object* _called_obj)
{
    lx_object* obj = lx_object_stack_pop(vm->stack);
    lx_object* o  = obj;
    while( o && o->type != LX_OBJECT_NIL)
        o = lx_object_stack_pop(vm->stack);
    if (obj && obj->type == LX_OBJECT_STRING) {
        lx_object_string* sobj = CAST_S obj;
        char * str = (char*)lx_malloc(sobj->text_len + 1);
        memcpy(str, sobj->text, sobj->text_len);
        str[sobj->text_len] = '\0';
        FILE* fp = fopen(str, "rb");
        if (fp == NULL) {
            /* this is not a file path */
            if (strcmp(str, "math") == 0) {

            }
        } else {
            /* file path */
            fseek(fp, 0, SEEK_END);
            int filelength = ftell(fp);
            printf("filelength:%d\n", filelength);

            char* data = (char*)lx_malloc(filelength + 1);
            fseek(fp, 0, SEEK_SET);
            int ret;
            if ((ret = fread(data, 1, filelength, fp)) <= 0) {
                printf("fail: can't read file:%s\n", str);
                fclose(fp);
                return;
            }
            *(data + ret) = '\0';
            fclose(fp);
            //... todo
        }
    }
}
// print(tab)
// print('this is debug info')
void _print(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* obj;
    while ((obj = lx_object_stack_pop(vm->stack)) && obj->type != LX_OBJECT_TAG) {
        lx_dump_object(obj, stdout);
        printf("\n");
    }
    if (obj == NULL) {
        lx_throw_s(vm, "can't find the TAG of function arguments");
    }
}
void _dump_stack(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* obj;
    while ((obj = lx_object_stack_pop(vm->stack)) && obj->type != LX_OBJECT_TAG)
        ;
    if (obj == NULL) {
        lx_throw_s(vm, "can't find the TAG of function arguments");
    }
    lx_dump_object_stack(vm->stack);
}
void _emit_VS_breakpoint(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object* obj;
    while ((obj = lx_object_stack_pop(vm->stack)) && obj->type != LX_OBJECT_TAG)
        ;
    if (obj == NULL) {
        lx_throw_s(vm, "can't find the TAG of function arguments");
    }
    int a = 0; /* set this line a breakpoint in VS */
}
void _show_gc_info(lx_vm* vm, lx_object* _called_obj)
{
    _op_pop_to_tag(vm);
    lx_dump_vm_gc_status(vm);
}

//
// meta functions for default table
//
void default_meta_func__get(lx_vm* vm, lx_object* _called_obj)
{
    _table_get(vm, _called_obj);
}
void default_meta_func__set(lx_vm* vm, lx_object* _called_obj)
{
    _table_set(vm, _called_obj);
}
// tab[tab]._call = function(tab, arg1, arg2) ... end; tab(arg1, arg2);
void default_meta_func__call(lx_vm* vm, lx_object* _called_obj)
{
    lx_throw_s(vm, "default meta table's _call can't be called");
}
void default_meta_func__delete(lx_vm* vm, lx_object* _called_obj)
{
    _op_pop_to_tag(vm); // DO nothing
}


//
// meta functions for env table
//

// get(tab, key)
void default_env_meta_func__get(lx_vm* vm, lx_object* _called_obj)
{
    UNUSED_ARGUMENT(_called_obj);
    lx_object_stack* s = vm->stack;
    lx_object_table* _env = CAST_T lx_object_stack_pop(s);
    lx_object* key = lx_object_stack_pop(s);
    if (lx_object_stack_pop(s)->type != LX_OBJECT_TAG) {
        assert(false && "default_env_meta_func__get argument error");
    }
    _object_table_kv* kv = lx_object_table_find(_env, key);
    if (kv) { // found in the current env
        lx_object_stack_push(s, kv->value);
        return;
    }
    // not found, search in it's _father_env
    lx_object* _father_env = lx_meta_element_get(_env, "_father_env");
    if (_father_env && _father_env->type != LX_OBJECT_NIL) {
        lx_object_stack_push(s, LX_OBJECT_tag());
        lx_object_stack_push(s, key);
        lx_object_stack_push(s, _father_env);
        //_op_call(vm, NULL, lx_meta_function_get(_father_env, "_get"));
        lx_object_function* _get = lx_meta_function_get(CAST_T _father_env, "_get");
        _get->func_ptr(vm, CAST_O _get);
    } else {
        lx_throw_s(vm, "using undeclared variable XXX"); // todo
    }
}
// set(tab, key, value)
void default_env_meta_func__set(lx_vm* vm, lx_object* _called_obj)
{
    lx_object_stack* s = vm->stack;
    lx_object_table* _curr_env = CAST_T lx_object_stack_pop(s);
    lx_object_table* _env = _curr_env;
    while(_env && _env->base.type != LX_OBJECT_NIL){
        if (lx_object_table_find(_env, s->arr[s->curr])) {
            lx_object_stack_push(s, CAST_O _env);
            _table_set(vm, _called_obj);
            return; // finished
        } else {
            _env = CAST_T lx_meta_element_get(_env, "_father_env");
        }
    }
    lx_throw_s(vm, "using undeclared variable XXX"); // todo
}
void default_env_meta_func__call(lx_vm* vm, lx_object* _called_obj)
{
    default_meta_func__call(vm, _called_obj);
}
void default_env_meta_func__delete(lx_vm* vm, lx_object* _called_obj)
{
    _op_pop_to_tag(vm); // DO nothing
}


lx_object* LX_OBJECT_nil()
{
    static lx_object o =
    {
        .type = LX_OBJECT_NIL,
        .marked = true,
        .fnumber = 0.0f
    };
    return &o;
}
lx_object* LX_OBJECT_true()
{
    static lx_object o =
    {
        .type = LX_OBJECT_BOOL,
        .marked = true,
        .fnumber = 1.0f
    };
    return &o;
}
lx_object* LX_OBJECT_false()
{
    static lx_object o =
    {
        .type = LX_OBJECT_BOOL,
        .marked = true,
        .fnumber = 0.0f
    };
    return &o;
}
lx_object* LX_OBJECT_tag()
{
    static lx_object o =
    {
        .type = LX_OBJECT_TAG,
        .marked = true,
    };
    return &o;
}


// run a luax function achieved in luax code
static int _vm_run_opcodes(lx_vm* vm, lx_object_function* func_obj, lx_object_table* _env)
{
    if (func_obj->base.type != LX_OBJECT_FUNCTION) {
        lx_throw_s(vm, "VM ERROR: func_obj->type != LX_OBJECT_FUNCTION");
        return -1;
    }
    lx_object_stack* stack = vm->stack;
    const lx_opcodes* ops = func_obj->func_opcodes;
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
                lx_throw_s(vm, "VM ERROR: can't find a while_end or for_end to break");
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
                lx_throw_s(vm, "VM ERROR: call error");
            }
            while(stack->arr[tagi]->type == LX_OBJECT_TAG)
                tagi--;
            // now tagi points to the called object
            lx_object* called_obj = lx_object_stack_remove(stack, tagi); // remove the called object
            _op_call(vm, called_obj);
            continue;
        }
        case OP_RETURN: {
            lx_gc_collect(vm->gc);
            return 0; // end this luax function
        }
        case OP_FUNC_RET_VALUE_SHIFT_TO_1: {
            lx_object* value = lx_object_stack_pop(stack);
            lx_object* obj = value;
            for (; obj && obj->type != LX_OBJECT_TAG; obj = lx_object_stack_pop(stack))
                ;
            if(obj == NULL)
                lx_throw_s(vm, "VM ERROR: can't find the tag of OP...SHIFT_TO_1");
            lx_object_stack_push(stack, value->type == LX_OBJECT_TAG ? LX_OBJECT_nil() : value);
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
                    lx_throw_s(vm, "VM ERROR: can't find the destination of jmp");
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
            lx_object* condition = lx_object_stack_pop(stack);
            if (lx_object_is_jz_zero(condition)) {
                int label_count = ((lx_opcode_x *)ops->arr[i])->inumber;
                int direction = label_count / abs(label_count);
                label_count = abs(label_count);
                int count = 0;
                int f = i;
                for (; count < label_count; f += direction) {
                    if (f < 0 || f > ops->size) {
                        lx_throw_s(vm, "VM ERROR: can't find the destination of jz");
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
            lx_object_stack_push(stack, LX_OBJECT_tag());
            continue;
        }
        case OP_POP_TO_TAG: {
            _op_pop_to_tag(vm);
            continue;
        }

        case OP_LOCAL: {
            lx_object* obj;
            for (obj = lx_object_stack_pop(stack); obj && obj->type != LX_OBJECT_TAG; obj = lx_object_stack_pop(stack)){
                if (obj->type != LX_OBJECT_STRING) {
                    lx_throw_s(vm, "VM ERROR: `local` var should be a string");
                }
                //lx_object_table_replace(_env, obj, LX_OBJECT_nil()); /* init to nil */
                lx_object_stack_push(stack, LX_OBJECT_tag());
                lx_object_stack_push(stack, LX_OBJECT_nil());
                lx_object_stack_push(stack, obj);
                lx_object_stack_push(stack, CAST_O _env);
                _table_set(vm, NULL); // todo: optimize this, no need to push and call _table_set, just assign
            }
            if (obj == NULL) {
                lx_throw_s(vm, "VM ERROR: can't find the tag of `local` in stack");
            }
            continue;
        }
        case OP_LOCAL_INIT: {
            int tag_v = stack->curr;
            while (stack->arr[tag_v]->type != LX_OBJECT_TAG) {
                tag_v--;
                if (tag_v < 0) {
                    lx_throw_s(vm, "VM ERROR: can't find the tag of local_init in stack");
                }
            }
            int value = tag_v - 1;
            int key = stack->curr;
            while (stack->arr[key]->type != LX_OBJECT_TAG) {
                if (stack->arr[key]->type != LX_OBJECT_STRING) {
                    lx_throw_s(vm, "VM ERROR: `local` var should be a string (2)");
                }
                lx_object_stack_push(stack, LX_OBJECT_tag()); // tag for calling _env's meta function _set
                if(stack->arr[value]->type != LX_OBJECT_TAG)
                    lx_object_stack_push(stack, stack->arr[value--]);
                else
                    lx_object_stack_push(stack, LX_OBJECT_nil());
                lx_object_stack_push(stack, stack->arr[key--]);
                lx_object_stack_push(stack, CAST_O _env);
                _table_set(vm, NULL); // todo: optimize this, no need to push and call _table_set, just assign
            }
            while(stack->arr[value]->type != LX_OBJECT_TAG)
                value--;
            stack->curr = value - 1;
            continue;
        }
        case OP_PUSH_ENV: {
            _env = CAST_T managed_with_gc(vm->gc, CAST_O lx_create_object_env_table_with_father_env(_env, vm->gc));
            lx_object_stack_push(vm->call_stack, CAST_O _env);
            continue;
        }
        case OP_POP_ENV: {
            lx_object_table* prev_env = CAST_T lx_meta_element_get(_env, "_father_env");
            lx_meta_element_set(_env, "_father_env", LX_OBJECT_nil(), vm->gc); // let the GC to collect it
            _env = prev_env;
            lx_object_stack_pop(vm->call_stack);
            continue;
        }

        case OP_PUSHC_NIL: {
            lx_object_stack_push(stack, LX_OBJECT_nil());
            continue;
        }
        case OP_PUSHC_FALSE: {
            lx_object_stack_push(stack, LX_OBJECT_false());
            continue;
        }
        case OP_PUSHC_TRUE: {
            lx_object_stack_push(stack, LX_OBJECT_true());
            continue;
        }
        case OP_PUSHC_EMPTY_TABLE: {
            lx_object* table = managed_with_gc(vm->gc, CAST_O lx_create_object_table(vm->gc));
            lx_object_stack_push(stack, table);
            continue;
        }
        case OP_PUSHC_STR: {
            lx_opcode_x* op = (lx_opcode_x*) ops->arr[i];
            lx_object_stack_push(stack, managed_with_gc(vm->gc, CAST_O lx_create_object_string_s(op->text, op->text_len)));
            continue;
        }
        case OP_PUSHC_NUMBER: {
            lx_object_stack_push(stack, managed_with_gc(vm->gc, lx_create_object(LX_OBJECT_NUMBER)))->fnumber = ((lx_opcode_x *)(ops->arr[i]))->fnumber;
            continue;
        }
        case OP_PUSHC_TABLE: {
            lx_object_table* tab = CAST_T managed_with_gc(vm->gc, CAST_O lx_create_object_table(vm->gc));
            lx_object* value = lx_object_stack_pop(stack);
            while (value->type != LX_OBJECT_TAG) {
                lx_object_table_replace(tab, lx_object_stack_pop(stack), value);
                value = lx_object_stack_pop(stack);
            }
            lx_object_stack_push(stack, CAST_O tab);
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
                lx_throw_s(vm, "VM ERROR: can't find OP_FUNC_DEF_END");
            lx_opcodes* opcodes = LX_NEW(lx_opcodes);
            opcodes->size = i - func_begin;
            opcodes->capacity = opcodes->size;
            opcodes->arr = ops->arr + func_begin;
            lx_object* func = managed_with_gc(vm->gc, CAST_O lx_create_object_function_ops(opcodes, _env));
            lx_object_stack_push(stack, func);
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
            lx_object* key = lx_object_stack_pop(stack); // in tab[1, "key2"], `key` is 1
            lx_object* o = key;
            while (o && o->type != LX_OBJECT_TAG)
                o = lx_object_stack_pop(stack);
            if (o == NULL) {
                lx_throw_s(vm, "VM Error: stack out of range");
            }
            if(key->type == LX_OBJECT_TAG)
                key = LX_OBJECT_nil();
            lx_object_table* tab = (lx_object_table*)lx_object_stack_pop(stack);

            lx_object_stack_push(stack, LX_OBJECT_tag());
            lx_object_stack_push(stack, key);
            lx_object_stack_push(stack, CAST_O tab);

            _op_call(vm, CAST_O lx_meta_function_get(tab, "_get"));
            continue;
        }
        case OP_TABLE_SET_TKT: {
            lx_object* key = lx_object_stack_pop(stack);
            lx_object* o = key;
            while(o && o->type != LX_OBJECT_TAG)
                o = lx_object_stack_pop(stack);
            if (o == NULL) {
                lx_throw_s(vm, "VM Error: stack out of range");
            }
            if (key->type == LX_OBJECT_TAG)
                key = LX_OBJECT_nil();
            lx_object_table* tab = (lx_object_table*)lx_object_stack_pop(stack);

            //lx_object_stack_push(stack, LX_OBJECT_tag());
            lx_object_stack_push(stack, key);
            lx_object_stack_push(stack, CAST_O tab);
            continue;
        }
        case OP_TABLE_GET_IMM: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object_table* tab = (lx_object_table*)lx_object_stack_pop(stack);
            lx_object_stack_push(stack, LX_OBJECT_tag());
            lx_object_stack_push(stack, managed_with_gc(vm->gc, CAST_O lx_create_object_string_s(op->text, op->text_len)));
            lx_object_stack_push(stack, CAST_O tab);
            _op_call(vm, CAST_O lx_meta_function_get(tab, "_get"));
            continue;
        }
        case OP_TABLE_IMM_SET_TKT: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object* tab = lx_object_stack_pop(stack);
            lx_object_stack_push(stack, managed_with_gc(vm->gc, CAST_O lx_create_object_string_s(op->text, op->text_len)));
            lx_object_stack_push(stack, tab);
            continue;
        }
        case OP_G_TABLE_GET: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object_stack_push(stack, LX_OBJECT_tag());
            lx_object_stack_push(stack, managed_with_gc(vm->gc, CAST_O lx_create_object_string_s(op->text, op->text_len)));
            lx_object_stack_push(stack, CAST_O _env);
            _op_call(vm, CAST_O lx_meta_function_get(_env, "_get"));
            continue;
        }
        case OP_G_TABLE_SET_TKT: {
            lx_opcode_x* op = (lx_opcode_x*)ops->arr[i];
            lx_object_stack_push(stack, managed_with_gc(vm->gc, CAST_O lx_create_object_string_s(op->text, op->text_len)));
            lx_object_stack_push(stack, CAST_O _env);
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
                lx_object_stack_push(stack, LX_OBJECT_tag());

                if(stack->arr[rvalue]->type != LX_OBJECT_TAG) // value
                    lx_object_stack_push(stack, stack->arr[rvalue--]);
                else
                    lx_object_stack_push(stack, LX_OBJECT_nil());

                lx_object_stack_push(stack, stack->arr[left_tkt - 1]); // key
                lx_object_stack_push(stack, stack->arr[left_tkt]); // tab
                _op_call(vm, CAST_O lx_meta_function_get(CAST_T stack->arr[left_tkt], "_set"));

                left_tkt -= 2;
            }
            stack->curr = left_tkt - 1;
            continue;
        }
        case OP_ADD_ASSIGN: { //todo
            assert(false && " += has not been achieved");
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
            assert(false && "-= has not been achieved");
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
            assert(false && " *= has not been achieved");
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
            assert(false && "/= has not been achieved");
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
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber != 0.0f && b->fnumber != 0.0f) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_OR: { //todo
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber != 0.0f || b->fnumber != 0.0f) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_NOT: {
            lx_object* a = lx_object_stack_pop(stack);
            if (a) {
                lx_object* out = (a->fnumber == 0.0f) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_LESS: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber < b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_GREATER: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber > b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_LESS_EQL: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber <= b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_GREATER_EQL: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber >= b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_EQL_EQL: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber == b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_NOT_EQL: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = (a->fnumber != b->fnumber) ? LX_OBJECT_true() : LX_OBJECT_false();
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_ADD: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object_stack_push(stack, _op_add(a, b));
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_SUB: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = managed_with_gc(vm->gc, lx_create_object(LX_OBJECT_NUMBER));
                out->fnumber = a->fnumber - b->fnumber;
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_MUL: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                lx_object* out = managed_with_gc(vm->gc, lx_create_object(LX_OBJECT_NUMBER));
                out->fnumber = a->fnumber * b->fnumber;
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_DIV: {
            lx_object* b = lx_object_stack_pop(stack);
            lx_object* a = lx_object_stack_pop(stack);
            if (a && b) {
                if (b->fnumber == 0.0f) {
                    lx_throw_s(vm, "VM ERROR: div by 0 error\n");
                }
                lx_object* out = managed_with_gc(vm->gc, lx_create_object(LX_OBJECT_NUMBER));
                out->fnumber = a->fnumber / b->fnumber;
                lx_object_stack_push(stack, out);
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        case OP_INVERST: {
            if (0 <= stack->curr && stack->curr < stack->capacity) {
                stack->arr[stack->curr]->fnumber = - stack->arr[stack->curr]->fnumber;
            } else {
                lx_throw_s(vm, "VM ERROR: no item in stack\n");
            }
            continue;
        }
        default:
            assert(false && "VM ERROR: vm_run comes to default\n");
        }
    }
    lx_gc_collect(vm->gc);
    return 0;
}


lx_vm* lx_create_vm ()
{
    lx_vm* vm = LX_NEW(lx_vm);
    vm->curr_jmp_buf = NULL;
    vm->stack = lx_create_object_stack(32);
    vm->call_stack = lx_create_object_stack(32);
    vm->gc = lx_create_gc_info(vm->stack, vm->call_stack);
    return vm;
}

int lx_vm_run (lx_vm* vm, lx_object_function* func_obj, lx_object** exception)
{
#if LX_VM_OPCODE_SHOW
    lx_helper_dump_opcode(func_obj->func_opcodes, stdout);
#endif

#if LX_VM_DEBUG_LOG
    lx_dump_vm_gc_status(vm);
    printf("~~~~~~~~~~~ VM START ~~~~~~~~~~~~~\n");
#endif
    
    int ret = -1;
    *exception = NULL;
    jmp_buf _jmp;
    int res = setjmp(_jmp);
    if (res == 0) {
        vm->curr_jmp_buf = &_jmp;
        lx_object_table* _env = CAST_T managed_with_gc(vm->gc, CAST_O lx_create_object_env_table_with_father_env(func_obj->env_creator, vm->gc));
        lx_object_stack_push(vm->call_stack, CAST_O _env);
        lx_object_stack_push(vm->gc->always_in_mem, CAST_O func_obj);
        ret = _vm_run_opcodes(vm, func_obj, _env);
    } else {
        *exception = lx_object_stack_pop(vm->stack);
        ret = res;
    }
    lx_object_stack_pop(vm->gc->always_in_mem);
    lx_object_stack_pop(vm->call_stack);
    lx_gc_collect(vm->gc);

#if LX_VM_DEBUG_LOG
    printf("~~~~~~~~~~~ VM END(ret:%d) ~~~~~~~~~~~~~\n", ret);
    lx_dump_vm_gc_status(vm);
    lx_dump_object_stack(vm->stack);
    lx_dump_vm_status(vm);
#endif

    return ret;
}

void lx_delete_vm (lx_vm* vm)
{
    lx_delete_object_stack(vm->stack);
    lx_delete_gc_info(vm->gc);
    lx_delete_object_stack(vm->call_stack);
    lx_free(vm);
}

void lx_throw_s(lx_vm* vm, const char* str)
{
#if LX_DEBUG && LX_VM_DEBUG
    assert(false); // only useful in Visual Studio's Debug mode
#endif
    lx_object* s = managed_with_gc(vm->gc, CAST_O lx_create_object_string(str));
    lx_object_stack_push(vm->stack, LX_OBJECT_tag());
    lx_object_stack_push(vm->stack, LX_OBJECT_tag());
    lx_object_stack_push(vm->stack, s);
    _throw(vm, NULL);
}

void lx_dump_object_stack(lx_object_stack* s)
{
    if (s->curr <= -1) {
        printf("==== dump stack: %p is empty ====\n", s);
        return;
    }
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
    printf("vm->call_stack number: %d\n", vm->call_stack->curr + 1);
    printf("=============== dump end ===============\n");
}
void lx_dump_vm_gc_status(lx_vm* vm)
{
    printf("=== show gc info ===\n");
    printf("object number: %d\n", vm->gc->arr->curr + 1);
#if LX_MALLOC_STATISTICS
    printf("memory usage: %d bytes\n", lx_memory_usage);
    printf("memory max usage: %d bytes\n", lx_memory_max_usage);
#endif
    printf("=== end of gc info ===\n");
}