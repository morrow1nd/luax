#include "./parser.h"
#include "./base.h"
#include "./mem.h"
#include "./config.h"
#include "./parser_callback.h"


//
// Token Scanner - internal use
//

static lx_token lx_token_end;

static lx_token* add_one_token(lx_token_scanner *s, int token_type, char *ptr, int text_len, int linenum)
{
    s->token_number ++;
    if(s->token_number > s->tokens_capacity){
        lx_token **tem_tokens = (lx_token**)lx_malloc(sizeof(lx_token*) * (s->tokens_capacity / LX_CONFIG_TOKEN_GRAIN + 1));
        memcpy(tem_tokens, s->tokens, s->tokens_capacity * sizeof(lx_token*));
        if(s->tokens)
            lx_free(s->tokens);
        s->tokens = tem_tokens;
        s->tokens_capacity += LX_CONFIG_TOKEN_GRAIN;
    }
    lx_token* t = s->tokens[s->token_number] = LX_NEW(lx_token);
    t->type = token_type;
    t->text = ptr;
    t->text_len = text_len;
    t->linenum = linenum;
    return t;
}

static token_error(lx_token_scanner *s, char *ptr, int linenum)
{
    add_one_token(s, LX_TOKEN_ERROR, ptr, 1, linenum);
}

