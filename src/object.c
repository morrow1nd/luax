#include "base.h"
#include "mem.h"
#include "object.h"


lx_object* create_object(short type)
{
    lx_object* obj = LX_NEW(lx_object);
    obj->type = type;
    obj->marked = false;
    obj->is_singleton = false;
    return obj;
}
void delete_object(lx_object* obj)
{
    lx_free(obj);
}
bool object_is_jz_zero(lx_object* obj)
{
    if (obj->type == LX_OBJECT_NIL
        || (obj->type == LX_OBJECT_BOOL && obj->fnumber == 0.0f)
        || (obj->type == LX_OBJECT_NUMBER && obj->fnumber == 0.0f)
        )
        return true;
    else
        return false;
}


lx_object_function* create_object_function_p(lx_object_function_ptr_handle func_ptr, lx_object_table* env_creator)
{
    lx_object_function* ret = LX_NEW(lx_object_function);
    ret->base.type = LX_OBJECT_FUNCTION;
    ret->base.marked = false;
    ret->base.is_singleton = false;
    ret->env_creator = env_creator;
    ret->func_ptr = func_ptr;
    ret->func_opcodes = NULL;
    return ret;
}
lx_object_function* create_object_function_ops(const lx_opcodes* func_opcodes, lx_object_table* env_creator)
{
    lx_object_function* ret = LX_NEW(lx_object_function);
    ret->base.type = LX_OBJECT_FUNCTION;
    ret->base.marked = false;
    ret->base.is_singleton = false;
    ret->env_creator = env_creator;
    ret->func_opcodes = func_opcodes;
    ret->func_ptr = NULL;
    return ret;
}
void delete_object_function(lx_object_function* obj_func)
{
    lx_free(obj_func);
}

lx_object_table* create_object_table_raw()
{
    lx_object_table* tab = LX_NEW(lx_object_table);
    tab->base.type = LX_OBJECT_TABLE;
    tab->base.marked = false;
    tab->base.fnumber = 1.0f;
    tab->base.is_singleton = false;
    tab->keyvalue_map = NULL; /* important! initialize to NULL (needed by hash lib) */
    return tab;
}

void delete_object_table(lx_object_table* tab)
{
    _object_table_kv *current, *tmp;

    HASH_ITER(hh, tab->keyvalue_map, current, tmp) {
#if LX_DEBUG
        current->key = NULL;
        current->value = NULL;
#endif
        HASH_DEL(tab->keyvalue_map, current);  /* delete it (users advances to next) */
        lx_free(current->hash_key);
        lx_free(current);
    }
    lx_free(tab);
}


