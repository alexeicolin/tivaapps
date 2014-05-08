#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <ti/drivers/GPIO.h> 
#include <ti/sysbios/knl/Task.h>

#include <xdc/cfg/global.h>

#include "EK_TM4C123GXL.h"
#include "delay.h"

static Task_Handle ledTask;

Void ledTaskFxn(UArg arg1, UArg arg2)
{
    GPIO_write(EK_TM4C123GXL_LED_RED, EK_TM4C123GXL_LED_ON);
    mediumDelay();
}

Int app(Int argc, Char* argv[])
{
    Task_Params taskParams;
    Error_Block eb;

    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.priority = 3;
    ledTask = Task_create(ledTaskFxn, &taskParams, &eb);
    if (ledTask == NULL) {
        System_abort("Task create failed");
    }

    return 0;
}