lx_token_scanner* lx_scan_token(const char *source_code, const int source_code_length)
{
    lx_token_scanner * s = LX_NEW(lx_token_scanner);
    if(!s)
        return NULL;
    s->raw_source_code = source_code;
    s->raw_source_code_length = source_code_length;
    s->tokens_capacity = 0;
    s->token_number = 0;
    s->tokens = NULL;

    lx_token_end.linenum = -1;
    lx_token_end.text = source_code + source_code_length;
    lx_token_end.text_len = 0;
    lx_token_end.type = LX_TOKEN_END;

    const char *p = source_code;

    int linenum = 1;
    char vartem[LX_CONFIG_IDENTIFIER_MAX_LENGTH];
    while(p - source_code < source_code_length){
        // find one token
        if (*p == '\0') {
            assert(false);
            return NULL;
        }
        if(memcmp("//", p, 2) == 0 || memcmp("--", p, 2) == 0){
            int i = 0;
            for (; *(p + i) != '\n'; ++i)
                ;
            p += i;
            continue;
        }
        if (memcmp(p, "/*", 2) == 0) {
            int i = 0;
            for (; ; ++i) {
                if (*(p + i) == '\n')
                    ++linenum;
                if (*(p + i) == '*' && *(p + i + 1) == '/') {
                    ++i;
                    break;
                }
            }
            p += i + 1;
            continue;
        }
        if(memcmp(p, "--[[", 4) == 0){
            int i = 0;
            for (; ; ++i) {
                if (*(p + i) == '\n')
                    ++linenum;
                if (*(p + i) == ']' && *(p + i + 1) == ']') {
                    ++i;
                    break;
                }
            }
            p += i + 1;
            continue;
        }
        if (memcmp(p, "break", 5) == 0) {
            add_one_token(s, LX_TOKEN_BREAK, p, 5, linenum);
            p += 5;
            continue;
        }
        if (memcmp(p, "continue", 8) == 0) {
            add_one_token(s, LX_TOKEN_CONTINUE, p, 8, linenum);
            p += 8;
            continue;
        }
        if (memcmp(p, "return", 6) == 0) {
            add_one_token(s, LX_TOKEN_RETURN, p, 6, linenum);
            p += 6;
            continue;
        }
        if (memcmp(p, "if", 2) == 0) {
            add_one_token(s, LX_TOKEN_IF, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "then", 4) == 0) {
            add_one_token(s, LX_TOKEN_THEN, p, 4, linenum);
            p += 4;
            continue;
        }
        if (memcmp(p, "else", 4) == 0) {
            add_one_token(s, LX_TOKEN_ELSE, p, 4, linenum);
            p += 4;
            continue;
        }
        if (memcmp(p, "while", 5) == 0) {
            add_one_token(s, LX_TOKEN_WHILE, p, 5, linenum);
            p += 5;
            continue;
        }
        if (memcmp(p, "for", 3) == 0) {
            add_one_token(s, LX_TOKEN_FOR, p, 3, linenum);
            p += 3;
            continue;
        }
        if (memcmp(p, "function", 8) == 0) {
            add_one_token(s, LX_TOKEN_FUNCTION, p, 8, linenum);
            p += 8;
            continue;
        }
        if (memcmp(p, "not", 3) == 0) {
            add_one_token(s, LX_TOKEN_NOT, p, 3, linenum);
            p += 3;
            continue;
        }
        if (memcmp(p, "and", 3) == 0) {
            add_one_token(s, LX_TOKEN_AND, p, 3, linenum);
            p += 3;
            continue;
        }
        if (memcmp(p, "or", 2) == 0) {
            add_one_token(s, LX_TOKEN_OR, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "<=", 2) == 0) {
            add_one_token(s, LX_TOKEN_LESS_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, ">=", 2) == 0) {
            add_one_token(s, LX_TOKEN_GREATER_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "==", 2) == 0) {
            add_one_token(s, LX_TOKEN_EQL_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "!=", 2) == 0) {
            add_one_token(s, LX_TOKEN_NOT_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "+=", 2) == 0) {
            add_one_token(s, LX_TOKEN_ADD_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "-=", 2) == 0) {
            add_one_token(s, LX_TOKEN_SUB_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "*=", 2) == 0) {
            add_one_token(s, LX_TOKEN_MUL_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "/=", 2) == 0) {
            add_one_token(s, LX_TOKEN_DIV_EQL, p, 2, linenum);
            p += 2;
            continue;
        }
        if (memcmp(p, "nil", 3) == 0) {
            add_one_token(s, LX_TOKEN_NIL, p, 3, linenum);
            p += 3;
            continue;
        }
        if (memcmp(p, "false", 5) == 0) {
            add_one_token(s, LX_TOKEN_FALSE, p, 5, linenum);
            p += 5;
            continue;
        }
        if (memcmp(p, "true", 4) == 0) {
            add_one_token(s, LX_TOKEN_TRUE, p, 4, linenum);
            p += 4;
            continue;
        }

        int type = -1;
        switch (*p) {
        case ' ':
        case '\r':
        case '\t':
            ++p;
            continue;
        case '\n':
            ++linenum;
            ++p;
            continue;
        case '+':
            type = '+';
            break;
        case '-':
            type = '-';
            break;
        case '*':
            type = '*';
            break;
        case '/':
            type = '/';
            break;
        case '=':
            type = '=';
            break;
        case '<':
            type = '<';
            break;
        case '>':
            type = '>';
            break;
        case '(':
            type = '(';
            break;
        case ')':
            type = ')';
            break;
        case '[':
            type = '[';
            break;
        case ']':
            type = ']';
            break;
        case '{':
            type = '{';
            break;
        case '}':
            type = '}';
            break;
        case ';':
            type = ';';
            break;
        case ':':
            type = ':';
            break;
        default: {
            if (isdigit(*p)) {
                const char * pp = p;
                float data = strtod(p, &pp);
                if ((pp - p) == 0) {
                    token_error(s, p, linenum);
                    return s;
                }
                add_one_token(s, LX_TOKEN_NUMBER_IMMEDIATE, p, pp - p, linenum);
                p = pp;
                continue;
            } else if (isalpha(*p) || *p == '_') {
                int i = 1;
                while(*(p + i) != '\0' && (isalnum(*(p + i)) || *(p + i) == '_'))
                    ++i;

                add_one_token(s, LX_TOKEN_IDENTIFIER, p, i, linenum);
                p += i;
                continue;
            } else {
                token_error(s, p, linenum);
                return s;
            }
            break;
        }
        }
        add_one_token(s, type, p, 1, linenum);
        ++p;
    }
    // scan all source code successful
    s->curr = -1;
    return s;
}
lx_token* lx_token_next(lx_token_scanner *s)
{
    //if (s->curr >= s->token_number - 1)
    //    return &lx_token_end;
    //else
    //return s->tokens[s->curr + 1];
    return lx_token_nextN(s, 1);
}
lx_token* lx_token_nextN(lx_token_scanner *s, int n)
{
    if (s->curr >= s->token_number - n) 
        return &lx_token_end;
    else
        return s->tokens[s->curr + n];
}
int lx_token_scanner_get_curr_state(lx_token_scanner *s)
{
    return s->curr;
}
/**
 * return: the state before apply this new state.
 */
