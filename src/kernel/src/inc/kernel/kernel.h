#ifndef KERNEL_H
#define KERNEL_H

#include <datatypes.h>
#include <efi.h>
#include <stdio.h>
#include <drivers/video/video.h>
#include <kernel/messages.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>

#ifndef KVER
    #define KVER "0.0.0"
#endif

typedef struct {
    EFI_MEMORY_DESCRIPTOR* MemoryMap;
    uint64_t               MemoryMapSize;
    uint64_t               MemoryMapDescriptorSize;
    BOOT_VIDEO_MODE_INFO   VideoModeInfo;
    void*                  KernelBase; // The pointer to the beginning of the kernel buffer
} KERNEL_BOOT_INFO;

#endif
