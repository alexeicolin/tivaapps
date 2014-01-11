/*
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== EK_TM4C123GXL.c ========
 *  This file is responsible for setting up the board specific items for the
 *  EK_TM4C123GXL board.
 *
 *  The following defines are used to determine which TI-RTOS peripheral drivers
 *  to include:
 *     TI_DRIVERS_GPIO_INCLUDED
 *     TI_DRIVERS_SPI_INCLUDED
 *     TI_DRIVERS_UART_INCLUDED
 *     TI_DRIVERS_WATCHDOG_INCLUDED
 *     TI_DRIVERS_WIFI_INCLUDED
 *  These defines are created when a useModule is done on the driver in the
 *  application's .cfg file. The actual #define is in the application
 *  generated header file that is brought in via the xdc/cfg/global.h.
 *  For example the following in the .cfg file
 *     var GPIO = xdc.useModule('ti.drivers.GPIO');
 *  Generates the following
 *     #define TI_DRIVERS_GPIO_INCLUDED 1
 *  If there is no useModule of ti.drivers.GPIO, the constant is set to 0.
 *
 *  Note: a useModule is generated in the .cfg file via the graphical
 *  configuration tool when the "Add xxx to my configuration" is checked
 *  or "Use xxx" is selected.
 */
#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/ssi.h>
#include <driverlib/udma.h>
#include <driverlib/pin_map.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>

#include "EK_TM4C123GXL.h"

#pragma DATA_ALIGN(EK_TM4C123GXL_DMAControlTable, 1024)
static tDMAControlTable EK_TM4C123GXL_DMAControlTable[32];
static int DMA_count = -1;

/*
 *  ======== EK_TM4C123GXL_errorDMAHwi ========
 */
static Void EK_TM4C123GXL_errorDMAHwi(UArg arg)
{
    System_printf("DMA error code: %d\n", uDMAErrorStatusGet());
    uDMAErrorStatusClear();
    System_abort("DMA error!!");
}

/*
 *  ======== EK_TM4C123GXL_initDMA ========
 */
Void EK_TM4C123GXL_initDMA(Void)
{
    if(DMA_count == -1){
        Hwi_Params  hwiParams;
        Hwi_Handle  hwi;

        Hwi_Params_init(&hwiParams);
        hwi = Hwi_create(INT_UDMAERR, EK_TM4C123GXL_errorDMAHwi, &hwiParams, NULL);
        if (!hwi) {
            System_abort("Couldn't create DMA error hwi");
        }

        SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
        uDMAEnable();
        uDMAControlBaseSet(EK_TM4C123GXL_DMAControlTable);
	DMA_count++;
    }
}

/*
 *  ======== EK_TM4C123GXL_initGeneral ========
 */
Void EK_TM4C123GXL_initGeneral(Void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

#if TI_DRIVERS_GPIO_INCLUDED
#include <ti/drivers/GPIO.h>

/* Callback functions for the GPIO interrupt example. */
#if 0
Void gpioButtonFxn(Void);
Void gpioButtonFxn2(Void);
#endif

/* GPIO configuration structure */
const GPIO_HWAttrs gpioHWAttrs[EK_TM4C123GXL_GPIOCOUNT] = {
    {GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_OUTPUT}, /* EK_TM4C123GXL_LED_RED */
    {GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_OUTPUT}, /* EK_TM4C123GXL_LED_BLUE */
    {GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_OUTPUT}, /* EK_TM4C123GXL_LED_GREEN */
    {GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_INPUT},  /* EK_TM4C123GXL_GPIO_SW1 */
    {GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_INPUT},  /* EK_TM4C123GXL_GPIO_SW2 */
};

#if 0
/* GPIO callback structure to set callbacks for GPIO interrupts */
const GPIO_Callbacks EK_TM4C123GXL_gpioPortFCallbacks = {
    GPIO_PORTF_BASE, INT_GPIOF,
    {gpioButtonFxn2, NULL, NULL, NULL, gpioButtonFxn, NULL, NULL, NULL}
};
#endif

const GPIO_Config GPIO_config[] = {
    {&gpioHWAttrs[0]},
    {&gpioHWAttrs[1]},
    {&gpioHWAttrs[2]},
    {&gpioHWAttrs[3]},
    {&gpioHWAttrs[4]},
    {NULL},
};

/*
 *  ======== EK_TM4C123GXL_initGPIO ========
 */
Void EK_TM4C123GXL_initGPIO(Void)
{
    /* Setup the LED GPIO pins used */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1); /* EK_TM4C123GXL_LED_RED */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); /* EK_TM4C123GXL_LED_GREEN */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3); /* EK_TM4C123GXL_LED_BLUE */

    /* Setup the button GPIO pins used */
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);  /* EK_TM4C123GXL_GPIO_SW1 */
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

    /* PF0 requires unlocking before configuration */
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);  /* EK_TM4C123GXL_GPIO_SW2 */
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_M;


    /* Once GPIO_init is called, GPIO_config cannot be changed */
    GPIO_init();

    GPIO_write(EK_TM4C123GXL_LED_RED, EK_TM4C123GXL_LED_OFF);
    GPIO_write(EK_TM4C123GXL_LED_GREEN, EK_TM4C123GXL_LED_OFF);
    GPIO_write(EK_TM4C123GXL_LED_BLUE, EK_TM4C123GXL_LED_OFF);
}
#endif /* TI_DRIVERS_GPIO_INCLUDED */