int lx_token_scanner_recover_state(lx_token_scanner *s, int new_state) {
    if(new_state < -1)
        assert(false && "new_statem must >= -1");
    s->curr = new_state;
}

int lx_token_scanner_move_forward(lx_token_scanner *s, int step)
{
    if(s->curr + step < -1 || s->curr + step >= s->token_number)
        assert(false && "you must keep s->curr in the right range");
    s->curr += step;
}



//
// Recursive Descent Parser
//

#define CURR(_parser) (lx_token_nextN(_parser->scanner, 0))
#define NEXT(_parser) (lx_token_next(_parser->scanner))
#define NEXT_TYPE_EQUAL(_parser, _type) (NEXT(_parser)->type == _type)
#define GOTO_NEXT(_parser) (lx_token_move_forward(_parser->scanner, 1))
#define NEW_SYNTAX_NODE(_node) lx_syntax_node * _node = LX_NEW(lx_syntax_node)
#define NEW_SYNTAX_NODE_T(_node, _token) \
lx_syntax_node * _node = LX_NEW(lx_syntax_node);\
_node->token = _token

#define FREE_SYNTAX_NODE(_node) lx_free(_node)

int lx_parser_begin(lx_parser * p, lx_parser *parser)
{

}


static int compile_unit(lx_parser * p, lx_syntax_node * self)
{

}

static int immediate(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    NEW_SYNTAX_NODE(object_immediate_node);
    if (object_immediate(p, object_immediate_node) == 0) {
        LX_CALLBACK_CALL1(immediate, object_immediate, self, object_immediate_node);
        return 0;
    }
    FREE_SYNTAX_NODE(object_immediate_node);

    NEW_SYNTAX_NODE(function_define_node);
    if (function_define(p, function_define_node) == 0) {
        LX_CALLBACK_CALL1(immediate, function_define, self, function_define_node);
        return 0;
    }
    FREE_SYNTAX_NODE(function_define_node);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_STRING_IMMEDIATE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(string_immediate_node, CURR(p));
        LX_CALLBACK_CALL1(immediate, STRING_IMMEDIATE, self, string_immediate_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_NUMBER_IMMEDIATE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(number_immediate_node, CURR(p));
        LX_CALLBACK_CALL1(immediate, NUMBER_IMMEDIATE, self, number_immediate_node);
        return 0;
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}
static int object_immediate(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, '{')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(bl_node, CURR(p));
        NEW_SYNTAX_NODE(object_immediate_item_list_node);
        if (NEXT_TYPE_EQUAL(p, '}')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(br_node, CURR(p));
            LX_CALLBACK_CALL2(object_immediate, BL, BR,
                self, bl_node, br_node);
            return 0;
        } else if (object_immediate_item_list(p, object_immediate_item_list_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, '}')) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(br_node, CURR(p));
                LX_CALLBACK_CALL3(object_immediate, BL, object_immediate_item_list, BR,
                    self, bl_node, object_immediate_item_list_node, br_node);
                return 0;
            }
        }
        FREE_SYNTAX_NODE(object_immediate_item_list_node);
        FREE_SYNTAX_NODE(bl_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}
