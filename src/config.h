#ifndef __LX_CONFIG__H_
#define __LX_CONFIG__H_

/* used to control debug info output */
#define LX_DEBUG 1
/* print parser debug log to stdout(only function when LX_DEBUG is set to 1) */
#define LX_PARSER_DEBUG_LOG 1
/* show lx_malloc, lx_free calling info */
#define LX_PRINT_MALLOC_INFO 0

/* using stack_allocator in parser */
#define LX_USING_STACK_ALLOCATOR_IN_PARSER 0

/* grain of token buff used in toker scanner */
#define LX_CONFIG_TOKEN_GRAIN 1024
#define LX_CONFIG_IDENTIFIER_MAX_LENGTH (1024 * 4 - 1)



#endif // end of __LX_CONFIG__H_