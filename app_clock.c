/* app_clock: periodic clock event handler
 *
 * For testing the Clock module driven with timers in different timer run
 * modes.
 * */

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <ti/uia/sysbios/LoggerIdle.h>
#include <ti/sysbios/knl/Clock.h>

#include <xdc/cfg/global.h>

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

Clock_Handle clk1;

static Void clk1Fxn(UArg arg)
{
    Log_print0(Diags_INFO, "clk tick");
    LoggerIdle_flush();
}

Int app(Int argc, Char* argv[])
{
    Error_Block eb;
    Clock_Params clockParams;

    Error_init(&eb);

    System_printf("Hello world!\n");

    Clock_Params_init(&clockParams);
    /* clockParams.period = 10000; */ /* ticks */
    clockParams.period = 500; /* ticks */
    clockParams.arg = (UArg)42;
    clockParams.startFlag = TRUE;
    clk1 = Clock_create((Clock_FuncPtr)clk1Fxn,
             clockParams.period /* ticks, timeout */, &clockParams, &eb);

    return 0;
}
