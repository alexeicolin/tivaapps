#include "event_logger.h"

#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Assert.h>
/* #include <xdc/runtime/Diags.h> */

#include <ti/uia/runtime/EventHdr.h>

#include <string.h>


#if (xdc_target__bitsPerChar * xdc_target__sizeof_Int) == 32
#define SERIAL "#%010u "
#define HI "%0.8x:"
#define LO "%0.8x] "
#elif (xdc_target__bitsPerChar * xdc_target__sizeof_Long) == 32 
#define SERIAL "#%010lu "
#define HI "%0.8lx:"
#define LO "%0.8lx] "
#else
#error xdc.runtime.Log does not support this target. 
#endif

static char outbuf[1024];
static OutputFunc outputFunc = NULL;

static Void outputEvent(Log_EventRec *er)
{
    Text_RopeId rope;
    String  fmt;
    Bits32  hi, lo;
    Char   *bufPtr = outbuf;
    Int     size = 0;

    /* print serial number if there is one; 0 isn't a valid serial number */
    if (er->serial) {
        System_sprintf(bufPtr, SERIAL, er->serial);
        bufPtr = outbuf + strlen(outbuf);
    }

    /* print timestamp if there is one; ~0 isn't a valid timestamp value */
    hi = er->tstamp.hi;
    lo = er->tstamp.lo;
    if (lo != ~0 && hi != ~0) {
        System_sprintf(bufPtr, "[t=0x");
        bufPtr = outbuf + strlen(outbuf);
        if (hi) {
            System_sprintf(bufPtr, HI, hi);
            bufPtr = outbuf + strlen(outbuf);
        }
        System_sprintf(bufPtr, LO, lo);
        bufPtr = outbuf + strlen(outbuf);
    }

    /* print module name */
    Text_putMod((Text_RopeId)Types_getModuleId(er->evt), &bufPtr, -1);
    System_sprintf(bufPtr, ": ");
    bufPtr = outbuf + strlen(outbuf);
    
    /* print event */
    rope = Types_getEventId(er->evt);   /* the event id is the message rope */
    if (rope == 0) {
        /* Log_print() event */
        System_asprintf(bufPtr, (String)iargToPtr(er->arg[0]),
            er->arg[1], er->arg[2], er->arg[3], er->arg[4], 
            er->arg[5], er->arg[6], 0, 0);
        bufPtr = outbuf + strlen(outbuf);
    }
    else {
        /* Log_write() event */
        fmt = Text_ropeText(rope);
    
        if (Text_isLoaded) {
            System_asprintf(bufPtr, fmt, er->arg[0], er->arg[1], er->arg[2],
                    er->arg[3], er->arg[4], er->arg[5], er->arg[6],
                    er->arg[7]);
            bufPtr = outbuf + strlen(outbuf);
        }
        else {
            System_asprintf(bufPtr, "{evt: fmt=%p, args=[0x%x, 0x%x ...]}",
                fmt, er->arg[0], er->arg[1]);
            bufPtr = outbuf + strlen(outbuf);
        }
    }

    *bufPtr++ = '\n';
    size = bufPtr - outbuf;

    Assert_isTrue(outputFunc, NULL);
    outputFunc(outbuf, size);
}

Void EventLogger_registerOutputFunc(OutputFunc outFunc)
{
    outputFunc = outFunc;
}

Int EventLogger_processEvent(UChar *a, Int size)
{
    Log_EventRec evrec;
    EventHdr_HdrType hdrType;
    Int     i;
    Int     nBytes;
    Int     nWords;
    UInt32 *wordPtr;
    UInt32 *logRec;
    UInt32 *args;
    Int     nArgs;
    Int     bytesSent = 0;
    Int     remainder = size;

    System_flush();

    logRec = (UInt32 *)a;

    while (remainder > 0) {
        wordPtr = logRec;
        hdrType = EventHdr_getHdrType(*wordPtr);
        nBytes = EventHdr_getLength(*wordPtr);
        nWords = nBytes / 4;

        evrec.serial = EventHdr_getSeqCount(*wordPtr);

        if (hdrType == EventHdr_HdrType_EventWithTimestamp) {
            evrec.evt = *(wordPtr + 3);
            evrec.tstamp.hi = *(wordPtr + 2);
            evrec.tstamp.lo = *(wordPtr + 1);
            args = wordPtr + 4;
            nArgs = nWords - 4;
        }
        else {
            evrec.evt = *(wordPtr + 1);
            evrec.tstamp.hi = 0;
            evrec.tstamp.lo = 0;
            args = wordPtr + 2;
            nArgs = nWords - 2;
        }

        if (nArgs > Log_NUMARGS) {
            /* Bad event header! */
            nArgs = Log_NUMARGS;
        }

        for (i = 0; i < nArgs; i++) {
            evrec.arg[i] = *(args + i);
        }
        for (i = nArgs; i < Log_NUMARGS; i++) {
            evrec.arg[i] = 0;
        }

        outputEvent(&evrec);

        bytesSent += nBytes;
        remainder -= nBytes;
        logRec += nWords;
    }

    return (bytesSent);
}

