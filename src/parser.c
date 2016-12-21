#include "./parser.h"
#include "./base.h"
#include "./mem.h"
#include "./config.h"


//
// Token Scanner - internal use
//

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
    //    return NULL;
    //else
    //return s->tokens[s->curr + 1];
    return lx_token_nextN(s, 1);
}
lx_token* lx_token_nextN(lx_token_scanner *s, int n)
{
    if (s->curr >= s->token_number - n) 
        return NULL;
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
int lx_token_scanner_goto_state(lx_token_scanner *s, int new_state) {
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