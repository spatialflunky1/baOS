#ifndef KERNEL_LOADER_H
#define KERNEL_LOADER_H

#include <firmware/efi.h>
#include <elf.h>

//
// Functions for loading the kernel into memory space
//

EFI_STATUS load_kernel(EFI_FILE_PROTOCOL* RootFileSystem, 
                       CHAR16* KernelFilename, 
                       EFI_PHYSICAL_ADDRESS* KernelEntryPoint, 
                       EFI_SYSTEM_TABLE* SystemTable);

#endif
