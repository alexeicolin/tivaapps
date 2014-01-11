#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* #include <ti/sysbios/hal/Timer.h> */

/* LM4-specific timer to be able to specify altclk parameter */
//#define ti_sysbios_family_arm_lm4_Timer__nolocalnames
#include <ti/sysbios/family/arm/lm4/Timer.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Queue.h>

#include <xdc/cfg/global.h>


typedef struct {
    Queue_Elem elem;
    UInt32 timeout;
    UInt32 period;
    Void (*fxn)(UArg);
    UArg arg;
} Clock_Object;

Clock_Object clk1, clk2;
Timer_Handle timer;
Semaphore_Handle sem;
Swi_Handle swi;
Task_Handle task;
Queue_Handle clockQueue;

/* Here on Timer interrupt */
static Void hwiFxn(UArg arg)
{
    Swi_post(swi);
}

/* Swi thread to handle Timer interrupt */
static Void swiFxn(UArg arg1, UArg arg2)
{
    Queue_Elem *elem;
    Clock_Object *obj;

    System_flush();

    /* point to first clock object in the clockQueue */
    elem = Queue_next((Queue_Elem *)clockQueue);
    /* service all the Clock Objects in the clockQueue */
    while (elem != (Queue_Elem *)clockQueue) {
        obj = (Clock_Object *)elem;
        /* decrement the timeout counter */
        obj->timeout -= 1;
        /* if period has expired, refresh the timeout
         * value and invoke the clock func */
        if (obj->timeout == 0) {
            obj->timeout = obj->period;
            (obj->fxn)(obj->arg);
        }
        /* advance to next clock object in clockQueue */
        elem = Queue_next(elem);
    }
}
/* Task thread pends on Semaphore posted by Clock thread */
static Void taskFxn(UArg arg1, UArg arg2)
{
    System_printf("In taskFxn pending on Sempahore.\n");
    Semaphore_pend(sem, BIOS_WAIT_FOREVER);
    System_printf("In taskFxn returned from Sempahore.\n");
    System_exit(0);
}
/* First Clock function, invoked every 5 timer interrupts */
static Void clk1Fxn(UArg arg)
{
    System_printf("In clk1Fxn, arg = %d.\n", arg);
    clk1.arg++;
}

/* Second Clock function, invoked every 20 timer interrupts */
static Void clk2Fxn(UArg sem)
{
    //System_printf("In clk2Fxn, posting Semaphore.\n");
    Semaphore_post((Semaphore_Object *)sem);
}

Int app(Int argc, Char* argv[])
{
    Timer_Params timerParams;
    Task_Params taskParams;
    Error_Block eb;

    Error_init(&eb);

    Timer_Params_init(&timerParams);
    Task_Params_init(&taskParams);

    /* Create a Swi with default priority (15).
     * Swi handler is 'swiFxn' which runs as a Swi thread. */
    swi = Swi_create(swiFxn, NULL, &eb);
    if (swi == NULL) {
        System_abort("Swi create failed");
    }

    /* Create a Task with priority 3.
     * Task function is 'taskFxn' which runs as a Task thread. */
    taskParams.priority = 3;
    task = Task_create(taskFxn, &taskParams, &eb);
    if (task == NULL) {
        System_abort("Task create failed");
    }

    /* Create a binary Semaphore for example task to pend on */
    sem = Semaphore_create(0, NULL, &eb);
    if (sem == NULL) {
        System_abort("Semaphore create failed");
    }

    /* Create a Queue to hold the Clock Objects on */
    clockQueue = Queue_create(NULL, &eb);
    if (clockQueue == NULL) {
        System_abort("Queue create failed");
    }

    /* setup clk1 to go off every 5 timer interrupts. */
    clk1.fxn = clk1Fxn;
    clk1.period = 5;
    clk1.timeout = 5;
    clk1.arg = 1;

    /* add the Clock object to the clockQueue */
    Queue_put(clockQueue, &clk1.elem);

    /* setup clk2 to go off every 20 timer interrupts. */
    clk2.fxn = clk2Fxn;
    clk2.period = 20;
    clk2.timeout = 20;
    clk2.arg = (UArg)sem;

    /* add the Clock object to the clockQueue */
    Queue_put(clockQueue, &clk2.elem);

    /* Configure a periodic interrupt using any available Timer
     * with a 1000 microsecond (1ms) interrupt period.
     *
     * The Timer interrupt will be handled by 'hwiFxn' which
     * will run as a Hwi thread.
     */
    timerParams.period = 1000000;
    timerParams.startMode = Timer_StartMode_AUTO;
    timerParams.altclk = 1;
    timer = Timer_create(Timer_ANY, hwiFxn, &timerParams, &eb);
    if (timer == NULL) {
        System_abort("Timer create failed");
    }

    return 0;
}

