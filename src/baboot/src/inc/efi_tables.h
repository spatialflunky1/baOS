#ifndef EFI_TABLES_H
#define EFI_TABLES_H

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
        IN struct   EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN UINT64   ModeNumber, 
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


///
/// EFI Tables
///

// Table header used by most/all EFI tables
typedef struct {
    UINT64 Signature;  // Table signature
    UINT32 Revision;   // EFI spec
                       // Upper 16 bits: Major Revision
                       // Lower 16 bits: Minor Revision
                       // Format:  (Major).(Minor upper decimal).(Minor lower decimal)
    UINT32 HeaderSize; // Size of the table
    UINT32 CRC32;      // Cyclic Redundancy Check 32 (checksum)
    UINT32 Reserved;   // Set 0
} EFI_TABLE_HEADER;

// Structure specifying text output mode
typedef struct {
    INT32 MaxMode;
    // Current settings
    INT32 Mode;
    INT32 Attribute;
    INT32 CursorColumn;
    INT32 CursorRow;
    bool  CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

// Table for text output
typedef struct {
    EFI_TEXT_RESET               Reset;
    EFI_TEXT_STRING              OutputString;
    EFI_TEXT_TEST_STRING         TestString;
    EFI_TEXT_QUERY_MODE          QueryMode;
    EFI_TEXT_SET_MODE            SetMode;
    EFI_TEXT_SET_ATTRIBUTE       SetAttribute;
    EFI_TEXT_CLEAR_SCREEN        ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR       EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE*     Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// Table containing the various functions given by the firmware
// Definitions with the void pointer type are unused and therefore undefined
typedef struct {
    EFI_TABLE_HEADER        Hdr;

    // Task Priority Services
    void*                   RaiseTPL;
    void*                   RestoreTPL;
    
    // Memory Services
    void*                   AllocatePages;
    void*                   FreePages;
    void*                   GetMemoryMap;
    void*                   AllocatePool;
    void*                   FreePool;
    
    // Event & Timer Services
    void*                   CreateEvent;
    void*                   SetTimer;
    void*                   WaitForEvent;
    void*                   SignalEvent;
    void*                   CloseEvent;
    void*                   CheckEvent;
    
    // Protocol Handler Services
    void*                   InstallProtocolInterface;
    void*                   ReinstallProtocolInterface;
    void*                   UninstallProtocolInterface;
    void*                   HandleProtocol;
    void*                   Reserved;
    void*                   RegisterProtocolNotify;
    void*                   LocateHandle;
    void*                   LocateDevicePath;
    void*                   InstallConfigurationTable;
    
    // Image Services
    void*                   LoadImage;
    void*                   StartImage;
    void*                   Exit;
    void*                   UnloadImage;
    void*                   ExitBootServices;
    
    // Miscellaneous Services
    void*                   GetNextMonotonicCount;
    void*                   Stall;
    EFI_SET_WATCHDOG_TIMER  SetWatchdogTimer;
    
    // DriverSupport Services
    void*                   ConnectController;
    void*                   DisconnectController;
    
    // Open and Close Protocol Services
    void*                   OpenProtocol;
    void*                   CloseProtocol;
    void*                   OpenProtocolInformation;
    
    // Library Services
    void*                   ProtocolsPerHandle;
    void*                   LocateHandleBuffer;
    void*                   LocateProtocol;
    void*                   InstallMultipleProtocolInterfaces;
    void*                   UninstallMultipleProtocolInterfaces;
    
    // 32-bit CRC Services
    void*                   CalculateCrc32;
    
    // Miscellaneous Services
    void*                   CopyMem;
    void*                   SetMem;
    void*                   CreateEventEx;
} EFI_BOOT_SERVICES;

// The main system table used by the bootloader
// Contains critical function addresses
// Definitions with the void pointer type are unused and therefore undefined
typedef struct {
    EFI_TABLE_HEADER                 Hdr;
    CHAR16*                          FirmwareVendor;
    UINT32                           FirmwareRevision;
    EFI_HANDLE                       ConsoleInHandle;
    void*                            ConIn;
    EFI_HANDLE                       ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
    EFI_HANDLE                       StandardErrorHandle;
    void*                            StdErr;
    void*                            RuntimeServices;
    EFI_BOOT_SERVICES*               BootServices;
    UINTN                            NumberOfTableEntries;
    void*                            ConfigurationTable;
} EFI_SYSTEM_TABLE;

#endif
