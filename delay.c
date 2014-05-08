#include <stdint.h>
#include <xdc/std.h>

#include <xdc/cfg/global.h>

Void delay(UInt32 iters)
{
    UInt32 i = 0;
    while (i < iters)
        i++;
}

