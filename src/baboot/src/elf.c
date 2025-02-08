#include <elf.h>

EFI_STATUS read_elf_identity(EFI_FILE_PROTOCOL* KernelImage, 
                             UINT8*             ElfIDBuffer) {
    // To store the returned status of various functions
    EFI_STATUS status     = EFI_SUCCESS;
    // To hold the returned size of the ELF identity buffer
    UINT64     BufferSize = EI_NIDENT;

    #ifdef __DEBUG__
        print(L"DEBUG: Reading kernel ELF identity\r\n");
    #endif

    // Set the kernel's file pointer to the beginning of the file (pos 0)
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage, 0);
    if (EFI_ERROR(status)) {
        print(L"Error: Unable to set the position of the kernel file pointer\r\n");
        return status;
    }

    // Read the ELF identity at the beginning of the file pointer
    #ifdef __DEBUG__
        print(L"DEBUG: Reading ELF identity from kernel image\r\n");
    #endif
    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage,
            &BufferSize,
            (void*)ElfIDBuffer);
    if (EFI_ERROR(status)) {
        print(L"Error: Unable to read from the kernel image file\r\n");
        return status;
    }

    return status;
}

/* ----------------------Kernel ELF Identity Validation----------------------
 * In order for the ELF identity of a file to be valid it must start with a
 * "magic" value of 0x7F, followed by "ELF" in ascii.
 *
 * Following this the ELF class must specify an x86_64 file as the kernel
 * only supports 64 bit systems on the x86 architecture.
 *
 * Finally, the identity needs to specify that the ELF image is in LSB (least 
 * significant byte) format as that is how the data was encoded.
 *---------------------------------------------------------------------------*/
EFI_STATUS validate_elf_identity(UINT8* ELFIDBuffer) {
    #ifdef __DEBUG__
        print(L"DEBUG: Validating kernel ELF identity\r\n");
    #endif

    //    
    // Validate ELF magic
    //
    if ((ELFIDBuffer[EI_MAG0] != 0x7F) ||
        (ELFIDBuffer[EI_MAG1] != 'E')  ||
        (ELFIDBuffer[EI_MAG2] != 'L') ||
        (ELFIDBuffer[EI_MAG3] != 'F')) 
    {
        print(L"Error: Invalid ELF header magic in kernel image\r\n");
        return EFI_INVALID_PARAMETER;
    }
    
    //
    // Validate ELF class
    //
    if (ELFIDBuffer[EI_CLASS] != ELFCLASS64) {
        print(L"Error: Kernel is not identified as x86_64 compatible, something may be very wrong\r\n");
        return EFI_INCOMPATIBLE_VERSION;
    }
    
    //
    // Validate ELF format
    //
    if (ELFIDBuffer[EI_DATA] != ELFDATA2LSB) {
        print(L"Error: Kernel ELF header is not labeled as LSB compliant\r\n");
        print(L"       Refer to EI_CLASS for more info\r\n");
        return EFI_INCOMPATIBLE_VERSION;
    }
    
    #ifdef __DEBUG__
        print(L"DEBUG: Kernel ELF identity is valid\r\n");
    #endif
    return EFI_SUCCESS;
}

/* -------------------------------Note-------------------------------
 * All memory allocated in this function will be later cleaned up by
 * load_kernel() in loader.c
 * ------------------------------------------------------------------*/
