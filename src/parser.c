#include "./parser.h"
#include "./base.h"
#include "./mem.h"
#include "./config.h"
#include "./parser_callback.h"


// helper function
char* lx_helper_dump_token(lx_token *token, char * outstr)
{
    char token_text[LX_CONFIG_IDENTIFIER_MAX_LENGTH + 1];
    if(token->text_len > 0 && token->text_len < LX_CONFIG_IDENTIFIER_MAX_LENGTH){
        memcpy(token_text, token->text, token->text_len);
        token_text[token->text_len] = '\0';
    }else{
        token_text[0] = '\0';
    }
    sprintf(outstr, "T%d,%d,%s,L%d", token->type, token->text_len, token_text, token->linenum);
    return outstr;
}


//
// Token Scanner - internal use
//

static lx_token lx_token_end;

static lx_token* add_one_token(lx_token_scanner *s, int token_type, char *ptr, int text_len, int linenum)
{
    s->token_number ++;
    if(s->token_number > s->tokens_capacity){
        lx_token **tem_tokens = (lx_token**)lx_malloc(sizeof(lx_token*) * (s->tokens_capacity / LX_CONFIG_TOKEN_GRAIN + 1) * LX_CONFIG_TOKEN_GRAIN);
        if (s->tokens) {
            memcpy(tem_tokens, s->tokens, s->tokens_capacity * sizeof(lx_token*));
            lx_free(s->tokens);
        }
        s->tokens = tem_tokens;
        s->tokens_capacity += LX_CONFIG_TOKEN_GRAIN;
    }
    lx_token* t = LX_NEW(lx_token);
    t->type = token_type;
    t->text = ptr;
    t->text_len = text_len;
    t->linenum = linenum;
    s->tokens[s->token_number] = t;
    return t;
}

static lx_token* token_error(lx_token_scanner *s, char *ptr, int linenum)
{
    return add_one_token(s, LX_TOKEN_ERROR, ptr, 1, linenum);
}

