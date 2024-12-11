#ifndef EFI_VIDEO_H
#define EFI_VIDEO_H

#include <efi_tables.h>

// Statically defined global variable for use with EFI functions
static EFI_SYSTEM_TABLE* sys_table = NULL;

void initializeVideoSysTableVar(EFI_SYSTEM_TABLE* SystemTable); 
void print(CHAR16* s);

#endif
