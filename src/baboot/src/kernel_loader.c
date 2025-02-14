#include <kernel_loader.h>

EFI_STATUS load_kernel(EFI_FILE_PROTOCOL* RootFileSystem, 
                       CHAR16* KernelFilename, 
                       EFI_PHYSICAL_ADDRESS* KernelEntryPoint, 
                       EFI_SYSTEM_TABLE* SystemTable) {
    // To store the returned status of various functions
    EFI_STATUS         status               = EFI_SUCCESS;
    // Stores the address of the loaded kernel image file in memory
    EFI_FILE_PROTOCOL* KernelImage          = NULL;
    // Buffer to store the ELF identity
    UINT8*             ELFIDBuffer          = NULL;
    // ELF headers
    Elf64_Ehdr*        KernelHeader         = NULL;
    Elf64_Phdr*        KernelProgramHeaders = NULL;
    
    // Preliminary parameter NULL check
    if (RootFileSystem == NULL) {
        print(L"Fatal: Filesystem not located in memory");
        return EFI_NOT_FOUND;
    }
    if (KernelFilename == NULL) {
        print(L"Fatal: No kernel filename specified for loading");
        return EFI_NOT_FOUND;
    }
    if (SystemTable == NULL) {
        // How did we get here???
        // Error message shouldn't be able to print if this is true anyways
        print(L"Fatal: Firmware system table not found");
        return EFI_NOT_FOUND;
    }

    print(L"Loading baOS kernel...\r\n");
    //
    // Load kernel image into memory (for reading info only)
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Opening kernel image\r\n");
    #endif
    status = RootFileSystem->Open(
            (struct EFI_FILE_PROTOCOL*)RootFileSystem,
            (struct EFI_FILE_PROTOCOL**)&KernelImage,
            KernelFilename,
            EFI_FILE_MODE_READ,
            EFI_FILE_READ_ONLY);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error opening kernel image\r\n");
        if (status == EFI_NOT_FOUND || KernelImage == NULL) {
            print(L"Error: Kernel image file not found\r\n");
        }
        print_hex(status, true);
        return status;
    }
    if (KernelImage == NULL) {
        print(L"Fatal: Unknown error locating kernel image\r\n");
        print_hex(status, true);
        return EFI_NOT_FOUND;
    }
    #ifdef __DEBUG__
        print(L"DEBUG: Kernel executable loaded into memory at: ");
        print_hex((UINT64)KernelImage, true);
        print(L"\r\n");
    #endif

    //
    // Read ELF (executable linking format) information
    //
    
    // Allocate a buffer for the ELF identity
    status = SystemTable->BootServices->AllocatePool(
            EfiLoaderData,
            EI_NIDENT,
            (void**)&ELFIDBuffer);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Unable to allocate memory for kernel ELF identity\r\n");
        print_hex(status, true);
        return status;
    }

    // Read and validate the ELF identity
    status = read_elf_identity(KernelImage, ELFIDBuffer);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Unable to read kernel ELF identity\r\n");
        return status;
    }

    status = validate_elf_identity(ELFIDBuffer);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Invalid ELF identity in kernel imager\r\n");
        return status;
    }

    // Clear the memory used for the ELF identity
    status = SystemTable->BootServices->FreePool(ELFIDBuffer);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Unable to free buffer used by kernel ELF identity\r\n");
        print_hex(status, true);
        return status;
    }
    
    #ifdef __DEBUG__
        print(L"Reading kernel ELF headers\r\n");
    #endif
    // Read in the ELF headers and program headers
    status = read_elf_headers(SystemTable, 
                      KernelImage, 
                      &KernelHeader, 
                      &KernelProgramHeaders);
    if (EFI_ERROR(status)) {
        // Error printed within function call above
        return status;
    }

    // Print details of the executable file when in debug mode
    #ifdef __DEBUG__
        print_elf_info(KernelHeader);
    #endif

    // Assign the variable that keeps track of the kernel's entry point address
    *KernelEntryPoint = ((Elf64_Ehdr*)KernelHeader)->e_entry;

    //
    // Load the segments of the kernel into memory
    //

    #ifdef __DEBUG__
        print(L"DEBUG: Loading kernel image program segments\r\n");
    #endif

    status = load_program_segments(SystemTable, KernelImage, KernelHeader, KernelProgramHeaders);
    if (EFI_ERROR(status)) { 
        print(L"Fatal: Error loading kernel image program segments\r\n");
        return status;
    }

    //
    // Cleanup
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Cleaning memory from reading kernel image\r\n");
    #endif

    // Close the open kernel image file
    status = KernelImage->Close((struct EFI_FILE_PROTOCOL*)KernelImage);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error while closing closing the previously opened kernel image file\r\n");
        print_hex(status, true);
        return status;
    }

    // Free the buffer storing the ELF header
    status = SystemTable->BootServices->FreePool(KernelHeader);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error while freeing the buffer storing the kernel ELF header\r\n");
        print_hex(status, true);
        return status;
    }

    // Free the buffer storing the ELF program headers
    status = SystemTable->BootServices->FreePool(KernelProgramHeaders);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error while freeing the buffer storing the kernel ELF program headers\r\n");
        print_hex(status, true);
        return status;
    }

    #ifdef __DEBUG__
        print(L"DEBUG: Kernel successfully loaded into memory\r\n");
    #endif

    return status;
}