static int object_immediate_item_list(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    NEW_SYNTAX_NODE(object_immediate_item_node);
    if (object_immediate_item(p, object_immediate_item_node) == 0) {
        if (NEXT_TYPE_EQUAL(p, ':')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(colon_node, CURR(p));
            NEW_SYNTAX_NODE(object_immediate_item_list_node);
            if (object_immediate_item_list(p, object_immediate_item_list_node) == 0) {
                LX_CALLBACK_CALL3(object_immediate_item_list, object_immediate_item, COLON, object_immediate_item_list,
                    self, object_immediate_item_node, colon_node, object_immediate_item_list_node);
                return 0;
            }
            FREE_SYNTAX_NODE(object_immediate_item_list_node);
            FREE_SYNTAX_NODE(colon_node);
        } else {
            LX_CALLBACK_CALL1(object_immediate_item_list, object_immediate_item,
                self, object_immediate_item_node);
            return 0;
        }
    }
    FREE_SYNTAX_NODE(object_immediate_item_node);

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}
static int object_immediate_item(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_IDENTIFIER)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(identifier_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ':')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(colon_node, CURR(p));
            NEW_SYNTAX_NODE(object_immediate_item_value_node);
            if (object_immediate_item_value(p, object_immediate_item_value_node) == 0) {
                LX_CALLBACK_CALL3(object_immediate_item, IDENTIFIER, COLON, object_immediate_item_value_node, 
                    self, identifier_node, colon_node, object_immediate_item_value_node);
                return 0;
            }
            FREE_SYNTAX_NODE(object_immediate_item_value_node);
            FREE_SYNTAX_NODE(colon_node);
        }
        FREE_SYNTAX_NODE(identifier_node);
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_STRING_IMMEDIATE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(string_immediate_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ':')) {
            GOTO_NEXT(p);
           NEW_SYNTAX_NODE_T(colon_node, CURR(p));
           NEW_SYNTAX_NODE(object_immediate_item_value_node);
           if (object_immediate_item_value(p, object_immediate_item_value_node) == 0) {
               LX_CALLBACK_CALL3(object_immediate_item, STRING_IMMEDIATE, COLON, object_immediate_item_value_node,
                   self, string_immediate_node, colon_node, object_immediate_item_value_node);
               return 0;
           }
            FREE_SYNTAX_NODE(object_immediate_item_value_node);
            FREE_SYNTAX_NODE(colon_node);
        }
        FREE_SYNTAX_NODE(string_immediate_node);
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_NUMBER_IMMEDIATE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(number_immediate_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ':')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(colon_node, CURR(p));
            NEW_SYNTAX_NODE(object_immediate_item_value_node);
            if (object_immediate_item_value(p, object_immediate_item_value_node) == 0) {
                LX_CALLBACK_CALL3(object_immediate_item, NUMBER_IMMEDIATE, COLON, object_immediate_item_value_node,
                    self, number_immediate_node, colon_node, object_immediate_item_value_node);
                return 0;
            }
            FREE_SYNTAX_NODE(object_immediate_item_value_node);
            FREE_SYNTAX_NODE(colon_node);
        }
        FREE_SYNTAX_NODE(number_immediate_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}
static int object_immediate_item_value(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(immediate_node);
    if (immediate(p, immediate_node) == 0) {
        LX_CALLBACK_CALL1(object_immediate_item_value, immediate, self, immediate_node);
        return 0;
    }
    FREE_SYNTAX_NODE(immediate_node);
    return -1;
}

