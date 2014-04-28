/* app_timer: hwi timer tick handler
 *
 * For testing timer in different run modes.
 * */

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <ti/uia/sysbios/LoggerIdle.h>
#include <ti/sysbios/family/arm/lm4/Timer.h>

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

Timer_Handle timer;

static Void hwiFxn(UArg arg)
{
    Log_print0(Diags_INFO, "Tick");
    LoggerIdle_flush();
}

Int app(Int argc, Char* argv[])
{
    Timer_Params timerParams;
    Error_Block eb;

    Error_init(&eb);

    System_printf("Creating the timer handler...\n");

    Timer_Params_init(&timerParams);

/* For EDGECOUNT mode, the CCP pin for this timer should be selected in
 * the corresponding GPIO port mux. */
#define TIMER_ID 1

    /*
    timerParams.period = 500000;
    timerParams.periodType = Timer_PeriodType_MICROSECS;
    timerParams.runMode = Timer_RunMode_CONTINUOUS;
    */
    timerParams.period = 500000;
    timerParams.periodType = Timer_PeriodType_MICROSECS;
    timerParams.extFreq.lo = 0;
    timerParams.extFreq.lo = 10000; /* Hz */
    timerParams.runMode = Timer_RunMode_EDGECOUNT;

    timerParams.startMode = Timer_StartMode_AUTO;
    timerParams.altclk = 0;
    timer = Timer_create(TIMER_ID, hwiFxn, &timerParams, &eb);
    if (timer == NULL) {
        System_abort("Timer create failed");
    }

    return 0;
}