#if TI_DRIVERS_SPI_INCLUDED
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPITivaDMA.h>

/* SPI objects */
SPITivaDMA_Object spiTivaDMAobjects[EK_TM4C123GXL_SPICOUNT];

/* SPI configuration structure, describing which pins are to be used */
const SPITivaDMA_HWAttrs spiTivaDMAHWAttrs[EK_TM4C123GXL_SPICOUNT] = {
    {
        SSI0_BASE,
        INT_SSI0,
        UDMA_CHANNEL_SSI0RX,
        UDMA_CHANNEL_SSI0TX,
        uDMAChannelAssign,
        UDMA_CH10_SSI0RX,
        UDMA_CH11_SSI0TX
    },
    {
        SSI2_BASE,
        INT_SSI2,
        UDMA_SEC_CHANNEL_UART2RX_12,
        UDMA_SEC_CHANNEL_UART2TX_13,
        uDMAChannelAssign,
        UDMA_CH12_SSI2RX,
        UDMA_CH13_SSI2TX
    },
    {
        SSI3_BASE,
        INT_SSI3,
        UDMA_SEC_CHANNEL_TMR2A_14,
        UDMA_SEC_CHANNEL_TMR2B_15,
        uDMAChannelAssign,
        UDMA_CH14_SSI3RX,
        UDMA_CH15_SSI3TX
    }
};

const SPI_Config SPI_config[] = {
    {&SPITivaDMA_fxnTable, &spiTivaDMAobjects[0], &spiTivaDMAHWAttrs[0]},
    {&SPITivaDMA_fxnTable, &spiTivaDMAobjects[1], &spiTivaDMAHWAttrs[1]},
    {&SPITivaDMA_fxnTable, &spiTivaDMAobjects[2], &spiTivaDMAHWAttrs[2]},
    {NULL, NULL, NULL},
};

/*
 *  ======== EK_TM4C123GXL_initSPI ========
 */
Void EK_TM4C123GXL_initSPI(Void)
{
    /* SPI1 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    /* Need to unlock PF0 */
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 |
                                    GPIO_PIN_4 | GPIO_PIN_5);

    /* SSI2 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    GPIOPinConfigure(GPIO_PB5_SSI2FSS);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);

    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 |
                                    GPIO_PIN_6 | GPIO_PIN_7);

    /* SSI3 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);

    GPIOPinConfigure(GPIO_PD0_SSI3CLK);
    GPIOPinConfigure(GPIO_PD1_SSI3FSS);
    GPIOPinConfigure(GPIO_PD2_SSI3RX);
    GPIOPinConfigure(GPIO_PD3_SSI3TX);

    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                                    GPIO_PIN_2 | GPIO_PIN_3);

    EK_TM4C123GXL_initDMA();
    SPI_init();
}
#endif /* TI_DRIVERS_SPI_INCLUDED */

#if TI_DRIVERS_UART_INCLUDED
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTTiva.h>

/* UART objects */
UARTTiva_Object uartTivaObjects[EK_TM4C123GXL_UARTCOUNT];

/* UART configuration structure */
const UARTTiva_HWAttrs uartTivaHWAttrs[EK_TM4C123GXL_UARTCOUNT] = {
    {UART0_BASE, INT_UART0}, /* EK_TM4C123GXL_UART0 */
};

