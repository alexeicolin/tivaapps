#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/BIOS.h>

#include <xdc/cfg/global.h>

#include <ti/uia/sysbios/LoggerIdle.h>

/*
 *  Use the long names for ti/sysbios/family/arm/m3/Hwi.h, so we don't
 *  have name collisions with ti/sysbios/hal/Hwi.h, which is included
 *  by UART.h.
 */
#define ti_sysbios_family_arm_m3_Hwi__nolocalnames
#include <ti/sysbios/family/arm/m3/Hwi.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include "Board.h"

#include "event_logger.h"
#include "uart_iface.h"

/* Application entry function declaration */
Int app(Int argc, Char* argv[]);

/* Log events and console output go to this UART port */
UART_Iface_Port uartPort = NULL;

/* Called back by both system console and event logger */
Void outputToUART(Char *buf, Int size) {
    UART_Iface_write(uartPort, buf, size);
}

/*
 *  ======== myExceptionHook ========
 *  User exception hook function.  This function will be run on the ISR
 *  stack, so it must run to completion.  This function is called without
 *  any Task or Swi scheduling protection, so it cannot call any functions
 *  that may cause a scheduling operation (eg, Swi_post(), Semaphore_post()).
 */
Void onException(ti_sysbios_family_arm_m3_Hwi_ExcContext *excp)
{
    System_printf("Exception context = 0x%x\n", excp);
    LoggerIdle_flush();
}

Int main(Int argc, Char* argv[])
{
    /* Core configuration parameters (e.g. system clock) are specified in
     * xdctools:ti.platforms.stellaris.Platform */

    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

    if (!(uartPort = UART_Iface_openHandle(Board_UART, FALSE)))
        System_abort("Failed to open UART\n");

    EventLogger_registerOutputFunc(&outputToUART);

    GPIO_write(Board_LED, Board_LED_ON);

    System_printf("Running application...\n");
    System_flush();

    if (app(argc, argv))
        System_abort("Application failed\n");

    System_printf("Starting BIOS...\n");
    System_flush();

    BIOS_start();
    return 0;
}