EFI_STATUS read_elf_headers(EFI_SYSTEM_TABLE*  SystemTable, 
                            EFI_FILE_PROTOCOL* KernelImage, 
                            Elf64_Ehdr**       KernelELFHeader_ptr, 
                            Elf64_Phdr**       KernelELFProgramHeaders_ptr) {
    // To store the returned status of various functions
    EFI_STATUS status               = EFI_SUCCESS;
    // To store the size of the buffers read, needed by firmware defined functions
    UINT64     BufferSize           = sizeof(Elf64_Ehdr);
    // To store the offset into the kernel image in which the ELF program headers are located
    UINT64     ProgramHeadersOffset = 0;

    #ifdef __DEBUG__
        print(L"DEBUG: Reading headers from kernel image\r\n");
    #endif

    // Reset kernel image file pointer to the beginning of the file
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage, 0);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error setting kernel image file pointer position\r\n");
        return status;
    }
    
    //
    // Read kernel ELF header
    //
    #ifdef __DEBUG__
        print(L"  DEBUG: Reading ELF header\r\n");
    #endif

    // Allocate buffer for ELF header
    status = SystemTable->BootServices->AllocatePool(
            EfiLoaderData,
            BufferSize,
            (void**)KernelELFHeader_ptr);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error allocating memory for kernel ELF header\r\n");
        return status;
    }
    
    // Read ELF header
    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage,
            &BufferSize,
            *KernelELFHeader_ptr);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error reading kernel ELF header\r\n");
        return status;
    }
    
    //
    // Read kernel ELF program headers
    //
    #ifdef __DEBUG__
        print(L"  DEBUG: Reading ELF program headers\r\n");
    #endif
    
    // Set values for ELF program headers found in the ELF header
    ProgramHeadersOffset = ((Elf64_Ehdr*)*KernelELFHeader_ptr)->e_phoff;
    BufferSize           = sizeof(Elf64_Phdr) * ((Elf64_Ehdr*)*KernelELFHeader_ptr)->e_phnum;
    
    // Allocate buffer for ELF program headers
    status = SystemTable->BootServices->AllocatePool(
            EfiLoaderData,
            BufferSize,
            (void**)KernelELFProgramHeaders_ptr);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error allocating memory for kernel ELF program headers\r\n");
        return status;
    }

    // Set kernel image file pointer position to the start of the program headers (program header offset)
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage, ProgramHeadersOffset);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error setting kernel image file pointer position\r\n");
        return status;
    }

    // Read ELF program headers
    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage,
            &BufferSize,
            *KernelELFProgramHeaders_ptr);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error reading kernel ELF program headers\r\n");
        return status;
    }

    return status;
}

EFI_STATUS load_segment(EFI_SYSTEM_TABLE* SystemTable, 
                        EFI_FILE_PROTOCOL* KernelImage, 
                        Elf64_Phdr* ProgramHeader) {
    // To store the returned status of various functions
    EFI_STATUS status                  = EFI_SUCCESS;
    // To store the size of different buffers being addressed
    UINT64     BufferSize              = 0;
    // The buffer containing the contents of the program segment to be retrieved and stored in memory
    void*      ProgramSegmentBuffer    = NULL;
    // The number of pages to allocate as calculated by the size specified to occupy in memory
    UINT64     SegmentPageCount        = EFI_SIZE_TO_PAGES(ProgramHeader->p_memsz);
    // Values to store info about the ELF post segment zero fill area
    EFI_PHYSICAL_ADDRESS ZeroFillStart = ProgramHeader->p_paddr + ProgramHeader->p_filesz;
    UINT64               ZeroFillSize  = ProgramHeader->p_memsz - ProgramHeader->p_filesz;
    
    // Set kernel file pointer to program segment location
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage, ProgramHeader->p_offset);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error setting kernel file position to program segment offset: ");
        print_hex(ProgramHeader->p_offset, true);
        print(L"\r\n");
    }

    if (ProgramHeader->p_filesz > 0) {
        // The buffer read will be the exact size of the segment on file
        BufferSize = ProgramHeader->p_filesz;

        // Allocate memory to the buffer that will store the program segment
        status = SystemTable->BootServices->AllocatePool(EfiLoaderData,
                                                         BufferSize,
                                                         &ProgramSegmentBuffer);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while allocating memory for a kernel program segment buffer\r\n");
            return status;
        }
    
        // Allocate pages in memory to store the program segment stored in the buffer
        status = SystemTable->BootServices->AllocatePages(AllocateAddress,
                                                          EfiLoaderData,
                                                          SegmentPageCount,
                                                          (EFI_PHYSICAL_ADDRESS*)&(ProgramHeader->p_paddr));
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while allocating pages for a kernel program segment\r\n");
            return status;
        }

        // Read the segment into the buffer allocated previously
        status = KernelImage->Read((struct EFI_FILE_PROTOCOL*)KernelImage,
                                   &BufferSize,
                                   (void*)ProgramSegmentBuffer);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while reading a program segment from the kernel image file\r\n");
            return status;
        }

        // Copy the contents of the buffer into the allocated pages
        status = SystemTable->BootServices->CopyMem((void*)ProgramHeader->p_paddr,
                                                    ProgramSegmentBuffer,
                                                    ProgramHeader->p_filesz);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while copying memory from program segment buffer into memory page\r\n");
            return status;
        }
    
        // Cleanup the memory allocated for the buffer
        status = SystemTable->BootServices->FreePool((void*)ProgramSegmentBuffer);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while freeing the buffer for a kernel program segment\r\n");
            return status;
        }
    }

    // The ELF standard requires all memory following the segment to be zero filled
    // e.g. the location past the segment when the size in memory exceeds the filesize
    if (ZeroFillSize > 0) {
        status = SystemTable->BootServices->SetMem((void*)ZeroFillStart,
                                                   ZeroFillSize,
                                                   0);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while zero filling the memory location after a kernel program segment\r\n");
            return status;
        }
    }

    return status;
}