const UART_Config UART_config[] = {
    {
        &UARTTiva_fxnTable,
        &uartTivaObjects[0],
        &uartTivaHWAttrs[0]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C123GXL_initUART ========
 */
Void EK_TM4C123GXL_initUART()
{
    /* Enable and configure the peripherals used by the uart. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Initialize the UART driver */
    UART_init();
}
#endif /* TI_DRIVERS_UART_INCLUDED */

/*
 *  ======== EK_TM4C123GXL_initUSB ========
 *  This function just turns on the USB
 */
Void EK_TM4C123GXL_initUSB(EK_TM4C123GXL_USBMode usbMode)
{
    /* Enable the USB peripheral and PLL */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    SysCtlUSBPLLEnable();

    /* Setup pins for USB operation */
    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    if (usbMode == EK_TM4C123GXL_USBHOST) {
        System_abort("USB host not supported\n");
    }
}

#if TI_DRIVERS_WATCHDOG_INCLUDED
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogTiva.h>

/* Watchdog objects */
WatchdogTiva_Object watchdogTivaObjects[EK_TM4C123GXL_WATCHDOGCOUNT];

/* Watchdog configuration structure */
const WatchdogTiva_HWAttrs watchdogTivaHWAttrs[EK_TM4C123GXL_WATCHDOGCOUNT] = {
    /* EK_TM4C123GXL_WATCHDOG0 with 1 sec period at default CPU clock freq */
    {WATCHDOG0_BASE, INT_WATCHDOG, 80000000},
};

const Watchdog_Config Watchdog_config[] = {
    {&WatchdogTiva_fxnTable, &watchdogTivaObjects[0], &watchdogTivaHWAttrs[0]},
    {NULL, NULL, NULL},
};

/*
 *  ======== EK_TM4C123GXL_initWatchdog ========
 *
 * NOTE: To use the other watchdog timer with base address WATCHDOG1_BASE,
 *       an additional function call may need be made to enable PIOSC. Enabling
 *       WDOG1 does not do this. Enabling another peripheral that uses PIOSC
 *       such as ADC0 or SSI0, however, will do so. Example:
 *
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG1);
 *
 *       See the following forum post for more information:
 *       http://e2e.ti.com/support/microcontrollers/stellaris_arm_cortex-m3_microcontroller/f/471/p/176487/654390.aspx#654390
 */
Void EK_TM4C123GXL_initWatchdog()
{
    /* Enable peripherals used by Watchdog */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    /* Initialize the Watchdog driver */
    Watchdog_init();
}
#endif /* TI_DRIVERS_WATCHDOG_INCLUDED */

#if TI_DRIVERS_WIFI_INCLUDED
#include <ti/drivers/WiFi.h>
#include <ti/drivers/wifi/WiFiTivaCC3000.h>

/* WiFi objects */
WiFiTivaCC3000_Object wiFiTivaCC3000Objects[EK_TM4C123GXL_WIFICOUNT];

/* WiFi configuration structure */
const WiFiTivaCC3000_HWAttrs wiFiTivaCC3000HWAttrs[EK_TM4C123GXL_WIFICOUNT] = {
    { GPIO_PORTB_BASE, /* IRQ port */
      GPIO_PIN_2,      /* IRQ pin */
      INT_GPIOB,       /* IRQ port interrupt vector */

      GPIO_PORTE_BASE, /* CS port */
      GPIO_PIN_0,      /* CS pin */

      GPIO_PORTB_BASE, /* WLAN EN port */
      GPIO_PIN_5       /* WLAN EN pin */
    }
};

const WiFi_Config WiFi_config[] = {
    {&WiFiTivaCC3000_fxnTable,
     &wiFiTivaCC3000Objects[0],
     &wiFiTivaCC3000HWAttrs[0]},
    {NULL, NULL, NULL},
};

/*
 *  ======== EK_TM4C123GXL_initWiFi ========
 */
Void EK_TM4C123GXL_initWiFi(Void)
{
    /* Configure IRQ pin */
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA,
                            GPIO_PIN_TYPE_STD_WPU);

    /* Configure WLAN EN pin */
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA,
                            GPIO_PIN_TYPE_STD_WPD);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);

    /* Configure CS pin */
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
                            GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, (~0));

    /* Configure SSI2 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);

    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7);

    /* Call necessary SPI init functions */
    SPI_init();
    EK_TM4C123GXL_initDMA();

    /* Initialize WiFi driver */
    WiFi_init();
}
#endif /* TI_DRIVERS_WIFI_INCLUDED */
