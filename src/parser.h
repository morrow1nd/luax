
enum lx_token_type
{
    LX_TOKEN_BREAK = 256,
    LX_TOKEN_CONTINUE,
    LX_TOKEN_RETURN,
    LX_TOKEN_IF,
    LX_TOKEN_THEN,
    LX_TOKEN_END,
    LX_TOKEN_ELSE,
    LX_TOKEN_WHILE,
    LX_TOKEN_FOR,
    LX_TOKEN_FUNCTION,

    LX_TOKEN_NOT,           // not
    LX_TOKEN_AND,           // and
    LX_TOKEN_OR,            // or
    LX_TOKEN_LESS_EQL,      // <=
    LX_TOKEN_GREATER_EQL,   // >=
    LX_TOKEN_EQL_EQL,       // ==
    LX_TOKEN_NOT_EQL,       // !=

    LX_TOKEN_ADD_EQL,       // +=
    LX_TOKEN_SUB_EQL,       // -=
    LX_TOKEN_MUL_EQL,       // *=
    LX_TOKEN_DIV_EQL,       // /=

    LX_TOKEN_NIL,           // nil
    LX_TOKEN_FALSE,         // false
    LX_TOKEN_TRUE,          // true
    LX_TOKEN_IDENTIFIER,
    LX_TOKEN_STRING_IMMEDIATE,
    LX_TOKEN_NUMBER_IMMEDIATE,

    LX_TOKEN_END
};

typedef struct
{
    int type;
    char * text;
} lx_token;

lx_token * lx_token_create(int type, char * text)
{
    lx_token * ret = (lx_token *)lx_malloc(sizeof(lx_token));
    ret.type = type;
    ret.text = text;
    return ret;
}

void lx_token_destory(lx_token * token)
{
    if(!token)
        assert(false);
    free(token->text);
    free(token);
}

typedef struct
{

} lx_token_scanner;

lx_token_scanner * lx_scan_token(char * source_code, int source_code_length)
{
    
}

lx_token * lx_token_next(lx_token_scanner * scanner)
{

}