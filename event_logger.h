#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <xdc/std.h>

typedef Void (*OutputFunc)(Char *buf, Int size);

Void EventLogger_registerOutputFunc(OutputFunc outFunc);
Int EventLogger_processEvent(UChar *a, Int size);

#endif
