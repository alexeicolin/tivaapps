#include "uart_iface.h"

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <stdint.h> /* assumed by ti.drivers headers (?) */
#include <ti/drivers/UART.h>

struct _UART_Iface_Port {
    UART_Handle handle; /* Handle for all UART APIs */
    UInt        base;   /* Base address of the UART */
    UInt        open;   /* Number of users for this UART */
    Bool        binary; /* UART has been opened in binary mode */
};

/* Using only UART0 */
#define NUM_PORTS 1
static struct _UART_Iface_Port ports[NUM_PORTS] = {{NULL, 0, 0, FALSE}};

/*
 *  ======== openHandle ========
 *  The UART driver will return NULL if there was an error creating a UART 
 *
 *  @param  index  Index into the ports array of UARTPorts
 *  @param  binary Open the UART in binary mode
 *  @return        an opaque port handle
 */
UART_Iface_Port UART_Iface_openHandle(UInt base, Bool binary)
{
    UART_Params uartParams;
    UInt index = 0;

    /* Find a free port slot */
    while (ports[index].handle != NULL && index < NUM_PORTS)
        index++;
    
    /* Only UART 0 is supported in this example. */
    if (index >= NUM_PORTS) {
        System_printf("Cannot allocate port object: all %d taken.\n",
                NUM_PORTS);
        return (NULL);
    }

    /*
     *  The UART driver only allows creating once, return the handle if
     *  already open in the correct mode.
     */
    if (ports[index].open) {
        /* Make sure the index is not already opened in the wrong mode */
        if (binary != ports[index].binary) {
            return (NULL);
        }
        ports[index].open++;
        return (ports[index].handle);
    }

    /* Create a UART with the parameters below. */
    ports[index].base = base;
    UART_Params_init(&uartParams);
    if (binary == TRUE) {
        uartParams.readEcho = UART_ECHO_OFF;
        uartParams.writeDataMode = UART_DATA_BINARY;
        ports[index].binary = TRUE;
    }
    else {
        uartParams.writeDataMode = UART_DATA_TEXT;
        ports[index].binary = FALSE;
    }
    ports[index].handle = UART_open(ports[index].base, &uartParams);
    if (ports[index].handle != NULL) {
        ports[index].open = 1;
    }
    
    /* The handle is a one-based index (to let NULL signify error) */
    return (Void *)(index + 1);
}

/*
 *  ======== closeHandle ========
 */
Void UART_Iface_closeHandle(UART_Iface_Port port)
{
    UInt index = (UInt)port - 1;
    Assert_isTrue(port > 0, NULL);
    ports[index].open--;
    if (ports[index].open == 0) {
        UART_close(ports[index].handle);
        ports[index].handle = NULL;
    }
}

Void UART_Iface_write(UART_Iface_Port port, Char *buf, Int size)
{
    UART_Handle uart;
    Int nBytes = 0;
    Int bytesLeft = size;
    Int bytesToWrite;
    UInt index = (UInt)port - 1;

    Assert_isTrue(port > 0, NULL);

    if (size > 0) {
        uart = ports[index].handle;

        /* Write to the UART until the transfer is finished. */
        while (bytesLeft > 0) {
            bytesToWrite = (bytesLeft < 16) ? bytesLeft : 16;
            nBytes = UART_writePolling(uart, (Char *)buf, bytesToWrite);

            /*
             *  If the buffer contains a '\n',  UART_writePolling() will output
             *  a '\r' in addition to the '\n', so the number of bytes returned
             *  may be greater than the number of bytes to write from the
             *  buffer.  So make sure not advance the buffer pointer by more
             *  than what we intended to write.
             */
            nBytes = (nBytes > bytesToWrite) ? bytesToWrite : nBytes;

            buf += nBytes;
            bytesLeft -= nBytes;
        }
    }
}
