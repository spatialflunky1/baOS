#ifndef MESSAGES_H
#define MESSAGES_H

#include <datatypes.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef MESSAGE_FILTER
    #define MESSAGE_FILTER FILTER_NONE
#endif

// Definition of all filter levels for kernel messages
typedef enum {
    FILTER_NONE,
    DebugMsg,
    InfoMsg,
    WarningMsg,
    ErrMsg,
    CriticalMsg
} FILTER_LEVEL;

// Returns the number of bytes printed or -1 if the filter level is not met
int klog(FILTER_LEVEL level, bool prefix, const char* restrict format, ...);

#endif