lx_token_scanner* lx_scan_token(char *source_code, const int source_code_length)
{
    lx_token_scanner * s = LX_NEW(lx_token_scanner);
    if(!s)
        return NULL;
    s->raw_source_code = source_code;
    s->raw_source_code_length = (int)source_code_length;
    s->tokens_capacity = 0;
    s->token_number = 0;
    s->tokens = NULL;

    lx_token_end.linenum = -1;
    lx_token_end.text = source_code + (int)source_code_length;
    lx_token_end.text_len = 0;
    lx_token_end.type = LX_TOKEN_END;

    char *p = source_code;

    int linenum = 1;
    char vartem[LX_CONFIG_IDENTIFIER_MAX_LENGTH];
    while(p - source_code < source_code_length){
        // find one token
        if (*p == '\0') {
            assert(false);
            return NULL;
        }
        if (memcmp(p, "--[[", 4) == 0) {
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
                char * pp = p;
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
                while (*(p + i) != '\0' && (isalnum(*(p + i)) || *(p + i) == '_'))
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
lx_token* lx_token_nextN(lx_token_scanner *s, int n)
{
    if (s->curr >= s->token_number - n)
        return &lx_token_end;
    else
        return s->tokens[s->curr + n];
}
lx_token* lx_token_next(lx_token_scanner *s)
{
    //if (s->curr >= s->token_number - 1)
    //    return &lx_token_end;
    //else
    //return s->tokens[s->curr + 1];
    return lx_token_nextN(s, 1);
}
int lx_token_scanner_get_curr_state(lx_token_scanner *s)
{
    return s->curr;
}
/**
 * return: the state before apply this new state.
 */
int lx_token_scanner_recover_state(lx_token_scanner *s, int new_state) {
    if (new_state < -1)
        assert(false && "new_statem must >= -1");
    s->curr = new_state;
}

int lx_token_scanner_move_forward(lx_token_scanner *s, int step)
{
    if (s->curr + step < -1 || s->curr + step >= s->token_number)
        assert(false && "you must keep s->curr in the right range");
    s->curr += step;
}



//
// Recursive Descent Parser
//

#define CURR(_parser) (lx_token_nextN(_parser->scanner, 0))
#define NEXT(_parser) (lx_token_next(_parser->scanner))
#define NEXT_TYPE_EQUAL(_parser, _type) (NEXT(_parser)->type == _type)
#define GOTO_NEXT(_parser) (lx_token_scanner_move_forward(_parser->scanner, 1))
#define NEW_SYNTAX_NODE(_node) lx_syntax_node * _node = LX_NEW(lx_syntax_node)
#define NEW_SYNTAX_NODE_T(_node, _token) \
lx_syntax_node * _node = LX_NEW(lx_syntax_node);\
_node->token = _token

#define FREE_SYNTAX_NODE(_node) lx_free(_node)

//
// forward declaration
//

static int compile_unit(lx_parser *p, lx_syntax_node *self);
static int stmt_sequence(lx_parser *p, lx_syntax_node *self);
static int stmt(lx_parser *p, lx_syntax_node *self);
static int if_stmt(lx_parser *p, lx_syntax_node *self);
static int while_stmt(lx_parser *p, lx_syntax_node *self);
static int for_stmt(lx_parser *p, lx_syntax_node *self);
static int expr_stmt(lx_parser *p, lx_syntax_node *self);
static int expr_list(lx_parser *p, lx_syntax_node *self);
static int prefix_expr_list(lx_parser *p, lx_syntax_node *self);

static int expr(lx_parser *p, lx_syntax_node *self);
static int assign_expr(lx_parser *p, lx_syntax_node *self);
static int logical_expr(lx_parser *p, lx_syntax_node *self);
static int compare_expr(lx_parser *p, lx_syntax_node *self);
static int addtive_expr(lx_parser *p, lx_syntax_node *self);
static int multiply_expr(lx_parser *p, lx_syntax_node *self);
static int prefix_expr(lx_parser *p, lx_syntax_node *self);
static int suffix_expr(lx_parser *p, lx_syntax_node *self);
static int single_expr(lx_parser *p, lx_syntax_node *self);
static int suffix_op(lx_parser *p, lx_syntax_node *self);

static int immediate(lx_parser *p, lx_syntax_node *self);
static int object_immediate(lx_parser *p, lx_syntax_node *self);
static int object_immediate_item_list(lx_parser *p, lx_syntax_node *self);
static int object_immediate_item(lx_parser *p, lx_syntax_node *self);
static int object_immediate_item_value(lx_parser *p, lx_syntax_node *self);

static int function_define(lx_parser *p, lx_syntax_node *self);
static int identifier_list(lx_parser *p, lx_syntax_node *self);

static int assign_op(lx_parser *p, lx_syntax_node *self);
static int logical_op(lx_parser *p, lx_syntax_node *self);
static int compare_op(lx_parser *p, lx_syntax_node *self);
static int addtive_op(lx_parser * p, lx_syntax_node * self);
static int multiply_op(lx_parser *p, lx_syntax_node* self);
static int prefix_op(lx_parser * p, lx_syntax_node *self);


int lx_parser_begin(lx_parser * p, lx_parser *parser)
{

}


static int compile_unit(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(stmt_sequence_node);
    if (stmt_sequence(p, stmt_sequence_node) == 0) {
        LX_CALLBACK_CALL1(compile_unit, stmt_sequence,
            self, stmt_sequence_node);
        return 0;
    }
    FREE_SYNTAX_NODE(stmt_sequence_node);
    return -1;
}

static int stmt_sequence(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(stmt_node);
    if (stmt(p, stmt_node) == 0) {
        NEW_SYNTAX_NODE(stmt_sequence_node);
        if (stmt_sequence(p, stmt_sequence_node) == 0) {
            LX_CALLBACK_CALL2(stmt_sequence, stmt, stmt_sequence,
                self, stmt_node, stmt_sequence_node);
            return 0;
        }
        FREE_SYNTAX_NODE(stmt_sequence_node);
        LX_CALLBACK_CALL1(stmt_sequence, stmt,
            self, stmt_node);
        return 0;
    }
    FREE_SYNTAX_NODE(stmt_node);
    LX_CALLBACK_CALL1(stmt_sequence, EMPTY_SYMBOL,
        self, NULL);
    return 0;
}

static int stmt(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, ';')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(eos_node, CURR(p));
        LX_CALLBACK_CALL1(stmt, EOS,
            self, eos_node);
        return 0;
    }

    NEW_SYNTAX_NODE(if_stmt_node);
    if (if_stmt(p, if_stmt_node) == 0) {
        LX_CALLBACK_CALL1(stmt, if_stmt,
            self, if_stmt_node);
        return 0;
    }
    FREE_SYNTAX_NODE(if_stmt_node);

    NEW_SYNTAX_NODE(while_stmt_node);
    if (while_stmt(p, while_stmt_node) == 0) {
        LX_CALLBACK_CALL1(stmt, while_stmt,
            self, while_stmt_node);
        return 0;
    }
    FREE_SYNTAX_NODE(while_stmt_node);

    NEW_SYNTAX_NODE(for_stmt_node);
    if (for_stmt(p, for_stmt_node) == 0) {
        LX_CALLBACK_CALL1(stmt, for_stmt,
            self, for_stmt_node);
        return 0;
    }
    FREE_SYNTAX_NODE(for_stmt_node);

    NEW_SYNTAX_NODE(expr_stmt_node);
    if (expr_stmt(p, expr_stmt_node) == 0) {
        LX_CALLBACK_CALL1(stmt, expr_stmt,
            self, expr_stmt_node);
        return 0;
    }
    FREE_SYNTAX_NODE(expr_stmt_node);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_BREAK)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(break_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ';')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(eos_node, CURR(p));
            LX_CALLBACK_CALL2(stmt, BREAK, EOS,
                self, break_node, eos_node);
            return 0;
        }
        FREE_SYNTAX_NODE(break_node);
        lx_token_scanner_recover_state(p->scanner, backup_state);
        return -1; // no need to continue
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_CONTINUE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(continue_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ';')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(eos_node, CURR(p));
            LX_CALLBACK_CALL2(stmt, CONTINUE, EOS,
                self, continue_node, eos_node);
            return 0;
        }
        FREE_SYNTAX_NODE(continue_node);
        lx_token_scanner_recover_state(p->scanner, backup_state);
        return -1; // no need to continue
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_RETURN)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(return_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ';')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(eos_node, CURR(p));
            LX_CALLBACK_CALL2(stmt, RETURN, EOS,
                self, return_node, eos_node);
            return 0;
        }
        NEW_SYNTAX_NODE(expr_list_node);
        if (expr_list(p, expr_list_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, ';')) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(eos_node, CURR(p));
                LX_CALLBACK_CALL3(stmt, RETURN, expr_list, EOS,
                    self, return_node, expr_list_node, eos_node);
                return 0;
            }
        }
        FREE_SYNTAX_NODE(expr_list_node);
        FREE_SYNTAX_NODE(return_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}

