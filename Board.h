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

#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "EK_TM4C123GXL.h"

#define Board_initDMA               EK_TM4C123GXL_initDMA
#define Board_initGeneral           EK_TM4C123GXL_initGeneral
#define Board_initGPIO              EK_TM4C123GXL_initGPIO
#define Board_initSPI               EK_TM4C123GXL_initSPI
#define Board_initUART              EK_TM4C123GXL_initUART
#define Board_initUSB               EK_TM4C123GXL_initUSB
#define Board_initWatchdog          EK_TM4C123GXL_initWatchdog
#define Board_initWiFi              EK_TM4C123GXL_initWiFi
#define Board_LED                   EK_TM4C123GXL_LED_BLUE
#define Board_LED2                  EK_TM4C123GXL_LED_RED
#define Board_BUTTON                EK_TM4C123GXL_GPIO_SW1
#define Board_BUTTON2               EK_TM4C123GXL_GPIO_SW2
#define Board_LED_ON                EK_TM4C123GXL_LED_ON
#define Board_LED_OFF               EK_TM4C123GXL_LED_OFF
#define Board_SPI0                  EK_TM4C123GXL_SPI0
#define Board_SPI1                  EK_TM4C123GXL_SPI3
#define Board_SPI_CC3000            EK_TM4C123GXL_SPI2
#define Board_USBDEVICE             EK_TM4C123GXL_USBDEVICE
#define Board_UART                  EK_TM4C123GXL_UART0
#define Board_WATCHDOG              EK_TM4C123GXL_WATCHDOG0
#define Board_WIFI                  EK_TM4C123GXL_WIFI
#define Board_gpioCallbacks         EK_TM4C123GXL_gpioPortFCallbacks

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
