#include "./opcode.h"
#include "./base.h"


bool lx_opcode_is_label(unsigned char type)
{
    return 1 <= type && type <= 9;
}