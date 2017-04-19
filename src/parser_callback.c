#include "./parser_callback.h"
#include "./base.h"
#include "./mem.h"
#include "./opcode.h"


#if(!LX_PARSER_DEBUG_LOG)
# define debug(msg)
# define debuglog(msg)
# define debuglog_l(l, msg)
# define debuglog_luax_str(l, msg)
#endif

void lx_syntax_node_init(lx_syntax_node* n)
{
    n->token = NULL;
    n->next = NULL;
    n->tag = -1;
    n->opcodes = NULL;
}


/* just a wrapper for link to a list */
struct opcode_w {
    lx_opcode* real_opcode;
    struct opcode_w* next;
};

struct opcode_list {
    struct opcode_w* back;
    struct opcode_w* front;
    int label_size;
};

static struct opcode_list* __new_opcodes()
{
    struct opcode_list* l = LX_NEW(struct opcode_list);
    l->back = NULL;
    l->front = NULL;
    l->label_size = 0;
    return l;
}
static struct opcode_w* __wrapper_lx_opcode(lx_opcode* op)
{
    struct opcode_w* w = LX_NEW(struct opcode_w);
    w->next = NULL;
    w->real_opcode = op;
    return w;
}
static struct opcode_w* __new_op(enum LX_OPCODE_TYPE type)
{
    lx_opcode* op = LX_NEW(lx_opcode);
    op->type = (unsigned char)type;
    op->extra_info = -1;
    return __wrapper_lx_opcode(op);
}
static struct opcode_w* __new_op_x(enum LX_OPCODE_TYPE type, lx_syntax_node* node)
{
    lx_opcode_x* op_x = LX_NEW(lx_opcode_x);
    op_x->text = node->token->text;
    op_x->text_len = node->token->text_len;
    op_x->_.type = (unsigned char)type;
    op_x->_.extra_info = -1;
    return __wrapper_lx_opcode((lx_opcode*)op_x);
}
static struct opcode_w* __new_op_i(enum LX_OPCODE_TYPE type, int i)
{
    lx_opcode_x* op_x = LX_NEW(lx_opcode_x);
    op_x->inumber = i;
    op_x->_.type = (unsigned char)type;
    op_x->_.extra_info = -1;
    return __wrapper_lx_opcode((lx_opcode*)op_x);
}
static struct opcode_w* __new_op_f(enum LX_OPCODE_TYPE type, float f)
{
    lx_opcode_x* op_x = LX_NEW(lx_opcode_x);
    op_x->fnumber = f;
    op_x->_.type = (unsigned char)type;
    op_x->_.extra_info = -1;
    return __wrapper_lx_opcode((lx_opcode*)op_x);
}


/* opcode helper function */
static void move(lx_syntax_node* _self, lx_syntax_node* _1)
{
    if(_1->opcodes == NULL)
        return;
    if (_self->opcodes == NULL) {
        _self->opcodes = __new_opcodes();
    }
    _self->opcodes->label_size += _1->opcodes->label_size;
    if (_self->opcodes->back != NULL) {
        _self->opcodes->back->next = _1->opcodes->front;
    }
    _self->opcodes->back = _1->opcodes->back;
    if(_self->opcodes->front == NULL)
        _self->opcodes->front = _1->opcodes->front;
}
static void move2(lx_syntax_node* _self, lx_syntax_node* _1, lx_syntax_node* _2)
{
    move(_self, _1);
    move(_self, _2);
}
static void move3(lx_syntax_node* _self, lx_syntax_node* _1, lx_syntax_node* _2, lx_syntax_node* _3)
{
    move(_self, _1);
    move(_self, _2);
    move(_self, _3);
}
static void append(lx_syntax_node* _self, struct opcode_w* op)
{
    if(_self->opcodes == NULL)
        _self->opcodes = __new_opcodes();
    if(lx_opcode_is_label(op->real_opcode->type))
        _self->opcodes->label_size ++;
    if (_self->opcodes->back == NULL) {
        _self->opcodes->back = op;
        _self->opcodes->front = _self->opcodes->back;
    } else {
        _self->opcodes->back->next = op;
        _self->opcodes->back = _self->opcodes->back->next; // todo <=?=> _self->opcodes->back = op;
    }
}
static void append_with_opinfo(lx_syntax_node* _self, struct opcode_w* op, int op_extra_info)
{
    op->real_opcode->extra_info = op_extra_info;
    append(_self, op);
}

