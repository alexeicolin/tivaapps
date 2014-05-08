#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/knl/Task.h>

#include <xdc/cfg/global.h>

Task_Handle busyTask;

static Void busyTaskFxn(UArg arg1, UArg arg2)
{
    while(1);
}

Int app(Int argc, Char* argv[])
{
    Error_Block eb;
    Task_Params taskParams;

    Error_init(&eb);

    System_printf("About to be busy!\n");

    Task_Params_init(&taskParams);
    taskParams.priority = 3;
    busyTask = Task_create(busyTaskFxn, &taskParams, &eb);
    if (busyTask == NULL) {
        System_abort("Task create failed");
    }

    return 0;
}