EFI_STATUS load_program_segments(EFI_SYSTEM_TABLE* SystemTable, 
                                 EFI_FILE_PROTOCOL* KernelImage, 
                                 Elf64_Ehdr* KernelHeader, 
                                 Elf64_Phdr* KernelProgramHeaders) {
    // To store the returned status of various functions
    EFI_STATUS status            = EFI_SUCCESS;
    // Contains the number of ELF program headers contained in the kernel image
    UINT16     NumProgramHeaders = KernelHeader->e_phnum;
    // Keep track of the number of loadable program segments in the image
    UINT16     NumSegmentsLoaded = 0;

    // Exit when no program segments are found (e.g. empty kernel image)
    if (NumProgramHeaders == 0) {
        print(L"Fatal: No program segments found within the kernel image\r\n");
        return EFI_INVALID_PARAMETER;
    }

    for (UINT16 SegNum = 0; SegNum < NumProgramHeaders; ++SegNum) {
        // Load all segments that are marked for loading
        if (KernelProgramHeaders[SegNum].p_type == PT_LOAD) {
            status = load_segment(SystemTable,
                                  KernelImage,
                                  &KernelProgramHeaders[SegNum]);
            if (EFI_ERROR(status)) {
                return status;
            }
            ++NumSegmentsLoaded;
        }
    }

    if (NumSegmentsLoaded == 0) {
        print(L"Fatal: No LOADABLE program segments found within the kernel image\r\n");
        return EFI_NOT_FOUND;
    }

    return status;
}

void print_elf_info(void* KernelHeader) {
    Elf64_Ehdr* header = (Elf64_Ehdr*)KernelHeader;
    print(L"Debug: Kernel ELF header info:\r\n");
    
    // Magic
    print(L"  Magic: ");
    for (int i = 0; i <= 3; i++) {
        print_hex(header->e_ident[i], false);
        print(L" ");
    }
    print(L"\r\n");
    // Class
    print(L"  Class: ");
    print_hex(header->e_ident[EI_CLASS], true);
    print(L"\r\n");
    // Data
    print(L"  Data: ");
    print_hex(header->e_ident[EI_DATA], true); 
    print(L"\r\n");
    // Version
    print(L"  Version: ");
    print_hex(header->e_ident[EI_VERSION], true);
    print(L"\r\n");
    // OS/ABI
    print(L"  OS/ABI: ");
    print_hex(header->e_ident[EI_OSABI], true);
    print(L"\r\n");
    // ABI Version
    print(L"  ABI Version: ");
    print_hex(header->e_ident[EI_ABIVERSION], true);
    print(L"\r\n");
    // Type
    print(L"  Type: ");
    print_hex(header->e_type, true);
    print(L"\r\n");
    // Machine
    print(L"  Machine: ");
    print_hex(header->e_machine, true);
    print(L"\r\n");
    // Version
    print(L"  Version: ");
    print_hex(header->e_version, true);
    print(L"\r\n");
    // Entry point address
    print(L"  Entry point address: ");
    print_hex(header->e_entry, true);
    print(L"\r\n");
    // Start of program headers
    print(L"  Start of program headers: ");
    print_hex(header->e_phoff, true);
    print(L"\r\n");
    // Start of section headers
    print(L"  Start of section headers: ");
    print_hex(header->e_shoff, true);
    print(L"\r\n");
    // Flags
    print(L"  Flags: ");
    print_hex(header->e_flags, true);
    print(L"\r\n");
    // Size of this header
    print(L"  Size of this header: ");
    print_hex(header->e_ehsize, true);
    print(L"\r\n");
    // Size of program headers
    print(L"  Size of program headers: ");
    print_hex(header->e_phentsize, true);
    print(L"\r\n");
    // Number of program headers
    print(L"  Number of program headers: ");
    print_num(header->e_phnum);
    print(L"\r\n");
    // Size of section headers
    print(L"  Size of section headers: ");
    print_hex(header->e_shentsize, true);
    print(L"\r\n");
    // Number of section headers
    print(L"  Number of section headers: ");
    print_num(header->e_shnum);
    print(L"\r\n");
    // Section header string table index
    print(L"  Section header string table index: ");
    print_hex(header->e_shstrndx, true);
    print(L"\r\n");
}
