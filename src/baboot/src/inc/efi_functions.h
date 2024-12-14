#ifndef EFI_FUNCTIONS_H
#define EFI_FUNCTIONS_H

#include <efi_datatypes.h>

//
// Forward declarations
//
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL;
struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;
struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

//
// EFI Firmware Given Function Declarations (for tables)
//
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_RESET) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN bool   ExtendedVerification
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING) (
        IN struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN CHAR16* String
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_TEST_STRING) (
        IN struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN CHAR16* String
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_QUERY_MODE) (
        IN  struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN  UINT64  ModeNumber, 
        OUT UINT64* Columns,
        OUT UINT64* Rows
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_MODE) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN UINT64 ModeNumber
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_ATTRIBUTE) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN UINT64 Attribute
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_CLEAR_SCREEN) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_CURSOR_POSITION) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN UINT64 Column,
        IN UINT64 Row
);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_ENABLE_CURSOR) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN bool   Visible
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_WATCHDOG_TIMER) (
        IN UINTN   Timeout,
        IN UINT64  WatchdogCode,
        IN UINTN   DataSize,
        IN CHAR16* WatchdogData OPTIONAL
);

typedef
EFI_STATUS
(EFIAPI *EFI_OPEN_PROTOCOL) (
    IN  EFI_HANDLE Handle,
    IN  EFI_GUID*  Protocol,
    OUT void**     Interface OPTIONAL,
    IN  EFI_HANDLE AgentHandle,
    IN  EFI_HANDLE ControllerHandle,
    IN  UINT32     Attributes
);

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_HANDLE_BUFFER) (
        IN     EFI_LOCATE_SEARCH_TYPE SearchType,
        IN     EFI_GUID*              Protocol OPTIONAL,
        IN     void*                  SearchKey OPTIONAL,
        IN OUT UINTN*                 NoHandles,
        OUT    EFI_HANDLE**           Buffer
);

typedef
EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
    IN  struct EFI_GRAPHICS_OUTPUT_PROTOCOL*          This,
    IN  UINT32                                        ModeNumber,
    OUT UINTN*                                        SizeOfInfo,
    OUT struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION** Info
);

typedef
EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
    IN struct EFI_GRAPHICS_OUTPUT_PROTOCOL* This,
    IN UINT32                               ModeNumber
);

typedef
EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
    IN     struct EFI_GRAPHICS_OUTPUT_PROTOCOL*  This,
    IN OUT struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltBuffer, OPTIONAL
    IN     EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
    IN     UINTN                                 SourceX,
    IN     UINTN                                 SourceY,
    IN     UINTN                                 DestinationX,
    IN     UINTN                                 DestinationY,
    IN     UINTN                                 Width,
    IN     UINTN                                 Height,
    IN     UINTN                                 Delta OPTIONAL
);

#endif
