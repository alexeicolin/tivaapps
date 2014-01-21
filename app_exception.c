#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

#include <ti/sysbios/knl/Task.h>

#include <xdc/cfg/global.h>

/* Exception handling example (taken from UART Log Idle example) */

Task_Handle task;

/* This task will eventually divide-by-zero */
Void taskFxn(UArg arg0, UArg arg1)
{
    Int    sleepDur = 100;
    Int    id = (Int)arg0;
    Int    count = 40;
    Bits32 t1, t2;

    while (TRUE) {
        Task_sleep(sleepDur);
        /* Benchmark time to do a Log_print2 */
        t1 = Timestamp_get32();
        Log_print4(Diags_USER1, "Task %d awake, count = %d, %d, %d",
                id, count, 0xabcd, 0x1234);
        t2 = Timestamp_get32();

        System_printf("Time to doo Log_print2: %d, count: %d\n", t2 - t1,
                count);

        /* Decrement count so that eventually we will get an exception */
        count--;
        sleepDur = (sleepDur / count) * count;
    }
}

Int app(Int argc, Char* argv[])
{
    Task_Params taskParams;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&taskParams);

    System_printf("Creating a task that will encounter an exception...\n");

    taskParams.priority = 3;
    task = Task_create(taskFxn, &taskParams, &eb);
    if (task == NULL) {
        System_abort("Task create failed");
    }

    return 0;
}
