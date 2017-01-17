#ifndef __LX_CONFIG__H_
#define __LX_CONFIG__H_

/* print parser debug log to stdout */
#define LX_PARSER_DEBUG_LOG 0
/* used to control debug info output */
#define LX_DEBUG 0
/* show lx_malloc, lx_free calling info */
#define LX_PRINT_MALLOC_INFO 1

/* using stack_allocator in parser */
#define LX_USING_STACK_ALLOCATOR_IN_PARSER 1

/* grain of token buff used in toker scanner */
#define LX_CONFIG_TOKEN_GRAIN 1024
#define LX_CONFIG_IDENTIFIER_MAX_LENGTH (1024 * 4 - 1)



#endif // end of __LX_CONFIG__H_