static int if_stmt(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_IF)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(if_node, CURR(p));
        NEW_SYNTAX_NODE(expr_node);
        if (expr(p, expr_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, LX_TOKEN_THEN)) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(then_node, CURR(p));
                NEW_SYNTAX_NODE(stmt_sequence_node);
                if (stmt_sequence(p, stmt_sequence_node) == 0) {
                    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_END)) {
                        GOTO_NEXT(p);
                        NEW_SYNTAX_NODE_T(end_node, CURR(p));
                        LX_CALLBACK_CALL5(if_stmt, IF, expr, THEN, stmt_sequence, END,
                            self, if_node, expr_node, then_node, stmt_sequence_node, end_node);
                        return 0;
                    }
                    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_ELSE)) {
                        GOTO_NEXT(p);
                        NEW_SYNTAX_NODE_T(else_node, CURR(p));
                        NEW_SYNTAX_NODE(stmt_sequence_node2);
                        if (stmt_sequence(p, stmt_sequence_node2) == 0) {
                            if (NEXT_TYPE_EQUAL(p, LX_TOKEN_END)) {
                                GOTO_NEXT(p);
                                NEW_SYNTAX_NODE_T(end_node, CURR(p));
                                LX_CALLBACK_CALL7(if_stmt, IF, expr, THEN, stmt_sequence, ELSE, stmt_sequence, END,
                                    self, if_node, expr_node, then_node, stmt_sequence_node, else_node, stmt_sequence_node2, end_node);
                                return 0;
                            }
                        }
                        FREE_SYNTAX_NODE(stmt_sequence_node2);
                        FREE_SYNTAX_NODE(else_node);
                    }
                }
                FREE_SYNTAX_NODE(stmt_sequence_node);
                FREE_SYNTAX_NODE(then_node);
            }
        }
        FREE_SYNTAX_NODE(expr_node);
        FREE_SYNTAX_NODE(if_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}

static int while_stmt(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_WHILE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(while_node, CURR(p));
        NEW_SYNTAX_NODE(expr_node);
        if (expr(p, expr_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, LX_TOKEN_THEN)) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(then_node, CURR(p));
                NEW_SYNTAX_NODE(stmt_sequence_node);
                if (stmt_sequence(p, stmt_sequence_node) == 0) {
                    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_END)) {
                        GOTO_NEXT(p);
                        NEW_SYNTAX_NODE_T(end_node, CURR(p));
                        LX_CALLBACK_CALL5(while_stmt, WHILE, expr, THEN, stmt_sequence, END,
                            self, while_node, expr_node, then_node, stmt_sequence_node, end_node);
                        return 0;
                    }
                }
                FREE_SYNTAX_NODE(stmt_sequence_node);
                FREE_SYNTAX_NODE(then_node);
            }
        }
        FREE_SYNTAX_NODE(expr_node);
        FREE_SYNTAX_NODE(while_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}

