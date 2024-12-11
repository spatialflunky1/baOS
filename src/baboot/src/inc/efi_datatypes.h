#ifndef EFI_DATATYPES_H
#define EFI_DATATYPES_H

//
// Miscellaneous
//

// NULL
#define NULL ((void*)0)

// Max type digits
#define UINT64_DEC_MAXDIGIT 20
#define UINT64_HEX_MAXDIGIT 16

// Boolean values
#define true 1
#define false 0

//
// Basic types
//

// Unsigned
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;
typedef unsigned int __attribute__((__mode__(__DI__))) UINT64;

// Signed
typedef signed char  INT8;
typedef signed short INT16;
typedef signed int   INT32;
typedef signed int __attribute__((__mode__(__DI__))) INT64;

// Misc. basic
typedef UINT16 CHAR16;
typedef UINT8 bool;

//
// EFI Types
//

typedef UINT64   EFI_STATUS;
typedef UINT64   UINTN;
typedef void*    EFI_HANDLE;

#endif
