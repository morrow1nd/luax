#ifndef __LX_PARSER__H_
#define __LX_PARSER__H_

enum lx_token_type
{
    LX_TOKEN_BREAK = 256,
    LX_TOKEN_CONTINUE,
    LX_TOKEN_RETURN,
    LX_TOKEN_IF,
    LX_TOKEN_THEN,
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

    LX_TOKEN_ERROR, // error happened!
    LX_TOKEN_END    // it can remove the check before dereferencing lx_token_next
                    // without this, we may use this:
                    //      if(lx_token_next(scanner) && lx_token_next(scanner)->type == LX_TOKEN_IF){
                    //          ...
                    //      }
                    // but now, lx_token_next(scanner) would return LX_TOKEN_END when next one is out of range.
};

typedef struct
{
    int type;
    char *text;
    int text_len;
    int linenum;
} lx_token;

typedef struct
{
    int token_number;
    lx_token **tokens;

    int curr; // point to current token

    int tokens_capacity;

    int raw_source_code_length;
    char *raw_source_code;
} lx_token_scanner;

typedef struct
{
    // config before parser
    lx_token_scanner *scanner;
} lx_parser;


//
// Interface to generate bytecode
//


#endif // end of __LX_PARSER__H_