static int for_stmt(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_FOR)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(for_node, CURR(p));
        NEW_SYNTAX_NODE(expr_node);
        if (expr(p, expr_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, ';')) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(eos_node, CURR(p));
                NEW_SYNTAX_NODE(expr_node2);
                if (expr(p, expr_node2) == 0) {
                    if (NEXT_TYPE_EQUAL(p, ';')) {
                        GOTO_NEXT(p);
                        NEW_SYNTAX_NODE_T(eos_node2, CURR(p));
                        NEW_SYNTAX_NODE(expr_node3);
                        if (expr(p, expr_node3) == 0) {
                            if (NEXT_TYPE_EQUAL(p, LX_TOKEN_THEN)) {
                                GOTO_NEXT(p);
                                NEW_SYNTAX_NODE_T(then_node, CURR(p));
                                NEW_SYNTAX_NODE(stmt_sequence_node);
                                if (stmt_sequence(p, stmt_sequence_node) == 0) {
                                    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_END)) {
                                        GOTO_NEXT(p);
                                        NEW_SYNTAX_NODE_T(end_node, CURR(p));
                                        LX_CALLBACK_CALL9(for_stmt, FOR, expr, EOS, expr, EOS, expr, THEN, stmt_sequence, END,
                                            self, for_node, expr_node, eos_node, expr_node2, eos_node2, expr_node3, then_node, stmt_sequence_node, end_node);
                                        return 0;
                                    }
                                }
                                FREE_SYNTAX_NODE(stmt_sequence_node);
                                FREE_SYNTAX_NODE(then_node);
                            }
                        }
                        FREE_SYNTAX_NODE(expr_node3);
                        FREE_SYNTAX_NODE(eos_node2);
                    }
                }
                FREE_SYNTAX_NODE(expr_node2);
                FREE_SYNTAX_NODE(eos_node);
            }
        }
        FREE_SYNTAX_NODE(expr_node);
        FREE_SYNTAX_NODE(for_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}

static int expr_stmt(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(expr_node);
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);
    if (expr(p, expr_node) == 0) {
        if (NEXT_TYPE_EQUAL(p, ';')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(eos_node, CURR(p));
            LX_CALLBACK_CALL2(expr_stmt, expr, EOS,
                self, expr_node, eos_node);
            return 0;
        }
    }
    lx_token_scanner_recover_state(p->scanner, backup_state);
    FREE_SYNTAX_NODE(expr_node);
    return -1;
}

