#include "utils.h"


unsigned short conv_order_short(unsigned short i)
{
    return (i<<8) | (i>>8);
}
