#ifndef UART_IFACE_H
#define UART_IFACE_H

#include <xdc/std.h>

typedef Void * UART_Iface_Port;

UART_Iface_Port UART_Iface_openHandle(UInt base, Bool binary);
Void UART_Iface_closeHandle(UART_Iface_Port port);
Void UART_Iface_write(UART_Iface_Port port, Char *buf, Int size);

#endif
