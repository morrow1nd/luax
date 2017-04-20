#ifndef __LX_CONFIG__H_
#define __LX_CONFIG__H_


#ifdef _DEBUG
# define LX_DEBUG 1
#else
# define LX_DEBUG 0
#endif

/* print parser debug log to stdout(only function when LX_DEBUG is set to 1) */
#define LX_PARSER_DEBUG_LOG 0
/* enable VM debug: assert */
#define LX_VM_DEBUG 0
/* show opcodes before run VM */
#define LX_VM_OPCODE_SHOW 0
/* show lx_malloc, lx_free calling info */
#define LX_PRINT_MALLOC_INFO 0
/* record memory usage */
#define LX_MALLOC_STATISTICS 0

/* using stack_allocator in parser */
#define LX_USING_STACK_ALLOCATOR_IN_PARSER 0 /* todo: we need to check whether we can use stack allocator */

/* grain of token buff used in toker scanner */
#define LX_CONFIG_TOKEN_GRAIN 1024
#define LX_CONFIG_IDENTIFIER_MAX_LENGTH (1024 * 4 - 1)


#endif // !__LX_CONFIG__H_