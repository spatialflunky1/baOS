#ifndef EFI_H
#define EFI_H

#include <efi_datatypes.h>
#include <efi_status.h>

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

// The main system table used by the bootloader
// Contains critical function addresses
// Definitions with the void pointer type are unused and therefore undefined
typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16*          FirmwareVendor;
    UINT32           FirmwareRevision;
    EFI_HANDLE       ConsoleInHandle;
    void*            ConIn;
    EFI_HANDLE       ConsoleOutHandle;
    void*            ConOut;
    EFI_HANDLE       StandardErrorHandle;
    void*            StdErr;
    void*            RuntimeServices;
    void*            BootServices;
    UINTN            NumberOfTableEntries;
    void*            ConfigurationTable;
} EFI_SYSTEM_TABLE;

#endif
