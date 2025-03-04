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
        print_hex(status, true);
        print(L"\r\n");
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
        print_hex(status, true);
        print(L"\r\n");
        return status;
    }
    
    // Read ELF header
    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage,
            &BufferSize,
            *KernelELFHeader_ptr);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error reading kernel ELF header\r\n");
        print_hex(status, true);
        print(L"\r\n");
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
        print_hex(status, true);
        print(L"\r\n");
        return status;
    }

    // Set kernel image file pointer position to the start of the program headers (program header offset)
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage, ProgramHeadersOffset);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error setting kernel image file pointer position\r\n");
        print_hex(status, true);
        return status;
    }

    // Read ELF program headers
    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage,
            &BufferSize,
            *KernelELFProgramHeaders_ptr);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error reading kernel ELF program headers\r\n");
        print_hex(status, true);
        print(L"\r\n");
        return status;
    }

    return status;
}

EFI_STATUS load_program_segments(EFI_SYSTEM_TABLE*     SystemTable, 
                                 EFI_FILE_PROTOCOL*    KernelImage, 
                                 Elf64_Ehdr*           KernelHeader, 
                                 Elf64_Phdr*           KernelProgramHeaders,
                                 EFI_PHYSICAL_ADDRESS* KernelEntryPoint) {
    // To store the returned status of various functions
    EFI_STATUS           status              = EFI_SUCCESS;
    // Contains the number of ELF program headers contained in the kernel image
    UINT16               NumProgramHeaders   = KernelHeader->e_phnum;
    // Keep track of the number of loadable program segments in the image
    UINT16               NumLoadableSegments = 0;
    // Maximum alignment found in the program segments, starting with the default page alignment
    UINT64               MaxAlignment        = EFI_PAGE_SIZE;
    // Store the beginning and end address of the program headers
    UINT64               HdrStart            = 0;
    UINT64               HdrEnd              = 0;
    // Store the range of memory to allocate for the page
    UINT64               LowerMem            = UINT64_MAX;
    UINT64               UpperMem            = 0;
    // The number of pages to be allocated for the previous buffer
    UINT64               NumPages            = 0;
    // The buffer of pages for the kernel program segments
    EFI_PHYSICAL_ADDRESS ProgramSegment_buf  = 0;
    // The temporary buffer used to copy memory into pages
    EFI_PHYSICAL_ADDRESS TempSegment_buf     = 0;
    // To store the size of different buffers being addressed
    UINT64               BufferSize          = 0;
    // To locate the relative position inside the page buffer for kernel program segments
    UINT64               RelativeOffset      = 0;

    // Exit when no program segments are found (e.g. empty kernel image)
    if (NumProgramHeaders == 0) {
        print(L"Fatal: No program segments found within the kernel image\r\n");
        return EFI_INVALID_PARAMETER;
    }

    for (UINT16 SegNum = 0; SegNum < NumProgramHeaders; ++SegNum) {
        // Skip all segments that are not marked for loading
        if (KernelProgramHeaders[SegNum].p_type != PT_LOAD) { continue; }
        NumLoadableSegments++;
        #ifdef __DEBUG__
            print(L"  DEBUG: Loading segment (p): ");
            print_hex(KernelProgramHeaders[SegNum].p_paddr, true);
            print(L"  (v): ");
            print_hex(KernelProgramHeaders[SegNum].p_vaddr, true);
            print(L"\r\n");
        #endif
        
        // Save the maximum alignment
        if (MaxAlignment < KernelProgramHeaders[SegNum].p_align) {
            MaxAlignment = KernelProgramHeaders[SegNum].p_align;
        }
        // Mark the beginning and end of the header (aligned)
        HdrStart = KernelProgramHeaders[SegNum].p_vaddr;
        HdrEnd   = HdrStart + KernelProgramHeaders[SegNum].p_memsz + (MaxAlignment - 1);
        // Limit the previous range to the current alignment
        HdrStart &= ~(MaxAlignment-1);
        HdrEnd   &= ~(MaxAlignment-1);
        // Expand the memory range if necessary
        if (HdrStart < LowerMem) { LowerMem = HdrStart; }
        if (HdrEnd   > UpperMem) { UpperMem = HdrEnd;   }
    }

    if (NumLoadableSegments == 0) {
        print(L"Fatal: No LOADABLE program segments found within the kernel image\r\n");
        return EFI_NOT_FOUND;
    }

    //
    // Allocate the pages as computed in the previous loop
    //
    NumPages = ((UpperMem-LowerMem) + (EFI_PAGE_SIZE-1)) / EFI_PAGE_SIZE;
    status = SystemTable->BootServices->AllocatePages(AllocateAnyPages, 
                                                      EfiLoaderCode, 
                                                      NumPages, 
                                                      &ProgramSegment_buf);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Failed to allocate pages for the kernel program segments\r\n");
        return status;
    }

    // Zero set the entire buffer to cover the requirement that the area post-segment must be zero filled
    status = SystemTable->BootServices->SetMem((void*)ProgramSegment_buf, UpperMem-LowerMem, 0);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error while zero setting the pages allocated for the kernel program segments\r\n");
        return status;
    }

    for (UINT16 SegNum = 0; SegNum < NumProgramHeaders; ++SegNum) {
        // Skip all segments that are not marked for loading
        if (KernelProgramHeaders[SegNum].p_type != PT_LOAD) { continue; }

        //
        // Copy segment to the newly allocated pages
        //
        
        // Allocate temporary buffer
        BufferSize = KernelProgramHeaders[SegNum].p_filesz;
        status = SystemTable->BootServices->AllocatePool(EfiLoaderData,
                                                         BufferSize,
                                                         (void**)&TempSegment_buf);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Failed to allocate a temporary buffer to store the kernel program segment\r\n");
            return status;
        }

        // Set kernel image to location of program segment
        status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage,
                                          KernelProgramHeaders[SegNum].p_offset);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error setting position of kernel image file pointer\r\n");
            return status;
        }

        // Read segment into temp buffer
        status = KernelImage->Read((struct EFI_FILE_PROTOCOL*)KernelImage,
                                   &BufferSize,
                                   (void*)TempSegment_buf);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error reading from kernel image file\r\n");
            return status;
        }

        // Copy temp buffer into page buffer
        RelativeOffset = KernelProgramHeaders[SegNum].p_vaddr - LowerMem;
        status = SystemTable->BootServices->CopyMem((void*)(ProgramSegment_buf + RelativeOffset),
                                                    (void*)TempSegment_buf,
                                                    KernelProgramHeaders[SegNum].p_filesz);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error copying memory from kernel program header temp buffer to page buffer\r\n");
            return status;
        }
        
        // Free temp buffer
        status = SystemTable->BootServices->FreePool((void*)TempSegment_buf);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while freeing temp buffer for kernel program segment\r\n");
            return status;
        }
    }

    // Update the kernel entry point to match the loaded kernel
    *KernelEntryPoint = (EFI_PHYSICAL_ADDRESS)((UINT8*)ProgramSegment_buf + (KernelHeader->e_entry - LowerMem));

    return status;
}

void print_elf_info(void* KernelHeader) {
    Elf64_Ehdr* header = (Elf64_Ehdr*)KernelHeader;
    print(L"DEBUG: Kernel ELF header info:\r\n");
    
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
