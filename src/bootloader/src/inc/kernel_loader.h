#ifndef KERNEL_LOADER_H
#define KERNEL_LOADER_H

#include <firmware/efi.h>
#include <elf.h>

//
// Tables for post-bootloader info storage
//
typedef struct {
    void*  FramebufferPointer;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    UINT32 PixelsPerScanline;
} KERNEL_BOOT_VIDEO_MODE_INFO;

typedef struct {
    EFI_MEMORY_DESCRIPTOR*      MemoryMap;
    UINT64                      MemoryMapSize;
    UINT64                      MemoryMapDescriptorSize;
    KERNEL_BOOT_VIDEO_MODE_INFO VideoModeInfo;
    void*                       KernelBase;
} KERNEL_BOOT_INFO;

//
// Functions for loading the kernel into memory space
//

EFI_STATUS load_kernel(EFI_FILE_PROTOCOL* RootFileSystem, 
                       CHAR16* KernelFilename, 
                       EFI_PHYSICAL_ADDRESS* KernelEntryPoint, 
                       EFI_SYSTEM_TABLE* SystemTable,
                       EFI_PHYSICAL_ADDRESS* ProgramSegment_buf);

#endif
