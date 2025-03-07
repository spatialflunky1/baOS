#ifndef ELF_H
#define ELF_H

#include <firmware/efi.h>

//
// ELF macros
//

// ELF identity size
#define EI_NIDENT 16

// ELF header fields
#define EI_MAG0       0x0
#define EI_MAG1       0x1
#define EI_MAG2       0x2
#define EI_MAG3       0x3
#define EI_CLASS      0x4
#define EI_DATA       0x5
#define EI_VERSION    0x6
#define EI_OSABI      0x7
#define EI_ABIVERSION 0x8

// Program header types
#define PT_NULL    0x0
#define PT_LOAD    0x1
#define PT_DYNAMIC 0x2
#define PT_INTERP  0x3
#define PT_NOTE    0x4
#define PT_SHLIB   0x5
#define PT_PHDR    0x6
#define PT_TLS     0x7

// ELF file class (address width)
#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

// ELF data structure encoding
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

//
// ELF header structures
//
typedef struct {
    UINT8  e_ident[EI_NIDENT]; // ELF identity
    UINT16 e_type;             // Object file type (ex. executable/shared/core)
    UINT16 e_machine;          // Target instruction set architecture
    UINT32 e_version;          // Set 1
                               // 0: Invalid version
                               // 1: Current version
    UINT64 e_entry;            // Memory address of the entry point
    UINT64 e_phoff;            // Pointer to the program header table
    UINT64 e_shoff;            // Pointer to the section header table
    UINT32 e_flags;            // Processor-specific flags
    UINT16 e_ehsize;           // Size of this header
    UINT16 e_phentsize;        // Size of a program header table entry (typically 0x38 for 64 bit)
    UINT16 e_phnum;            // Number of entries in the program header table
    UINT16 e_shentsize;        // Size of a section header table entry (typically 0x40 for 64 bit)
    UINT16 e_shnum;            // Number of entries in the section header table
    UINT16 e_shstrndx;         // Index of the section header table entry with the section names
} Elf64_Ehdr;

typedef struct {
    UINT32 p_type;   // Segment/Identity type
    UINT32 p_flags;  // Segment-dependent flags
    UINT64 p_offset; // Offset of the segment in the file
    UINT64 p_vaddr;  // Virtual address of the segment
    UINT64 p_paddr;  // Physical address of the segment (on systems using physical address)
    UINT64 p_filesz; // Size in bytes of the segment in the file
    UINT64 p_memsz;  // Size in bytes of the segment in memory
    UINT64 p_align;  // 0 or 1: no alignment
                     // p_vaddr = p_offset % p_align
} Elf64_Phdr;

//
// ELF related functions
//

EFI_STATUS read_elf_identity(EFI_FILE_PROTOCOL* KernelImage, 
                             UINT8*             ELFIDBuffer);
EFI_STATUS validate_elf_identity(UINT8* ELFIDBuffer);
EFI_STATUS read_elf_headers(EFI_SYSTEM_TABLE*  SystemTable, 
                            EFI_FILE_PROTOCOL* KernelImage, 
                            Elf64_Ehdr**       KernelELFHeader_ptr, 
                            Elf64_Phdr**       KernelELFProgramHeaders_ptr);
EFI_STATUS load_segment(EFI_SYSTEM_TABLE* SystemTable, 
                        EFI_FILE_PROTOCOL* KernelImage, 
                        Elf64_Phdr* ProgramHeader);
EFI_STATUS load_program_segments(EFI_SYSTEM_TABLE*     SystemTable, 
                                 EFI_FILE_PROTOCOL*    KernelImage, 
                                 Elf64_Ehdr*           KernelHeader, 
                                 Elf64_Phdr*           KernelProgramHeaders,
                                 EFI_PHYSICAL_ADDRESS* KernelEntryPoint,
                                 EFI_PHYSICAL_ADDRESS* KernelBase);
void       print_elf_info(void* KernelHeader);

#endif