lx_opcodes* genBytecode(lx_syntax_node* root)
{
    lx_opcodes* opcodes = LX_NEW(lx_opcodes);
    opcodes->capacity = 0;
    opcodes->size = 0;
    opcodes->arr = NULL;
    if(root->opcodes == NULL)
        return opcodes;
    for (struct opcode_w* n = root->opcodes->front; n != NULL; n = n->next){
        if (opcodes->size == opcodes->capacity) {
            // enarge it
            lx_opcode** arr = (lx_opcode**)lx_malloc(sizeof(lx_opcode*) * ((opcodes->capacity / 1024 + 1) * 1024));
            if (opcodes->arr != NULL) {
                memcpy(arr, opcodes->arr, opcodes->capacity);
                lx_free(opcodes->arr);
            }
            opcodes->arr = arr;
        }
        opcodes->arr[opcodes->size] = n->real_opcode;
        opcodes->size++;
    }
    return opcodes;
}

//
// Callback function
//

LX_CALLBACK_DECLARE1(compile_unit, stmt_sequence)
{
    debuglog("compile_unit  ->  stmt_sequence");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(stmt_sequence, stmt, stmt_sequence)
{
    debuglog("stmt_sequence  ->  stmt stmt_sequence");
    move2(_self, _1, _2);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt_sequence, stmt)
{
    debuglog("stmt_sequence  ->  stmt");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt_sequence, EMPTY_SYMBOL) // use EMPTY_SYMBOL to replace symbol:ε
{
    debuglog("stmt_sequence  ->  EMPTY_SYMBOL");

    // FREE_SYNTAX_NODE(_1); // empty_symbol is NULL
}

LX_CALLBACK_DECLARE1(stmt, EOS) /* EOS: end of statement */
{
    debuglog("stmt  ->  EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== empty statement");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, if_stmt)
{
    debuglog("stmt  ->  if_stmt");
    debuglog("================== new statement ================== if");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, while_stmt)
{
    debuglog("stmt  ->  while_stmt");
    debuglog("================== new statement ================== while");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, for_stmt)
{
    debuglog("stmt  ->  for_stmt");
    debuglog("================== new statement ================== for");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, expr_stmt)
{
    debuglog("stmt  ->  expr_stmt");
    debuglog("================== new statement ================== expr_stmt");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(stmt, BREAK, EOS)
{
    debuglog("stmt  ->  BREAK EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== break;");
    append(_self, __new_op(OP_BREAK));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(stmt, CONTINUE, EOS)
{
    debuglog("stmt  ->  CONTINUE EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== continue;");
    append(_self, __new_op(OP_CONTINUE));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(stmt, RETURN, EOS)
{
    debuglog("stmt  ->  RETURN EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== return;");
    // append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_return_stmt); // return element doesn't need a tag
    append(_self, __new_op(OP_RETURN));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(stmt, RETURN, expr_list, EOS)
{
    debuglog("stmt  ->  RETURN expr_list EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== return expr_list;");
    // append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_return_stmt); // return element doesn't need a tag
    move(_self, _2);
    append(_self, __new_op(OP_RETURN));
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(stmt, LOCAL, identifier_list, EOS)
{
    debuglog("stmt  ->  LOCAL identifier_list EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== local identifier_list;");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_local_declare);
    for (lx_syntax_node* n = _2->next, *tem; n != NULL;) {
        append(_self, __new_op_x(OP_PUSHC_STR, n));
        tem = n->next;
        FREE_SYNTAX_NODE(n);
        n = tem;
    }
    append(_self, __new_op(OP_LOCAL));
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE5(stmt, LOCAL, identifier_list, EQL, expr_list, EOS)
{
    debuglog("stmt  ->  LOCAL identifier_list EQL expr_list EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== local identifier_list = expr_list;");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_local_declare_with_init);
    move(_self, _4);
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_local_declare);
    for (lx_syntax_node* n = _2->next, *tem; n != NULL;){
        append(_self, __new_op_x(OP_PUSHC_STR, n));
        tem = n->next;
        FREE_SYNTAX_NODE(n);
        n = tem;
    }
    append(_self, __new_op(OP_LOCAL_INIT));
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE5(if_stmt, IF, expr, THEN, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "if_stmt  ->  IF expr THEN stmt_sequence END");
    int stmt_sequence_label_size = _4->opcodes ? _4->opcodes->label_size : 0;

    move(_self, _2);
    append(_self, __new_op_i(OP_JZ, stmt_sequence_label_size + 1));
    append(_self, __new_op(OP_PUSH_ENV));
    move(_self, _4);
    append(_self, __new_op(OP_POP_ENV));
    append(_self, __new_op(OP_LABEL));
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE7(if_stmt, IF, expr, THEN, stmt_sequence, ELSE, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "if_stmt  ->  IF expr THEN stmt_sequence ELSE stmt_sequence END");
    int stmt_sequence_label_size = _4->opcodes ? _4->opcodes->label_size : 0;
    int else_stmt_sequence_label_size = _6->opcodes ? _6->opcodes->label_size : 0;

    move(_self, _2);
    append(_self, __new_op_i(OP_JZ, stmt_sequence_label_size + 1));
    append(_self, __new_op(OP_PUSH_ENV));
    move(_self, _4);
    append(_self, __new_op(OP_POP_ENV));
    append(_self, __new_op_i(OP_JMP, else_stmt_sequence_label_size + 2));
    append(_self, __new_op(OP_LABEL));
    append(_self, __new_op(OP_PUSH_ENV));
    move(_self, _6);
    append(_self, __new_op(OP_POP_ENV));
    append(_self, __new_op(OP_LABEL));
    FREE_SYNTAX_NODE(_7);
    FREE_SYNTAX_NODE(_6);
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE5(while_stmt, WHILE, expr, THEN, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "while_stmt  ->  WHILE expr THEN stmt_sequence END");
    int stmt_sequence_label_size = _4->opcodes ? _4->opcodes->label_size : 0;

    append(_self, __new_op(OP_LABEL_WHILE_BEGIN));
    move(_self, _2);
    append(_self, __new_op_i(OP_JZ, stmt_sequence_label_size + 1));
    append(_self, __new_op(OP_PUSH_ENV));
    move(_self, _4);
    append(_self, __new_op(OP_POP_ENV));
    append(_self, __new_op_i(OP_JMP, -(stmt_sequence_label_size + 1)));
    append(_self, __new_op(OP_LABEL_WHILE_END));
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE9(for_stmt, FOR, expr, EOS, expr, EOS, expr, THEN, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "for_stmt  ->  FOR expr EOS expr EOS expr THEN stmt_sequence END");
    int stmt_sequence_label_size = _8->opcodes ? _8->opcodes->label_size : 0;

    move(_self, _2);
    append(_self, __new_op(OP_LABEL_FOR_BODY));
    move(_self, _4);
    append(_self, __new_op_i(OP_JZ, stmt_sequence_label_size + 2));
    append(_self, __new_op(OP_PUSH_ENV));
    move(_self, _8);
    append(_self, __new_op(OP_POP_ENV));
    append(_self, __new_op(OP_LABEL_FOR_BEGIN));
    move(_self, _6);
    append(_self, __new_op_i(OP_JMP, -(stmt_sequence_label_size + 2)));
    append(_self, __new_op(OP_LABEL_FOR_END));
    FREE_SYNTAX_NODE(_9);
    FREE_SYNTAX_NODE(_8);
    FREE_SYNTAX_NODE(_7);
    FREE_SYNTAX_NODE(_6);
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(expr_stmt, expr, EOS)
{
    debuglog_l(_2->token->linenum, "expr_stmt  ->  expr EOS");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_expr_stmt);
    move(_self, _1);
    append(_self, __new_op(OP_POP_TO_TAG));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE3(expr_list, expr, COMMA, expr_list)
{
    debuglog("expr_list  ->  expr COMMA expr_list");
    // move2(_self, _1, _3);
    move2(_self, _3, _1);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(expr_list, expr)
{
    debuglog("expr_list  ->  expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
// change `table_get` to `table_set_tkt` 
static void _table_set_prepare(par expr)
{
    lx_opcode* table_get_op = NULL;
    // find the last `table_get`-like opcode
    for (struct opcode_w* opw = expr->opcodes->front; opw != NULL; opw = opw->next) {
        if(lx_opcode_is_table_get(opw->real_opcode->type))
            table_get_op = opw->real_opcode;
    }
    if (table_get_op == NULL) {
        // !!!not found!!!
        // these codes cound generate `table_get`-like opcode:
        //      a = 1;
        //      tab[1] = 2;
        //      func()["key"] = 1, 2;
        //      tab.name = tab2.name;
        // and tab2.name would generate `table_get` too, but it wouldn't come to here!
        assert(false && "in prefix_expr, you must find a `table_get`-like opcode. we don't allow this code: `func() = 1`");
        return;
    }
    switch (table_get_op->type) { // todo: maybe we cound use: table_get_op->type ++; // this need the enum LX_OPCODE_TYPE to be sorted well
    case OP_TABLE_GET:
        table_get_op->type = OP_TABLE_SET_TKT;
        break;
    case OP_TABLE_GET_IMM:
        table_get_op->type = OP_TABLE_IMM_SET_TKT;
        break;
    case OP_G_TABLE_GET:
        table_get_op->type = OP_G_TABLE_SET_TKT;
        break;
    default:
        assert(false && "no way to come to here");
    }
}
//LX_CALLBACK_DECLARE3(prefix_expr_list, prefix_expr, COMMA, prefix_expr_list)
//{
//    debuglog("prefix_expr_list  ->  prefix_expr COMMA prefix_expr_list");
//    // move2(_self, _1, _3);
//    _table_set_prepare(_1);
//    move2(_self, _3, _1);
//    FREE_SYNTAX_NODE(_3);
//    FREE_SYNTAX_NODE(_2);
//    FREE_SYNTAX_NODE(_1);
//}
//LX_CALLBACK_DECLARE1(prefix_expr_list, prefix_expr)
//{
//    debuglog("prefix_expr_list  ->  prefix_expr");
//    _table_set_prepare(_1);
//    move(_self, _1);
//    FREE_SYNTAX_NODE(_1);
//}
LX_CALLBACK_DECLARE3(prefix_expr_list, prefix_expr, COMMA, prefix_expr_list)
{
    debuglog("prefix_expr_list  ->  prefix_expr COMMA prefix_expr_list");
    _self->next = _1;
    _1->next = _3->next;
    FREE_SYNTAX_NODE(_3);
    // FREE_SYNTAX_NODE(_2); // _2 is a NULL, no need to free it.
}
LX_CALLBACK_DECLARE1(prefix_expr_list, prefix_expr)
{
    debuglog("prefix_expr_list  ->  prefix_expr");
    _self->next = _1;
}


LX_CALLBACK_DECLARE1(expr, assign_expr)
{
    debuglog("expr  ->  assign_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE3(assign_expr, prefix_expr_list, assign_op, expr_list)
{
    debuglog("assign_expr  ->  prefix_expr_list assign_op expr_list");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_assign_stmt_lvalue);
    par _prev = NULL;
    par _curr = _1->next;
    par _next;
    while (_curr != NULL) {
        _next = _curr->next;

        _curr->next = _prev;

        _prev = _curr;
        _curr = _next;
    }
    par _prefix_expr = _prev;
    while(_prefix_expr != NULL) {
        _table_set_prepare(_prefix_expr);
        move(_self, _prefix_expr);

        par next = _prefix_expr->next;
        FREE_SYNTAX_NODE(_prefix_expr);
        _prefix_expr = next;
    }
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_assign_stmt_rvalue);
    move2(_self, _3, _2);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_expr, logical_expr)
{
    debuglog("assign_expr  ->  logical_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(logical_expr, NOT, compare_expr)
{
    debuglog("logical_expr  ->  NOT compare_expr");
    move(_self, _2);
    append(_self, __new_op(OP_NOT));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(logical_expr, compare_expr, logical_op, logical_expr)
{
    debuglog("logical_expr  ->  compare_expr logical_op logical_expr");
    move3(_self, _1, _3, _2);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(logical_expr, compare_expr)
{
    debuglog("logical_expr  ->  compare_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(compare_expr, addtive_expr, compare_op, compare_expr)
{
    debuglog("compare_expr  ->  addtive_expr compare_op compare_expr");
    move3(_self, _1, _3, _2);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_expr, addtive_expr)
{
    debuglog("compare_expr  ->  addtive_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(addtive_expr, multiply_expr, addtive_op, addtive_expr)
{
    debuglog("addtive_expr  ->  multiply_expr addtive_op addtive_expr");
    move3(_self, _1, _3, _2);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(addtive_expr, multiply_expr)
{
    debuglog("addtive_expr  ->  multiply_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(multiply_expr, prefix_expr, multiply_op, multiply_expr)
{
    debuglog("multiply_expr  ->  prefix_expr multiply_op multiply_expr");
    move3(_self, _1, _3, _2);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(multiply_expr, prefix_expr)
{
    debuglog("multiply_expr  ->  prefix_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(prefix_expr, prefix_op, suffix_expr)
{
    debuglog("prefix_expr  ->  prefix_op suffix_expr");
    move2(_self, _2, _1);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(prefix_expr, suffix_expr)
{
    debuglog("prefix_expr  ->  suffix_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(suffix_expr, single_expr, suffix_op)
{
    debuglog("suffix_expr  ->  single_expr suffix_op");
    move2(_self, _1, _2);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(suffix_expr, single_expr)
{
    debuglog("suffix_expr  ->  single_expr");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(single_expr, SL, expr, SR)
{
    debuglog("single_expr  ->  SL expr SR");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_return_values_shift_to_1);
    move(_self, _2);
    append(_self, __new_op(OP_FUNC_RET_VALUE_SHIFT_TO_1)); /* actually, this is not a function. Example: `a, b = (func()), 2;`, () makes sure b is assigned to 2 */
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, NIL)
{
    debuglog("single_expr  ->  NIL");
    append(_self, __new_op(OP_PUSHC_NIL));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, FALSE)
{
    debuglog("single_expr  ->  FALSE");
    append(_self, __new_op(OP_PUSHC_FALSE));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, TRUE)
{
    debuglog("single_expr  ->  TRUE");
    append(_self, __new_op(OP_PUSHC_TRUE));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, immediate)
{
    debuglog("single_expr  ->  immediate");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, IDENTIFIER)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("single_expr  ->  IDENTIFIER");
    append(_self, __new_op_x(OP_G_TABLE_GET, _1));
    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE3(suffix_op, SL, SR, suffix_op)
{
    debuglog("suffix_op  ->  SL SR suffix_op");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_return_values_shift_to_1);
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_call_argc_empty);
    append(_self, __new_op(OP_CALL));
    append(_self, __new_op(OP_FUNC_RET_VALUE_SHIFT_TO_1));
    move(_self, _3);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(suffix_op, SL, SR)
{
    debuglog("suffix_op  ->  SL SR");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_call_argc_empty);
    append(_self, __new_op(OP_CALL));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE4(suffix_op, SL, expr_list, SR, suffix_op)
{
    debuglog("suffix_op  ->  SL expr_list SR suffix_op");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_return_values_shift_to_1);
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_call_argc);
    move(_self, _2);
    append(_self, __new_op(OP_CALL));
    append(_self, __new_op(OP_FUNC_RET_VALUE_SHIFT_TO_1));
    move(_self, _4);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(suffix_op, SL, expr_list, SR)
{
    debuglog("suffix_op  ->  SL expr_list SR");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_call_argc);
    move(_self, _2);
    append(_self, __new_op(OP_CALL));
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE4(suffix_op, ML, expr, MR, suffix_op)
{
    debuglog("suffix_op  ->  ML expr MR suffix_op");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_table_index_ML_expr_MR);
    move(_self, _2);
    append(_self, __new_op(OP_TABLE_GET));
    move(_self, _4);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(suffix_op, ML, expr, MR)
{
    debuglog("suffix_op  ->  ML expr MR");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_table_index_ML_expr_MR);
    move(_self, _2);
    append(_self, __new_op(OP_TABLE_GET));
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(suffix_op, DOT, IDENTIFIER, suffix_op)
{
    debuglog("suffix_op  ->  DOT IDENTIFIER suffix_op");
    append(_self, __new_op_x(OP_TABLE_GET_IMM, _2));
    move(_self, _3);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(suffix_op, DOT, IDENTIFIER)
{
    debuglog("suffix_op  ->  DOT IDENTIFIER");
    append(_self, __new_op_x(OP_TABLE_GET_IMM, _2));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE1(immediate, object_immediate)
{
    debuglog("immediate  ->  object_immediate");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(immediate, function_define)
{
    debuglog("immediate  ->  function_define");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(immediate, STRING_IMMEDIATE)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("immediate  ->  STRING_IMMEDIATE");
    append(_self, __new_op_x(OP_PUSHC_STR, _1));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(immediate, NUMBER_IMMEDIATE)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("immediate  ->  NUMBER_IMMEDIATE");
    float f = 0.0f;
    {
        const lx_token* t = _1->token;
        char backup = *(t->text + t->text_len);
        *(t->text + t->text_len) = '\0';
        char *end;
        f = strtof(t->text, &end);
        if (end == t->text) {
            // todo
            assert(false);
        }
        *(t->text + t->text_len) = backup;
    }
    append(_self, __new_op_f(OP_PUSHC_NUMBER, f));
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(object_immediate, BL, BR)
{
    debuglog("object_immediate  ->  BL BR");
    append(_self, __new_op(OP_PUSHC_EMPTY_TABLE));
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(object_immediate, BL, object_immediate_item_list, BR) /* todo */
{
    debuglog("object_immediate  ->  BL object_immediate_item_list BR");
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_immediate_table);
    move(_self, _2);
    append(_self, __new_op(OP_PUSHC_TABLE));
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(object_immediate_item_list, object_immediate_item, COMMA, object_immediate_item_list) /* todo */
{
    debuglog("object_immediate_item_list  ->  object_immediate_item COMMA object_immediate_item_list");
    // move2(_self, _1, _3);
    move2(_self, _3, _1);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(object_immediate_item_list, object_immediate_item) /* todo */
{
    debuglog("object_immediate_item_list  ->  object_immediate_item");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(object_immediate_item, IDENTIFIER, COLON, object_immediate_item_value) /* todo */
{
    debuglog("object_immediate_item  ->  IDENTIFIER COLON object_immediate_item_value");
    // !!! we don't support this now.
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(object_immediate_item, STRING_IMMEDIATE, COLON, object_immediate_item_value) /* todo */
{
    debuglog("object_immediate_item  ->  STRING_IMMEDIATE COLON object_immediate_item_value");
    append(_self, __new_op_x(OP_PUSHC_STR, _1));
    move(_self, _3);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(object_immediate_item, NUMBER_IMMEDIATE, COLON, object_immediate_item_value) /* todo */
{
    debuglog("object_immediate_item  ->  NUMBER_IMMEDIATE COLON object_immediate_item_value");
    float f = 0.0f;
    {
        const lx_token* t = _1->token;
        char backup = *(t->text + t->text_len);
        *(t->text + t->text_len) = '\0';
        char *end;
        f = strtof(t->text, &end);
        if (end == t->text) {
            // todo
            assert(false);
        }
        *(t->text + t->text_len) = backup;
    }
    append(_self, __new_op_f(OP_PUSHC_NUMBER, f));
    move(_self, _3);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(object_immediate_item_value, immediate) /* todo */
{
    debuglog("object_immediate_item_value  ->  immediate");
    move(_self, _1);
    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE5(function_define, FUNCTION, SL, SR, stmt_sequence, END)
{
    debuglog("function_define  ->  FUNCTION SL SR stmt_sequence END");
    append(_self, __new_op(OP_FUNC_DEF_BEGIN));
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_local_declare);
    append(_self, __new_op(OP_LOCAL_INIT)); /* init expr(s) have been pushed to stack by caller */
    move(_self, _4);
    append(_self, __new_op(OP_RETURN)); /* we make sure there has at least one `return` */
    append(_self, __new_op(OP_FUNC_DEF_END));
    append(_self, __new_op(OP_PUSHC_FUNC));
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE6(function_define, FUNCTION, SL, identifier_list, SR, stmt_sequence, END)
{
    debuglog("function_define  ->  FUNCTION SL identifier_list SR stmt_sequence END");
    append(_self, __new_op(OP_FUNC_DEF_BEGIN));
    append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_local_declare);
    for (lx_syntax_node* n = _3->next, *tem; n != NULL;) {
        append(_self, __new_op_x(OP_PUSHC_STR, n));
        tem = n->next;
        FREE_SYNTAX_NODE(n);
        n = tem;
    }
    append(_self, __new_op(OP_LOCAL_INIT)); // init expr(s) have been pushed to stack by caller
    //append_with_opinfo(_self, __new_op(OP_TAG), OPINFO_tag_for_function_define_argc_end);
    move(_self, _5);
    append(_self, __new_op(OP_FUNC_DEF_END));
    append(_self, __new_op(OP_PUSHC_FUNC));
    FREE_SYNTAX_NODE(_6);
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(identifier_list, IDENTIFIER, COMMA, identifier_list)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("identifier_list  ->  IDENTIFIER COMMA identifier_list");
    //_self->next = _1;
    //_self->next->next = _3->next;
    lx_syntax_node* end_of__3;
    for(end_of__3 = _3; end_of__3->next != NULL; end_of__3 = end_of__3->next)
        ;
    end_of__3->next = _1;
    _self->next = _3->next;

    // FREE_SYNTAX_NODE(_2); // problem: this is wrong when it comes to using stack allocator
                             // solutation: parser call this function with NULL, so we don't need to free this node
}
LX_CALLBACK_DECLARE1(identifier_list, IDENTIFIER)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("identifier_list  ->  IDENTIFIER");
    _self->next = _1;
    // FREE_SYNTAX_NODE(_1); // we should free this node in it's father syntax node: `XXX -> identifier_list`
}

LX_CALLBACK_DECLARE1(assign_op, EQL)
{
    debuglog("assign_op  ->  EQL");
    append(_self, __new_op(OP_ASSIGN));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, ADD_EQL)
{
    debuglog("assign_op  ->  ADD_EQL");
    append(_self, __new_op(OP_ADD_ASSIGN));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, SUB_EQL)
{
    debuglog("assign_op  ->  SUB_EQL");
    append(_self, __new_op(OP_SUB_ASSIGN));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, MUL_EQL)
{
    debuglog("assign_op  ->  MUL_EQL");
    append(_self, __new_op(OP_MUL_ASSIGN));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, DIV_EQL)
{
    debuglog("assign_op  ->  DIV_EQL");
    append(_self, __new_op(OP_DIV_ASSIGN));
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(logical_op, AND)
{
    debuglog("logical_op  ->  AND");
    append(_self, __new_op(OP_AND));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(logical_op, OR)
{
    debuglog("logical_op  ->  OR");
    append(_self, __new_op(OP_OR));
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(compare_op, LESS)
{
    debuglog("compare_op  ->  LESS");
    append(_self, __new_op(OP_LESS));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, GREATER)
{
    debuglog("compare_op  ->  GREATER");
    append(_self, __new_op(OP_GREATER));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, LESS_EQL)
{
    debuglog("compare_op  ->  LESS_EQL");
    append(_self, __new_op(OP_LESS_EQL));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, GREATER_EQL)
{
    debuglog("compare_op  ->  GREATER_EQL");
    append(_self, __new_op(OP_GREATER_EQL));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, EQL_EQL)
{
    debuglog("compare_op  ->  EQL_EQL");
    append(_self, __new_op(OP_EQL_EQL));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, NOT_EQL)
{
    debuglog("compare_op  ->  NOT_EQL");
    append(_self, __new_op(OP_NOT_EQL));
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(addtive_op, ADD)
{
    debuglog("addtive_op  ->  ADD");
    append(_self, __new_op(OP_ADD));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(addtive_op, SUB)
{
    debuglog("addtive_op  ->  SUB");
    append(_self, __new_op(OP_SUB));
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(multiply_op, MUL)
{
    debuglog("multiply_op  ->  MUL");
    append(_self, __new_op(OP_MUL));
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(multiply_op, DIV)
{
    debuglog("multiply_op  ->  DIV");
    append(_self, __new_op(OP_DIV));
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(prefix_op, SUB) /* inverst operator */
{
    debuglog("prefix_op  ->  SUB");
    append(_self, __new_op(OP_INVERST));
    FREE_SYNTAX_NODE(_1);
}