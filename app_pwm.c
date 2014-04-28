#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

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

static Void enablePwm()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // For this example T1CCP1 is used with port B pin 5.
    // The actual port and pins used may be different on your part, consult
    // the data sheet for more information.
    // GPIO port B needs to be enabled so these pins can be used.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    // Configure the GPIO pin muxing for the Timer/CCP function.
    GPIOPinConfigure(GPIO_PB5_T1CCP1);

    // Configure the ccp settings for CCP pin.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_5);

    // Configure Timer1B as a 16-bit periodic timer.
    TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM);

    // Set the Timer1B load value to 50000.  For this example a 66% duty cycle
    // PWM signal will be generated.  From the load value (i.e. 50000) down to
    // match value (set below) the signal will be high.  From the match value
    // to 0 the timer will be low.
    TimerLoadSet(TIMER1_BASE, TIMER_B, 8000);

    // Set the Timer1B match value to load value / 2.
    TimerMatchSet(TIMER1_BASE, TIMER_B,
                  TimerLoadGet(TIMER1_BASE, TIMER_B) / 2);

    TimerEnable(TIMER1_BASE, TIMER_B);

}

Int app(Int argc, Char* argv[])
{
    System_printf("Hello world!\n");
    enablePwm();
    System_printf("PWM enabled\n");
    return 0;
}
