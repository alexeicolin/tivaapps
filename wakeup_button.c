#include <stdint.h>
#include <xdc/std.h>

#include <xdc/cfg/global.h>

Void gpioButton2Fxn()
{
    /* Safety: debugger has trouble connecting when program is (often) in deep
     * sleep state that's clocked by PI or LFI. This should open up a chance
     * for the debugger.
     */
    while (1);
}
