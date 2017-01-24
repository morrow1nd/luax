#ifndef __LUAX__H_
#define __LUAX__H_


#include "base.h"
#include "mem.h"
#include "opcode.h"
#include "parser.h"
#include "parser_callback.h"
#include "vm.h"


#ifndef LX_NO_STANDARD_LIBRARY
#include "standard_lib/lio.h"
#include "standard_lib/lmath.h"
#include "standard_lib/lstring.h"
#include "standard_lib/ltable.h"
#endif

#endif // !__LUAX__H_