static int function_define(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_FUNCTION)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(function_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, '(')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(sl_node, CURR(p));
            NEW_SYNTAX_NODE(identifier_list_node);
            if (identifier_list(p, identifier_list_node) == 0) {
                if (NEXT_TYPE_EQUAL(p, ')')) {
                    GOTO_NEXT(p);
                    NEW_SYNTAX_NODE_T(sr_node, CURR(p));
                    NEW_SYNTAX_NODE(stmt_sequence_node);
                    if (stmt_sequence(p, stmt_sequence_node) == 0) {
                        if (NEXT_TYPE_EQUAL(p, LX_TOKEN_END)) {
                            GOTO_NEXT(p);
                            NEW_SYNTAX_NODE_T(end_node, CURR(p));
                            LX_CALLBACK_CALL6(function_define, FUNCTION, SL, identifier_list, SR, stmt_sequence, END,
                                self, function_node, sl_node, identifier_list_node, sr_node, stmt_sequence_node, end_node);
                            return 0;
                        }
                    }
                    FREE_SYNTAX_NODE(stmt_sequence_node);
                    FREE_SYNTAX_NODE(sr_node);
                }
            }
            FREE_SYNTAX_NODE(identifier_list_node);
            FREE_SYNTAX_NODE(sl_node);
        }
        FREE_SYNTAX_NODE(function_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}
static int identifier_list(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_IDENTIFIER)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(identifier_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ',')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(comma_node, CURR(p));
            NEW_SYNTAX_NODE(identifier_list_node);
            if (identifier_list(p, identifier_list_node) == 0) {
                LX_CALLBACK_CALL3(identifier_list, IDENTIFIER, COMMA, identifier_list, self, identifier_node, comma_node, identifier_list_node);
                return 0;
            }
            FREE_SYNTAX_NODE(identifier_list_node);
            FREE_SYNTAX_NODE(comma_node);
        } else {
            LX_CALLBACK_CALL1(identifier_list, IDENTIFIER, self, identifier_node);
            return 0;
        }
        FREE_SYNTAX_NODE(identifier_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}

static int assign_op(lx_parser *p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, '=')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, EQL, self, eql_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_ADD_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(add_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_ADD_EQL, self, add_eql_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_SUB_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sub_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_SUB_EQL, self, sub_eql_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_MUL_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(mul_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_MUL_EQL, self, mul_eql_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_DIV_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(div_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_DIV_EQL, self, div_eql_node);
        return 0;
    } else {
        return -1;
    }
}

static int logical_op(lx_parser *p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_AND)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(and_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_AND, self, and_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_OR)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(or_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_OR, self, or_node);
        return 0;
    } else {
        return -1;
    }
}

static int compare_op(lx_parser *p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, '<')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(less_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LESS, self, less_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, '>')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(greater_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, GREATER, self, greater_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_LESS_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(less_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_LESS_EQL, self, less_eql_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_GREATER_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(greater_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_GREATER_EQL, self, greater_eql_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_EQL_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(eql_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_EQL_EQL, self, eql_eql_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, LX_TOKEN_NOT_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(not_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LX_TOKEN_NOT_EQL, self, not_eql_node);
        return 0;
    } else {
        return -1;
    }
}

static int addtive_op(lx_parser * p, lx_syntax_node * self)
{
    if (NEXT_TYPE_EQUAL(p, '+')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(add_node, CURR(p));
        LX_CALLBACK_CALL1(addtive_op, ADD, self, add_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, '-')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sub_node, CURR(p));
        LX_CALLBACK_CALL1(addtive_op, SUB, self, sub_node);
        return 0;
    } else {
        return -1;
    }
}

static int multiply_op(lx_parser *p, lx_syntax_node* self)
{
    if (NEXT_TYPE_EQUAL(p, '*')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(mul_node, CURR(p));
        LX_CALLBACK_CALL1(multiply_op, MUL, self, mul_node);
        return 0;
    } else if (NEXT_TYPE_EQUAL(p, '/')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(div_node, CURR(p));
        LX_CALLBACK_CALL1(multiply_op, DIV, self, div_node);
        return 0;
    } else {
        return -1;
    }
}

static int prefix_op(lx_parser * p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, '-')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sub_node, CURR(p));
        LX_CALLBACK_CALL1(prefix_op, SUB, self, sub_node);
        return 0;
    } else {
        return -1;
    }
}