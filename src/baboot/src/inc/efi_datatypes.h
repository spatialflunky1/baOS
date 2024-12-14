#ifndef EFI_DATATYPES_H
#define EFI_DATATYPES_H

//
// EFI Protocol Interface GUID Definitions
//
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    { 0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }

//
// EFI Open Protocol Methods
//
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

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
// For firmware functions
//

// EFI Function Parameter Specifications
#ifndef IN
    #define IN
    #define OUT
    #define OPTIONAL
#endif

// "Compiler" definition for calling conventions
#ifndef EFIAPI
    #define EFIAPI
#endif

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
typedef UINT8  bool;

//
// EFI Types
//

typedef UINT64 EFI_STATUS;
typedef UINT64 UINTN;
typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef void*  EFI_HANDLE;

// EFI locate search type, specifies what to be returned by the search
typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

// Specify the output pixel format
typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

// Specify different graphics operations
typedef enum {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

// Structure of the numeric identify for protocol interfaces
typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8  Data4[8];
} EFI_GUID;

#endif
