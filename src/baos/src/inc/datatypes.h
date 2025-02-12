#ifndef DATATYPES_H
#define DATATYPES_H

#define NULL ((void*)0)

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int __attribute__((__mode__(__DI__))) uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed int __attribute__((__mode__(__DI__))) int64_t;

#endif
