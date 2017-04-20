#ifndef __LX_PARSER__H_
#define __LX_PARSER__H_

#include "./base.h"
#include "./mem.h"
#include "./opcode.h"

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
    LX_TOKEN_END,
    LX_TOKEN_LOCAL,

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

    LX_TOKEN_ERROR,  // error happened!
    LX_TOKEN_NO_MORE // it can remove the check before dereferencing lx_token_next
                     // without this, we may use this:
                     //      if(lx_token_next(scanner) && lx_token_next(scanner)->type == LX_TOKEN_IF){
                     //          ...
                     //      }
                     // but now, lx_token_next(scanner) would return LX_TOKEN_NO_MORE when next one is out of range.
};

typedef struct
{
    int type;
    char *text;
    int text_len;
    int linenum;
} lx_token;

char* lx_helper_dump_token(lx_token *token, char * outstr);

typedef struct
{
    int token_number;
    lx_token **tokens;

    int curr; /* point to current token */

    int tokens_capacity;

    int raw_source_code_length;
    char *raw_source_code;
} lx_token_scanner;

typedef struct lx_parser
{
    lx_token_scanner *scanner;
#if LX_USING_STACK_ALLOCATOR_IN_PARSER
    lx_stack_allocator * stack_allocator;
#endif
    lx_opcodes* opcodes;
} lx_parser;


/* helper macro for create/delete syntax_node */

#if(LX_USING_STACK_ALLOCATOR_IN_PARSER)
# define NEW_SYNTAX_NODE(_node) lx_syntax_node * _node = (lx_syntax_node*)lx_stack_allocator_alloc(p->stack_allocator, sizeof(lx_syntax_node)); lx_syntax_node_init(_node)
#else
# define NEW_SYNTAX_NODE(_node) lx_syntax_node * _node = LX_NEW(lx_syntax_node); lx_syntax_node_init(_node)
#endif
#define NEW_SYNTAX_NODE_T(_node, _token) \
    NEW_SYNTAX_NODE(_node); \
    _node->token = _token

#if(LX_USING_STACK_ALLOCATOR_IN_PARSER)
# define FREE_SYNTAX_NODE(_node) if(_node->opcodes) lx_free(_node->opcodes); lx_stack_allocator_free(p->stack_allocator, _node)
#else
# define FREE_SYNTAX_NODE(_node) if(_node->opcodes) lx_free(_node->opcodes); lx_free(_node)
#endif



/* Interface to generate opcodes */
lx_parser* lx_gen_opcodes(const char* source_code, const int source_code_length);
void lx_delete_parser(lx_parser* p);


#endif // end of __LX_PARSER__H_