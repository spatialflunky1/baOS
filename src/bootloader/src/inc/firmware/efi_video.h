#ifndef EFI_VIDEO_H
#define EFI_VIDEO_H

#include <firmware/efi_datatypes.h>
#include <firmware/efi_tables.h>

//
// Graphics related macros
//

// Tells the open protocol function to utilize the handle
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001

// Structure to hold graphics handles returned by LocateHandleBuffer
typedef struct {
    EFI_HANDLE* handle_buffer;
    UINT64      handle_count;
} EfiGraphicsService;

//
// Statically defined global variables for use with EFI functions
//

// Stored system table
static EFI_SYSTEM_TABLE* sys_table = NULL;

// Required variable LocateHandleBuffer
static EFI_GUID gEfiGraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

//
// Video Functions
//

void initializeVideoSysTableVar(EFI_SYSTEM_TABLE* SystemTable); 
void print(CHAR16* s);
void print_num(UINT64 n);
void print_hex(UINT64 h, bool ZeroX);

#endif