static int expr_list(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(expr_node);
    if (expr(p, expr_node) == 0) {
        int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (NEXT_TYPE_EQUAL(p, ',')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(comma_node, CURR(p));
            NEW_SYNTAX_NODE(expr_list_node);
            if (expr_list(p, expr_list_node) == 0) {
                LX_CALLBACK_CALL3(expr_list, expr, COMMA, expr_list,
                    self, expr_node, comma_node, expr_list_node);
                return 0;
            }
            FREE_SYNTAX_NODE(expr_list_node);
            FREE_SYNTAX_NODE(comma_node);
        }
        lx_token_scanner_recover_state(p->scanner, backup_state);
        LX_CALLBACK_CALL1(expr_list, expr,
            self, expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(expr_node);

    return -1;
}

static int prefix_expr_list(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(prefix_expr_node);
    if (prefix_expr(p, prefix_expr_node) == 0) {
        int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (NEXT_TYPE_EQUAL(p, ',')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(comma_node, CURR(p));
            NEW_SYNTAX_NODE(prefix_expr_list_node);
            if (prefix_expr_list(p, prefix_expr_list_node) == 0) {
                LX_CALLBACK_CALL3(prefix_expr_list, prefix_expr, COMMA, prefix_expr_list,
                    self, prefix_expr_node, comma_node, prefix_expr_list_node);
                return 0;
            }
            FREE_SYNTAX_NODE(prefix_expr_list_node);
            FREE_SYNTAX_NODE(comma_node);
        }
        lx_token_scanner_recover_state(p->scanner, backup_state);
        LX_CALLBACK_CALL1(prefix_expr_list, prefix_expr,
            self, prefix_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(prefix_expr_node);

    return -1;
}



static int expr(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(assign_expr_node);
    if (assign_expr(p, assign_expr_node) == 0) {
        LX_CALLBACK_CALL1(expr, assign_expr,
            self, assign_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(assign_expr_node);
    return -1;
}

static int assign_expr(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    NEW_SYNTAX_NODE(prefix_expr_list_node);
    if (prefix_expr_list(p, prefix_expr_list_node) == 0) {
        NEW_SYNTAX_NODE(assign_op_node);
        if (assign_op(p, assign_op_node) == 0) {
            NEW_SYNTAX_NODE(expr_list_node);
            if (expr_list(p, expr_list_node) == 0) {
                LX_CALLBACK_CALL3(assign_expr, prefix_expr_list, assign_op, expr_list,
                    self, prefix_expr_list_node, assign_op_node, expr_list_node);
                return 0;
            }
            FREE_SYNTAX_NODE(expr_list_node);
        }
        FREE_SYNTAX_NODE(assign_op_node);
    }
    FREE_SYNTAX_NODE(prefix_expr_list_node);

    lx_token_scanner_recover_state(p->scanner, backup_state);
    NEW_SYNTAX_NODE(logical_expr_node);
    if (logical_expr(p, logical_expr_node) == 0) {
        LX_CALLBACK_CALL1(assign_expr, logical_expr,
            self, logical_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(logical_expr_node);

    return -1;
}

static int logical_expr(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_NOT)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(not_node, CURR(p));
        NEW_SYNTAX_NODE(compare_expr_node);
        if (compare_expr(p, compare_expr_node) == 0) {
            LX_CALLBACK_CALL2(logical_expr, NOT, compare_expr,
                self, not_node, compare_expr_node);
            return 0;
        }
        FREE_SYNTAX_NODE(compare_expr_node);
        FREE_SYNTAX_NODE(not_node);
    }
    lx_token_scanner_recover_state(p->scanner, backup_state);
    NEW_SYNTAX_NODE(compare_expr_node);
    if (compare_expr(p, compare_expr_node) == 0) {
        NEW_SYNTAX_NODE(logical_op_node);
        int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (logical_op(p, logical_op_node) == 0) {
            NEW_SYNTAX_NODE(logical_expr_node);
            if (logical_expr(p, logical_expr_node) == 0) {
                LX_CALLBACK_CALL3(logical_expr, compare_expr, logical_op, logical_expr,
                    self, compare_expr_node, logical_op_node, logical_expr_node);
                return 0;
            }
            FREE_SYNTAX_NODE(logical_expr_node);
        }
        lx_token_scanner_recover_state(p->scanner, backup_state);
        FREE_SYNTAX_NODE(logical_op_node);
        LX_CALLBACK_CALL1(logical_expr, compare_expr,
            self, compare_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(compare_expr_node);

    return -1;
}

static int compare_expr(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(addtive_expr_node);
    if (addtive_expr(p, addtive_expr_node) == 0) {
        NEW_SYNTAX_NODE(compare_op_node);
        int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (compare_op(p, compare_op_node) == 0) {
            NEW_SYNTAX_NODE(compare_expr_node);
            if (compare_expr(p, compare_expr_node) == 0) {
                LX_CALLBACK_CALL3(compare_expr, addtive_expr, compare_op, compare_expr,
                    self, addtive_expr_node, compare_op_node, compare_expr_node);
                return 0;
            }
            FREE_SYNTAX_NODE(compare_expr_node);
        }
        lx_token_scanner_recover_state(p->scanner, backup_state);
        FREE_SYNTAX_NODE(compare_op_node);
        LX_CALLBACK_CALL1(compare_expr, addtive_expr,
            self, addtive_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(addtive_expr_node);

    return -1;
}

static int addtive_expr(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(multiply_expr_node);
    if (multiply_expr(p, multiply_expr_node) == 0) {
        NEW_SYNTAX_NODE(addtive_op_node);
        int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (addtive_op(p, addtive_op_node) == 0) {
            NEW_SYNTAX_NODE(addtive_expr_node);
            if (addtive_expr(p, addtive_expr_node) == 0) {
                LX_CALLBACK_CALL3(addtive_expr, multiply_expr, addtive_op, addtive_expr,
                    self, multiply_expr_node, addtive_op_node, addtive_expr_node);
                return 0;
            }
            FREE_SYNTAX_NODE(addtive_expr_node);
        }
        lx_token_scanner_recover_state(p->scanner, backup_state);
        FREE_SYNTAX_NODE(addtive_op_node);
        LX_CALLBACK_CALL1(addtive_expr, multiply_expr,
            self, multiply_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(multiply_expr_node);

    return -1;
}

static int multiply_expr(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(prefix_expr_node);
    if (prefix_expr(p, prefix_expr_node) == 0) {
        NEW_SYNTAX_NODE(multiply_op_node);
        int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (multiply_op(p, multiply_op_node) == 0) {
            NEW_SYNTAX_NODE(multiply_expr_node);
            if (multiply_expr(p, multiply_expr_node) == 0) {
                LX_CALLBACK_CALL3(multiply_expr, prefix_expr, multiply_op, multiply_expr,
                    self, prefix_expr_node, multiply_op_node, multiply_expr_node);
                return 0;
            }
            FREE_SYNTAX_NODE(multiply_expr_node);
        }
        lx_token_scanner_recover_state(p->scanner, backup_state);
        FREE_SYNTAX_NODE(multiply_op_node);
        LX_CALLBACK_CALL1(multiply_expr, prefix_expr,
            self, prefix_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(prefix_expr_node);

    return -1;
}

static int prefix_expr(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    NEW_SYNTAX_NODE(prefix_op_node);
    if (prefix_op(p, prefix_op_node) == 0) {
        NEW_SYNTAX_NODE(suffix_expr_node);
        if (suffix_expr(p, suffix_expr_node) == 0) {
            LX_CALLBACK_CALL2(prefix_expr, prefix_op, suffix_expr,
                self, prefix_op_node, suffix_expr_node);
            return 0;
        }
        FREE_SYNTAX_NODE(suffix_expr_node);
    }
    FREE_SYNTAX_NODE(prefix_op_node);
    lx_token_scanner_recover_state(p->scanner, backup_state);
    NEW_SYNTAX_NODE(suffix_expr_node);
    if (suffix_expr(p, suffix_expr_node) == 0) {
        LX_CALLBACK_CALL1(prefix_expr, suffix_expr,
            self, suffix_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(suffix_expr_node);

    return -1;
}

static int suffix_expr(lx_parser *p, lx_syntax_node *self)
{
    NEW_SYNTAX_NODE(single_expr_node);
    if (single_expr(p, single_expr_node) == 0) {
        NEW_SYNTAX_NODE(suffix_op_node);
        if (suffix_op(p, suffix_op_node) == 0) {
            LX_CALLBACK_CALL2(suffix_expr, single_expr, suffix_op,
                self, single_expr_node, suffix_op_node);
            return 0;
        }
        FREE_SYNTAX_NODE(suffix_op_node);
        LX_CALLBACK_CALL1(suffix_expr, single_expr,
            self, single_expr_node);
        return 0;
    }
    FREE_SYNTAX_NODE(single_expr_node);

    return -1;
}

static int single_expr(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, '(')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sl_node, CURR(p));
        NEW_SYNTAX_NODE(expr_node);
        if (expr(p, expr_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, ')')) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(sr_node, CURR(p));
                LX_CALLBACK_CALL3(single_expr, SL, expr, SR,
                    self, sl_node, expr_node, sl_node);
                return 0;
            }
        }
        FREE_SYNTAX_NODE(expr_node);
        FREE_SYNTAX_NODE(sl_node);
    }
    lx_token_scanner_recover_state(p->scanner, backup_state);
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_NIL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(nil_node, CURR(p));
        LX_CALLBACK_CALL1(single_expr, NIL,
            self, nil_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_FALSE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(false_node, CURR(p));
        LX_CALLBACK_CALL1(single_expr, FALSE,
            self, false_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_TRUE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(true_node, CURR(p));
        LX_CALLBACK_CALL1(single_expr, TRUE,
            self, true_node);
        return 0;
    }
    NEW_SYNTAX_NODE(immediate_node);
    if (immediate(p, immediate_node) == 0) {
        LX_CALLBACK_CALL1(single_expr, immediate,
            self, immediate_node);
        return 0;
    }
    FREE_SYNTAX_NODE(immediate_node);
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_IDENTIFIER)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(identifier_node, CURR(p));
        LX_CALLBACK_CALL1(single_expr, IDENTIFIER,
            self, identifier_node);
        return 0;
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}
static int suffix_op(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, '(')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sl_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ')')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(sr_node, CURR(p));
            NEW_SYNTAX_NODE(suffix_op_node);
            if (suffix_op(p, suffix_op_node) == 0) {
                LX_CALLBACK_CALL3(suffix_op, SL, SR, suffix_op,
                    self, sl_node, sr_node, suffix_op_node);
                return 0;
            }
            FREE_SYNTAX_NODE(suffix_op_node);
            LX_CALLBACK_CALL2(suffix_op, SL, SR,
                self, sl_node, sr_node);
            return 0;
        }
        NEW_SYNTAX_NODE(expr_list_node);
        //int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (expr_list(p, expr_list_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, ')')) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(sr_node, CURR(p));
                NEW_SYNTAX_NODE(suffix_op_node);
                if (suffix_op(p, suffix_op_node) == 0) {
                    LX_CALLBACK_CALL4(suffix_op, SL, expr_list, SR, suffix_op,
                        self, sl_node, expr_list_node, sr_node, suffix_op_node);
                    return 0;
                }
                FREE_SYNTAX_NODE(suffix_op_node);
                LX_CALLBACK_CALL3(suffix_op, SL, expr_list, SR,
                    self, sl_node, expr_list_node, sr_node);
                return 0;
            }
        }
        // actually, we don't need this, for the reason that there are only three situation((, [, .), when we came
        // here, we have no chance to go to other situations.
        //lx_token_scanner_recover_state(p->scanner, backup_state);
        FREE_SYNTAX_NODE(expr_list_node);
        FREE_SYNTAX_NODE(sl_node);
    }
    if (NEXT_TYPE_EQUAL(p, '[')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(ml_node, CURR(p));
        NEW_SYNTAX_NODE(expr_node);
        if (expr(p, expr_node) == 0) {
            if (NEXT_TYPE_EQUAL(p, ']')) {
                GOTO_NEXT(p);
                NEW_SYNTAX_NODE_T(mr_node, CURR(p));
                NEW_SYNTAX_NODE(suffix_op_node);
                if (suffix_op(p, suffix_op_node) == 0) {
                    LX_CALLBACK_CALL4(suffix_op, ML, expr, MR, suffix_op,
                        self, ml_node, expr_node, mr_node, suffix_op_node);
                    return 0;
                }
                FREE_SYNTAX_NODE(suffix_op_node);
                LX_CALLBACK_CALL3(suffix_op, ML, expr, MR,
                    self, ml_node, expr_node, mr_node);
                return 0;
            }
        }
        FREE_SYNTAX_NODE(expr_node);
        FREE_SYNTAX_NODE(ml_node);
    }
    if (NEXT_TYPE_EQUAL(p, '.')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(dot_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, LX_TOKEN_IDENTIFIER)) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(identifier_node, CURR(p));
            NEW_SYNTAX_NODE(suffix_op_node);
            if (suffix_op(p, suffix_op_node) == 0) {
                LX_CALLBACK_CALL3(suffix_op, DOT, IDENTIFIER, suffix_op,
                    self, dot_node, identifier_node, suffix_op_node);
                return 0;
            }
            FREE_SYNTAX_NODE(suffix_op_node);
            LX_CALLBACK_CALL2(suffix_op, DOT, IDENTIFIER,
                self, dot_node, identifier_node);
            return 0;
        }
        FREE_SYNTAX_NODE(dot_node);
    }

    lx_token_scanner_recover_state(p->scanner, backup_state);
    return -1;
}


static int immediate(lx_parser *p, lx_syntax_node *self)
{
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
    return -1;
}
static int object_immediate(lx_parser *p, lx_syntax_node *self)
{
    int backup_state = lx_token_scanner_get_curr_state(p->scanner);

    if (NEXT_TYPE_EQUAL(p, '{')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(bl_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, '}')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(br_node, CURR(p));
            LX_CALLBACK_CALL2(object_immediate, BL, BR,
                self, bl_node, br_node);
            return 0;
        }
        NEW_SYNTAX_NODE(object_immediate_item_list_node);
        if (object_immediate_item_list(p, object_immediate_item_list_node) == 0) {
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
        if (NEXT_TYPE_EQUAL(p, ',')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(comma_node, CURR(p));
            NEW_SYNTAX_NODE(object_immediate_item_list_node);
            if (object_immediate_item_list(p, object_immediate_item_list_node) == 0) {
                LX_CALLBACK_CALL3(object_immediate_item_list, object_immediate_item, COMMA, object_immediate_item_list,
                    self, object_immediate_item_node, comma_node, object_immediate_item_list_node);
                return 0;
            }
            FREE_SYNTAX_NODE(object_immediate_item_list_node);
            FREE_SYNTAX_NODE(comma_node);
        }
        LX_CALLBACK_CALL1(object_immediate_item_list, object_immediate_item,
            self, object_immediate_item_node);
        return 0;
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
                LX_CALLBACK_CALL3(object_immediate_item, IDENTIFIER, COLON, object_immediate_item_value, 
                    self, identifier_node, colon_node, object_immediate_item_value_node);
                return 0;
            }
            FREE_SYNTAX_NODE(object_immediate_item_value_node);
            FREE_SYNTAX_NODE(colon_node);
        }
        FREE_SYNTAX_NODE(identifier_node);
    }
    lx_token_scanner_recover_state(p->scanner, backup_state);
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_STRING_IMMEDIATE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(string_immediate_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ':')) {
            GOTO_NEXT(p);
           NEW_SYNTAX_NODE_T(colon_node, CURR(p));
           NEW_SYNTAX_NODE(object_immediate_item_value_node);
           if (object_immediate_item_value(p, object_immediate_item_value_node) == 0) {
               LX_CALLBACK_CALL3(object_immediate_item, STRING_IMMEDIATE, COLON, object_immediate_item_value,
                   self, string_immediate_node, colon_node, object_immediate_item_value_node);
               return 0;
           }
            FREE_SYNTAX_NODE(object_immediate_item_value_node);
            FREE_SYNTAX_NODE(colon_node);
        }
        FREE_SYNTAX_NODE(string_immediate_node);
    }
    lx_token_scanner_recover_state(p->scanner, backup_state);
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_NUMBER_IMMEDIATE)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(number_immediate_node, CURR(p));
        if (NEXT_TYPE_EQUAL(p, ':')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(colon_node, CURR(p));
            NEW_SYNTAX_NODE(object_immediate_item_value_node);
            if (object_immediate_item_value(p, object_immediate_item_value_node) == 0) {
                LX_CALLBACK_CALL3(object_immediate_item, NUMBER_IMMEDIATE, COLON, object_immediate_item_value,
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
        LX_CALLBACK_CALL1(object_immediate_item_value, immediate,
            self, immediate_node);
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
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_IDENTIFIER)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(identifier_node, CURR(p));
        int backup_state = lx_token_scanner_get_curr_state(p->scanner);
        if (NEXT_TYPE_EQUAL(p, ',')) {
            GOTO_NEXT(p);
            NEW_SYNTAX_NODE_T(comma_node, CURR(p));
            NEW_SYNTAX_NODE(identifier_list_node);
            if (identifier_list(p, identifier_list_node) == 0) {
                LX_CALLBACK_CALL3(identifier_list, IDENTIFIER, COMMA, identifier_list,
                    self, identifier_node, comma_node, identifier_list_node);
                return 0;
            }
            FREE_SYNTAX_NODE(identifier_list_node);
            FREE_SYNTAX_NODE(comma_node);
        }
        lx_token_scanner_recover_state(p->scanner, backup_state);
        LX_CALLBACK_CALL1(identifier_list, IDENTIFIER, self, identifier_node);
        return 0;
    }
    return -1;
}


