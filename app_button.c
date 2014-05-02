#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

#include <ti/drivers/GPIO.h> 
#include <xdc/cfg/global.h>

#include "EK_TM4C123GXL.h"

/* #define PRINT_LOGS */

Void awakeIdleFxn(UArg arg1, UArg arg2)
{
    /* Flip the LEDs to indicate that we've been awake since
     * going to sleep in the Power.c (which turned these LEDs
     * on in the opposite arrangement) */
    GPIO_write(EK_TM4C123GXL_LED_GREEN, EK_TM4C123GXL_LED_OFF);
    GPIO_write(EK_TM4C123GXL_LED_BLUE, EK_TM4C123GXL_LED_ON);
}

Void gpioButton1Fxn()
{
    /* Generate an interrupt */

#ifdef PRINT_LOGS
    Log_print0(Diags_INFO, "INT Button 1");
#endif
    GPIO_write(EK_TM4C123GXL_LED_RED, EK_TM4C123GXL_LED_ON);
    GPIO_clearInt(EK_TM4C123GXL_GPIO_SW1);
}

Int app(Int argc, Char* argv[]) {
    System_printf("Press btn1 to fire an interrupt and btn 2 to wakeup...\n");
    return 0;
}
