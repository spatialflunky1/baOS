#ifndef DATATYPES_H
#define DATATYPES_H

// C Null definition
#define NULL ((void*)0)

// Basic types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int __attribute__((__mode__(__DI__))) uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed int __attribute__((__mode__(__DI__))) int64_t;

// Boolean values
typedef uint8_t bool;
#define true 1
#define false 0

// Max type digits (+1 is to account for signed values having a negative symbol)
#define UINT64_DEC_MAXDIGIT 20+1
#define UINT64_HEX_MAXDIGIT 16+1
#define UINT32_MAX 4294967295

#endif