static int assign_op(lx_parser *p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, '=')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(eql_node, CURR(p));
        LX_CALLBACK_CALL1(assign_op, EQL, self, eql_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_ADD_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(add_eql_node, CURR(p));
        LX_CALLBACK_CALL1(assign_op, ADD_EQL, self, add_eql_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_SUB_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sub_eql_node, CURR(p));
        LX_CALLBACK_CALL1(assign_op, SUB_EQL, self, sub_eql_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_MUL_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(mul_eql_node, CURR(p));
        LX_CALLBACK_CALL1(assign_op, MUL_EQL, self, mul_eql_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_DIV_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(div_eql_node, CURR(p));
        LX_CALLBACK_CALL1(assign_op, DIV_EQL, self, div_eql_node);
        return 0;
    }
    return -1;
}

static int logical_op(lx_parser *p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_AND)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(and_node, CURR(p));
        LX_CALLBACK_CALL1(logical_op, AND, self, and_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_OR)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(or_node, CURR(p));
        LX_CALLBACK_CALL1(logical_op, OR, self, or_node);
        return 0;
    }
    return -1;
}

static int compare_op(lx_parser *p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, '<')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(less_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LESS, self, less_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, '>')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(greater_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, GREATER, self, greater_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_LESS_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(less_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, LESS_EQL, self, less_eql_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_GREATER_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(greater_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, GREATER_EQL, self, greater_eql_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_EQL_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(eql_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, EQL_EQL, self, eql_eql_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, LX_TOKEN_NOT_EQL)) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(not_eql_node, CURR(p));
        LX_CALLBACK_CALL1(compare_op, NOT_EQL, self, not_eql_node);
        return 0;
    }
    return -1;
}

