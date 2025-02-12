/*
 * For anything EFI specific taken from the bootloader
 */

#ifndef EFI_H
#define EFI_H

#include <datatypes.h>

typedef struct {
    uint32_t Type;
    uint64_t PhysicalStart;
    uint64_t VirtualStart;
    uint64_t NumberOfPages;
    uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

#endif
