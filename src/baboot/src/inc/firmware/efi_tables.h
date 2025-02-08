#ifndef EFI_TABLES_H
#define EFI_TABLES_H

#include <firmware/efi_datatypes.h>
#include <firmware/efi_functions.h>

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

// Struct describing the pixel bitmask for a graphics mode
typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

//
typedef struct {
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

// Contains the information describing a returned graphics mode
typedef struct {
    UINT32                    Version;
    UINT32                    HorizontalResolution;
    UINT32                    VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK         PixelInformation;
    UINT32                    PixelsPerScanline;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32                                MaxMode; // # of modes supported by (query/set)mode
    UINT32                                Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info;
    UINTN                                 SizeOfInfo;
    EFI_PHYSICAL_ADDRESS                  FrameBufferBase;
    UINTN                                 FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// Provides the ability to set video modes and access the framebufffer
typedef struct {
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE   SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT        Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*      Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef struct _EFI_FILE_PROTOCOL {
    UINT64 Revision;
    EFI_FILE_OPEN         Open;
    EFI_FILE_CLOSE        Close;
    void*                 Delete;
    EFI_FILE_READ         Read;
    void*                 Write;
    void*                 GetPosition;
    EFI_FILE_SET_POSITION SetPosition;
    void*                 GetInfo;
    void*                 SetInfo;
    void*                 Flush;
    void*                 OpenEx; // Added for revision 2
    void*                 ReadEx; // Added for revision 2
    void*                 WriteEx; // Added for revision 2
    void*                 FlushEx; // Added for revision 2
} EFI_FILE_PROTOCOL;

// Structure containing the function to open the boot volume
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64 Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// Table containing the various functions given by the firmware
// Definitions with the void pointer type are unused and therefore undefined
typedef struct {
    EFI_TABLE_HEADER         Hdr;

    // Task Priority Services
    void*                    RaiseTPL;
    void*                    RestoreTPL;
    
    // Memory Services
    EFI_ALLOCATE_PAGES       AllocatePages;
    EFI_FREE_PAGES           FreePages;
    EFI_GET_MEMORY_MAP       GetMemoryMap;
    EFI_ALLOCATE_POOL        AllocatePool;
    EFI_FREE_POOL            FreePool;
    
    // Event & Timer Services
    void*                    CreateEvent;
    void*                    SetTimer;
    void*                    WaitForEvent;
    void*                    SignalEvent;
    void*                    CloseEvent;
    void*                    CheckEvent;
    
    // Protocol Handler Services
    void*                    InstallProtocolInterface;
    void*                    ReinstallProtocolInterface;
    void*                    UninstallProtocolInterface;
    void*                    HandleProtocol;
    void*                    Reserved;
    void*                    RegisterProtocolNotify;
    void*                    LocateHandle;
    void*                    LocateDevicePath;
    void*                    InstallConfigurationTable;
    
    // Image Services
    void*                    LoadImage;
    void*                    StartImage;
    void*                    Exit;
    void*                    UnloadImage;
    EFI_EXIT_BOOT_SERVICES   ExitBootServices;
    
    // Miscellaneous Services
    void*                    GetNextMonotonicCount;
    void*                    Stall;
    EFI_SET_WATCHDOG_TIMER   SetWatchdogTimer;
    
    // DriverSupport Services
    void*                    ConnectController;
    void*                    DisconnectController;
    
    // Open and Close Protocol Services
    EFI_OPEN_PROTOCOL        OpenProtocol;
    void*                    CloseProtocol;
    void*                    OpenProtocolInformation;
    
    // Library Services
    void*                    ProtocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL      LocateProtocol;
    void*                    InstallMultipleProtocolInterfaces;
    void*                    UninstallMultipleProtocolInterfaces;
    
    // 32-bit CRC Services
    void*                    CalculateCrc32;
    
    // Miscellaneous Services
    EFI_COPY_MEM             CopyMem;
    EFI_SET_MEM              SetMem;
    void*                    CreateEventEx;
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

// Firmware paging memory descriptor
typedef struct {
    UINT32               Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS  VirtualStart;
    UINT64               NumberOfPages;
    UINT64               Attribute;
} EFI_MEMORY_DESCRIPTOR;

#endif
