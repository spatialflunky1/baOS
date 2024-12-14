#ifndef EFI_FUNCTIONS_H
#define EFI_FUNCTIONS_H

#include <efi_datatypes.h>

//
// Forward declarations
//
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

//
// EFI Firmware Given Function Declarations (for tables)
//
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_RESET) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN bool   ExtendedVerification);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING) (
        IN struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN CHAR16* String);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_TEST_STRING) (
        IN struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN CHAR16* String);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_QUERY_MODE) (
        IN  struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN  UINT64  ModeNumber, 
        OUT UINT64* Columns,
        OUT UINT64* Rows);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_MODE) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN UINT64 ModeNumber);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_ATTRIBUTE) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN UINT64 Attribute);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_CLEAR_SCREEN) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_CURSOR_POSITION) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN UINT64 Column,
        IN UINT64 Row);

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_ENABLE_CURSOR) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN bool   Visible);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_WATCHDOG_TIMER) (
        IN UINTN   Timeout,
        IN UINT64  WatchdogCode,
        IN UINTN   DataSize,
        IN CHAR16* WatchdogData OPTIONAL);

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_HANDLE_BUFFER) (
        IN     EFI_LOCATE_SEARCH_TYPE SearchType,
        IN     EFI_GUID*              Protocol OPTIONAL,
        IN     void*                  SearchKey OPTIONAL,
        IN OUT UINTN*                 NoHandles,
        OUT    EFI_HANDLE**           Buffer
        );

#endif
