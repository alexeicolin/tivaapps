#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <xdc/cfg/global.h>

/* See datasheet TM4C123GH6PM p136 */
#define HWREG(x) (*((volatile unsigned long *)(x)))
#define ST_BASEADDR 0xE000E000
#define ST_INTNUM 15

#define R_STCTRL    HWREG(ST_BASEADDR+0x10)
#define R_STRELOAD  HWREG(ST_BASEADDR+0x14)
#define R_STCURRENT HWREG(ST_BASEADDR+0x18)

#define S_STCTRL_ENABLED 0
#define M_STCTRL_ENABLED (0x1 << S_STCTRL_ENABLED)
#define V_STCTRL_ENABLED (0x1 << S_STCTRL_ENABLED)
#define S_STCTRL_INTEN 1
#define M_STCTRL_INTEN (0x1 << S_STCTRL_INTEN)
#define V_STCTRL_INTEN (0x1 << S_STCTRL_INTEN)
#define S_STCTRL_CLKSRC 2
#define M_STCTRL_CLKSRC (0x1 << S_STCTRL_CLKSRC)
#define V_STCTRL_CLKSRC_PI_DIV4 (0x0 << S_STCTRL_CLKSRC)
#define V_STCTRL_CLKSRC_SYS (0x1 << S_STCTRL_CLKSRC)

#define R_SYSCTL    HWREG(0xE000ED10)
#define S_SLEEPDEEP 2
#define V_SLEEPDEEP (0x1 << S_SLEEPDEEP)
#define R_DSLPCLKCFG HWREG(0x400FE144)
#define S_DSDIVORIDE 23
#define M_DSDIVORIDE (0x3f << S_DSDIVORIDE)
#define V_DSDIVORIDE_DIV_1 (0x0 << S_DSDIVORIDE)
#define V_DSDIVORIDE_DIV_64 (0x3F << S_DSDIVORIDE)
#define S_DSOSCSRC 4
#define M_DSOSCSRC (0x7 << S_DSOSCSRC)
#define V_DSOSCSRC_MOSC (0x0 << S_DSOSCSRC)
#define V_DSOSCSRC_PIOSC (0x1 << S_DSOSCSRC)
#define V_DSOSCSRC_LFIOSC (0x3 << S_DSOSCSRC)
#define V_DSOSCSRC_HIB (0x7 << S_DSOSCSRC)
#define S_PIOSCPD 1
#define M_PIOSCPD (0x1 << S_PIOSCPD)
#define V_PIOSCPD_POWERON (0 << S_PIOSCPD)
#define V_PIOSCPD_POWEROFF (1 << S_PIOSCPD)


Hwi_Handle stHwi;

Void systickHandler(UArg arg)
{
    Log_print0(Diags_INFO, "Alarm!");
}

Void idleFunc()
{
    __asm__(" wfi");
}


/* This is an example suggesting that the behavior of the SysTick
 * timer while in deep sleep is not as expected. Its counting rate
 * seems to slow down while asleep, despite being supposedly clocked
 * by a the Precision Internal Oscillator (PI_DIV4) which is unaffected
 * by the clock changes due to sleep states. The slow down in counting
 * rate is expected when using the system clock as the clock source, but not
 * when using PI_DIV4. The general-purpose timers, which can only be clocked by
 * the system clock do experience the slowdown, as expected.
 */
Int app(Int argc, Char* argv[])
{
    Hwi_Params hwiParams;
    Error_Block eb;
    UInt32 val;

    System_printf("Configuring sleep state...\n");

    val = R_DSLPCLKCFG & ~(M_DSDIVORIDE | M_DSOSCSRC | M_PIOSCPD);
    val |= V_DSDIVORIDE_DIV_1 | V_PIOSCPD_POWERON;
    val |= V_DSOSCSRC_PIOSC;
    /* val |= V_DSOSCSRC_LFIOSC; */
    R_DSLPCLKCFG = val;
    R_SYSCTL |= V_SLEEPDEEP;

    System_printf("Setting up alarm...\n");

    val = R_STCTRL;
    val &= ~(M_STCTRL_INTEN | M_STCTRL_CLKSRC | M_STCTRL_ENABLED);
    val |= V_STCTRL_INTEN | V_STCTRL_ENABLED;
    /* val |= V_STCTRL_CLKSRC_SYS; */
    val |= V_STCTRL_CLKSRC_PI_DIV4;
    R_STCTRL = val;

    Error_init(&eb);
    Hwi_Params_init(&hwiParams);
    stHwi = Hwi_create(ST_INTNUM, systickHandler, &hwiParams, &eb);
    if (!stHwi) {
        System_abort("Hwi create failed");
    }

    Hwi_clearInterrupt(ST_INTNUM);
    Hwi_enableInterrupt(ST_INTNUM);

    R_STCURRENT = 0;
    R_STRELOAD = 100000; /* 0.1 * 10^6 counts @ 4 MHz = 25 msec */

    return 0;
}