static int addtive_op(lx_parser * p, lx_syntax_node * self)
{
    if (NEXT_TYPE_EQUAL(p, '+')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(add_node, CURR(p));
        LX_CALLBACK_CALL1(addtive_op, ADD, self, add_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, '-')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sub_node, CURR(p));
        LX_CALLBACK_CALL1(addtive_op, SUB, self, sub_node);
        return 0;
    }
    return -1;
}

static int multiply_op(lx_parser *p, lx_syntax_node* self)
{
    if (NEXT_TYPE_EQUAL(p, '*')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(mul_node, CURR(p));
        LX_CALLBACK_CALL1(multiply_op, MUL, self, mul_node);
        return 0;
    }
    if (NEXT_TYPE_EQUAL(p, '/')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(div_node, CURR(p));
        LX_CALLBACK_CALL1(multiply_op, DIV, self, div_node);
        return 0;
    }
    return -1;
}

static int prefix_op(lx_parser * p, lx_syntax_node *self)
{
    if (NEXT_TYPE_EQUAL(p, '-')) {
        GOTO_NEXT(p);
        NEW_SYNTAX_NODE_T(sub_node, CURR(p));
        LX_CALLBACK_CALL1(prefix_op, SUB, self, sub_node);
        return 0;
    }
    return -1;
}