static char* object_get_id(lx_object* obj, char id[])
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
    assert(false && "VM ERROR: ...\n");
    return "";
}
_object_table_kv* object_table_find(lx_object_table* tab, lx_object* k)
{
    static char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
    _object_table_kv* result = NULL;
    object_get_id(k, id);
    HASH_FIND_STR(tab->keyvalue_map, id, result);
    return result; /* return NULL when not found */
}
_object_table_kv* object_table_find_t(lx_object_table* tab, const char* text, int text_len)
{
    static lx_object_string obj = {
        .base.type = LX_OBJECT_STRING,
        .text = NULL,
        .text_len = 0
    };
    obj.text = text;
    obj.text_len = text_len;
    return object_table_find(tab, CAST_O &obj);
}
_object_table_kv* object_table_always_found(lx_object_table* tab, lx_object* k)
{
    _object_table_kv* kv = object_table_find(tab, k);
    if (kv == NULL) {
        char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
        object_get_id(k, id);
        int id_len = strlen(id);

        kv = LX_NEW(_object_table_kv);
        kv->hash_key = lx_malloc(id_len + 1);
        memcpy(kv->hash_key, id, id_len + 1);
        kv->key = k;
        kv->value = LX_OBJECT_nil();

        HASH_ADD_KEYPTR(hh, tab->keyvalue_map, kv->hash_key, id_len, kv);
    }
    return kv;
}
lx_object* object_table_replace(lx_object_table* tab, lx_object* k, lx_object* v)
{
    if (v == NULL)
        return NULL;

    lx_object* old;
    _object_table_kv* result = NULL;
    _object_table_kv* kv;
    char id[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
    object_get_id(k, id);
    size_t id_len = strlen(id);
    HASH_FIND_STR(tab->keyvalue_map, id, result);
    if (result) {
        HASH_DEL(tab->keyvalue_map, result);
        if (id_len > strlen(result->hash_key)) {
            lx_free(result->hash_key);
            result->hash_key = lx_malloc(id_len + 1);
        }
        kv = result;
        old = result->value;
    } else {
        old = LX_OBJECT_nil(); /* no old */
        kv = LX_NEW(_object_table_kv);
        kv->hash_key = lx_malloc(id_len + 1);
    }
    memcpy(kv->hash_key, id, id_len + 1);
    kv->value = v;
    kv->key = k;

    HASH_ADD_KEYPTR(hh, tab->keyvalue_map, kv->hash_key, id_len, kv);
    return old;
}

lx_object_table* table_get_meta_table(lx_object_table* tab)
{
    return CAST_T object_table_find(tab, CAST_O tab)->value;
}
void table_set_meta_table(lx_object_table* tab, lx_object_table* new_meta_table)
{
    object_table_replace(tab, CAST_O tab, CAST_O new_meta_table);
}
lx_object* table_meta_element_get(lx_object_table* tab, const char* str)
{
    return object_table_find_t(table_get_meta_table(tab), str, strlen(str))->value;
}
void table_meta_element_set(lx_object_table* tab, lx_object* k, lx_object* v)
{
    object_table_replace(table_get_meta_table(tab), k, v);
}
lx_object_function* table_meta_function_get(lx_object_table* tab, const char* str)
{
    return CAST_F object_table_find_t(table_get_meta_table(tab), str, strlen(str))->value;
}
void table_meta_function_set(lx_object_table* tab, lx_object* k, lx_object_function* v)
{
    object_table_replace(table_get_meta_table(tab), k, CAST_O v);
}


lx_object_string* create_object_string_t(const char * text, int text_len)
{
    lx_object_string* str = LX_NEW(lx_object_string);
    str->base.type = LX_OBJECT_STRING;
    str->base.marked = false;
    str->base.fnumber = 0.0f;
    str->base.is_singleton = false;
    str->need_free = false;
    str->text = text;
    str->text_len = text_len;
    return str;
}
lx_object_string* create_object_string_s(const char * _str)
{
    int text_len = strlen(_str);
    char * text = (char*)lx_malloc(text_len + 1);
    memcpy(text, _str, text_len);

    lx_object_string* str = LX_NEW(lx_object_string);
    str->base.type = LX_OBJECT_STRING;
    str->base.marked = false;
    str->base.fnumber = 0.0f;
    str->base.is_singleton = false;
    str->need_free = true;
    str->text = (const char*)text;
    str->text_len = text_len;
    return str;
}
void delete_object_string(lx_object_string* obj)
{
    if(obj->need_free)
        lx_free((char*)(obj->text));
    lx_free(obj);
}


lx_object_stack* lx_create_object_stack(int capacity_enlarge_grain)
{
    lx_object_stack* s = LX_NEW(lx_object_stack);
    s->capacity_enlarge_grain = capacity_enlarge_grain;
    s->capacity = capacity_enlarge_grain;
    s->curr = -1;
    s->arr = (lx_object**)lx_malloc(sizeof(lx_object*) * s->capacity_enlarge_grain);
    return s;
}
void lx_delete_object_stack(lx_object_stack* s)
{
    lx_free(s->arr);
    lx_free(s);
}
lx_object* lx_object_stack_push(lx_object_stack* s, lx_object* obj)
{
    if (s->curr + 1 == s->capacity) {
        /* enarge it */
        lx_object** objs = (lx_object**)lx_malloc(sizeof(lx_object*) * (s->capacity + s->capacity_enlarge_grain));
        memcpy(objs, s->arr, s->capacity * sizeof(lx_object*));
        s->capacity += s->capacity_enlarge_grain;
        lx_free(s->arr);
        s->arr = objs;
    }
    s->curr++;
    s->arr[s->curr] = obj;
    return obj;
}
lx_object* lx_object_stack_pop(lx_object_stack* s)
{
    if (s->curr < 0 || s->curr >= s->capacity)
        return NULL;
    s->curr--;
#if (defined _DEBUG) || (defined DEBUG) 
    lx_object* ret = s->arr[s->curr + 1];
    s->arr[s->curr + 1] = NULL; // clean it
    return ret;
#endif
    return s->arr[s->curr + 1];
}
lx_object* lx_object_stack_remove(lx_object_stack* stack, int index)
{
    if (index < 0 || index > stack->curr)
        assert(false && "out of range");
    lx_object* removed = stack->arr[index];
    memmove(stack->arr + index, stack->arr + (index + 1), sizeof(lx_object*) * (stack->curr - index));
    stack->curr--;
    return removed;
}
lx_object* lx_object_stack_top(lx_object_stack* stack)
{
    if (stack->curr < 0 || stack->curr >= stack->capacity)
        return NULL;
    return stack->arr[stack->curr];
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
    fprintf(fp, "table(%p):\n", tab);
    char new_line_before[1024];
    char tem[1024];
    _object_table_kv *current, *tmp;
    HASH_ITER(hh, tab->keyvalue_map, current, tmp) {
        fprintf(fp, "%s|- %s: ", line_before, current->hash_key);
        if (current->value->type == LX_OBJECT_TABLE) {
            if (current->value == CAST_O tab) {
                fprintf(fp, "<SELF> table(%p)\n", current->value);
            } else {
                int i = strlen(current->hash_key);
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
    case LX_OBJECT_NIL:
        fprintf(fp, "nil");
        break;
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
    case LX_OBJECT_TABLE:
        lx_dump_object_table((lx_object_table*)obj, fp, "    ");
        break;
    case LX_OBJECT_FUNCTION:
        fprintf(fp, "function(%p)", obj);
        break;
    default:
        assert(false);
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
        strcpy(str, "S{"); object_get_id(obj, str + strlen(str)); strcpy(str + strlen(str), "}"); return;
    case LX_OBJECT_BOOL:
        sprintf(str, "B{%s}", (obj->fnumber == 0.0f) ? "false" : "true"); return;
    case LX_OBJECT_NIL: sprintf(str, "nil"); return;
    default:
        assert(false);
    }
}

const char* lx_object_type_to_string(int type)
{
    switch (type) {
    case LX_OBJECT_NIL:
        return "nil";
    case LX_OBJECT_BOOL:
        return "bool";
    case LX_OBJECT_NUMBER:
        return "number";
    case LX_OBJECT_STRING:
        return "string";
    case LX_OBJECT_TABLE:
        return "table";
    case LX_OBJECT_TAG:
        return "tag";
    default:
        assert(false);
        return NULL;
    }
}

void delete_object_by_type(lx_object* obj)
{
    switch (obj->type) {
    case LX_OBJECT_NUMBER:
        delete_object(obj);
        break;
    case LX_OBJECT_STRING:
        delete_object_string(CAST_S obj);
        break;
    case LX_OBJECT_FUNCTION:
        delete_object_function(CAST_F obj);
        break;
    case LX_OBJECT_TABLE:
        delete_object_table(CAST_T obj);
        break;
    case LX_OBJECT_NIL:
    case LX_OBJECT_BOOL:
    case LX_OBJECT_TAG:
    default:
        assert(false);
        break;
    }
}
