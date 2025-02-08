#ifndef EFI_DATATYPES_H
#define EFI_DATATYPES_H

//
// EFI Protocol Interface GUID Definitions
//
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    { 0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
    { 0x964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

//
// EFI file open modes
//
#define EFI_FILE_MODE_READ   0x0000000000000001
#define EFI_FILE_MODE_WRITE  0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

//
// EFI file attributes
//
#define EFI_FILE_READ_ONLY  0x0000000000000001
#define EFI_FILE_HIDDEN     0x0000000000000002
#define EFI_FILE_SYSTEM     0x0000000000000004
#define EFI_FILE_RESERVED   0x0000000000000008
#define EFI_FILE_DIRECTORY  0x0000000000000010
#define EFI_FILE_ARCHIVE    0x0000000000000020
#define EFI_FILE_VALID_ATTR 0x0000000000000037

//
// Paging
//
#define EFI_PAGE_MASK  0xFFF
#define EFI_PAGE_SHIFT 12
#define EFI_SIZE_TO_PAGES(a)  \
    ( ((a) >> EFI_PAGE_SHIFT) + ((a) & EFI_PAGE_MASK ? 1 : 0) )

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
//Basic types
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
typedef UINT64   EFI_PHYSICAL_ADDRESS;
typedef UINT64   EFI_VIRTUAL_ADDRESS;
typedef UINT64   UINTN;
typedef void*    EFI_HANDLE;

// EFI locate search type, specifies what to be returned by the search
typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

// Specifier for different pixel formats described by the graphics output
typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

// Specifier for different blt operations
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

// Specifies the type of memory to be allocated by firmware
typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

// Specifies the type of pages that can be allocated by firmware
typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

#endif
