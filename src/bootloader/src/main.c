/*
 * The bootloader for baOS
 */

#include <firmware/efi.h>
#include <kernel_loader.h>

#define KERNEL_FILEPATH L"\\baos_kernel.elf"

// Forward declaration
EFI_STATUS ReadMemoryMap(EFI_SYSTEM_TABLE* SystemTable,
                        void**            MemoryMap,
                        UINT64*           MemoryMapSize,
                        UINT64*           MemoryMapKey,
                        UINT64*           DescriptorSize,
                        UINT32*           DescriptorVersion);

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    //
    // Bootloader vars
    //
    
    // To store the returned status of various functions
    EFI_STATUS                       status                           = EFI_SUCCESS;
    // To store graphics handles recieved from firmware
    EfiGraphicsService               GraphicsService;
    // Provides the ability to set video modes and access the framebuffer
    EFI_GRAPHICS_OUTPUT_PROTOCOL*    GraphicsOutputProtocol           = NULL;
    // The UEFI standard requires a variable to store GUID values
    EFI_GUID                         gEfiLoadedImageProtocolGuid      = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID                         gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    // Pointer to the interface of the loaded image protocol, used to get FS root
    EFI_LOADED_IMAGE_PROTOCOL*       LoadedImageProtocol              = NULL;
    // Pointer to the structure that contains the function to open fs volumes
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystemProtocol               = NULL;
    // To store the address to the root of the boot volume
    EFI_FILE_PROTOCOL*               RootFileSystem                   = NULL;
    // Address that the kernel is loaded into, and which the entry function is located 
    EFI_PHYSICAL_ADDRESS             KernelEntryPoint                 = 0;
    // Function pointer to load the function as referenced in the previous comment
    int (*kernel_entry_point)(KERNEL_BOOT_INFO* BootInfo)             = NULL;
    // Contains info on the currently operating firmware memor map
    EFI_MEMORY_DESCRIPTOR*           MemoryMap                        = NULL;
    UINT64                           MemoryMapSize                    = 0;
    UINT64                           MemoryMapKey                     = 0;
    UINT64                           DescriptorSize                   = 0;
    UINT32                           DescriptorVersion                = 0;
    // Info from the bootloader that needs to be transfered to the kernel will be kept in this structure
    KERNEL_BOOT_INFO                 BootInfo;
    // The buffer of pages for the kernel program segments
    EFI_PHYSICAL_ADDRESS             ProgramSegment_buf               = 0;

    // Assign statically defined sys_table variable for use with EFI functions
    initializeVideoSysTableVar(SystemTable);
    // Notify user of start
    print(L"Booting baOS...\r\n");


    //
    // Disable Firmware Watchdog Timer (5 min "inactivity" == reboot)
    //

    #ifdef __DEBUG__
        print(L"DEBUG: Disabling Firmware Watchdog Timer\r\n");
    #endif
    status = SystemTable->BootServices->SetWatchdogTimer(0,0,0,NULL);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error while disabling watchdog\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }

    //
    // Initialize Graphics Output Protocol
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Initializing the graphics output protocol service\r\n");
    #endif
    // Get all graphics service handle buffers for the active console
    #ifdef __DEBUG__
        print(L"  DEBUG: Locating the graphics service handle buffers\r\n");
    #endif
    status = SystemTable->BootServices->LocateHandleBuffer(
            ByProtocol,
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            &GraphicsService.handle_count,
            &GraphicsService.handle_buffer);
    if (EFI_ERROR(status)) {
        if (status != EFI_NOT_FOUND) {
            print(L"Fatal: Error initializing graphics service\r\n");
        }
        else {
            print(L"Fatal: Unable to locate graphics device\r\n");
        }
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }
    if (GraphicsService.handle_count == 0) {
        print(L"Fatal: Error locating any graphics output protocol handles\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }

    // Open the graphics output protocol on the first obtained buffer
    #ifdef __DEBUG__
        print(L"  DEBUG: Opening the graphics output protocol on the first obtained buffer\r\n");
    #endif
    status = SystemTable->BootServices->OpenProtocol(
            GraphicsService.handle_buffer[0],
            &gEfiGraphicsOutputProtocolGuid,
            (void**)&GraphicsOutputProtocol,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Failed to open graphics output protocol on the current console\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }

    //
    // Open the first Graphics Output Protocol from the returned buffer
    //
    status = SystemTable->BootServices->OpenProtocol(
            GraphicsService.handle_buffer[0],
            &gEfiGraphicsOutputProtocolGuid,
            (void**)&GraphicsOutputProtocol,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Failed to open graphics output protocol on the current console\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }
    
    //
    // Set the graphics mode by resolution
    //

    // Set the resolution to the 2nd supported mode
    status = GraphicsOutputProtocol->SetMode(
            (struct EFI_GRAPHICS_OUTPUT_PROTOCOL*)GraphicsOutputProtocol,
            2);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error setting graphics output mode\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }
   
    // Reset the current text mode output device 
    status = SystemTable->ConOut->Reset((struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*)SystemTable->ConOut, true);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error resetting console graphics device\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }

    // Return when framebuffer is null
    if ((void*)GraphicsOutputProtocol->Mode->FrameBufferBase == NULL) {
        print(L"Fatal: No framebuffer address was found for the kernel\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }
    

    //
    // Save Graphics Info for Later Use in the Kernel
    //
    BootInfo.VideoModeInfo.FramebufferPointer =
        (void*)GraphicsOutputProtocol->Mode->FrameBufferBase;

    BootInfo.VideoModeInfo.HorizontalResolution =
        GraphicsOutputProtocol->Mode->Info->HorizontalResolution;

    BootInfo.VideoModeInfo.VerticalResolution =
        GraphicsOutputProtocol->Mode->Info->VerticalResolution;

    BootInfo.VideoModeInfo.PixelsPerScanline =
        GraphicsOutputProtocol->Mode->Info->PixelsPerScanline;

    //
    // Obtain ACPI XSDP Table
    //
    

    //
    // Initialize Simple File System Protocol
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Loading EFI filesystem\r\n");
    #endif
    status = SystemTable->BootServices->OpenProtocol(
            ImageHandle,
            &gEfiLoadedImageProtocolGuid,
            (void**)&LoadedImageProtocol,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);                 
    if (EFI_ERROR(status)) {
        print(L"Fatal: Firmware error opening the loaded image protocol from the EFI image\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }

    status = SystemTable->BootServices->OpenProtocol(
            LoadedImageProtocol->DeviceHandle,
            &gEfiSimpleFileSystemProtocolGuid,
            (void**)&FileSystemProtocol,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Unable to open the file system protocol from the device listed by the EFI image\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }

    //
    // Open EFI Boot Volume
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Opening EFI boot volume\r\n");
    #endif
    status = FileSystemProtocol->OpenVolume(
            (struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*)FileSystemProtocol,
            (struct EFI_FILE_PROTOCOL**)&RootFileSystem);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error when opeining EFI boot volume\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while(1);
    }
    #ifdef __DEBUG__
        print(L"DEBUG: EFI boot volume root mounted at: ");
        print_hex((UINT64)RootFileSystem, 1);
        print(L"\r\n");
    #endif

    //
    // Load Kernel
    //
    status = load_kernel(RootFileSystem, KERNEL_FILEPATH, &KernelEntryPoint, SystemTable, &ProgramSegment_buf);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Unable to load baOS kernel\r\n");
        while(1);
    }

    BootInfo.KernelBase = (void*)ProgramSegment_buf;

    /*-------------------------------WARNING-------------------------------
     * Any firmware print statements after a successful call to
     * ExitBootServices will cause a crash
     *---------------------------------------------------------------------*/

    //
    // Get the firmware memory map and exit EFI boot services (allow for 5 retries)
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Saving the current firmware memory map and exiting EFI boot services\r\n");
    #endif
    int retries = 5;
    while (EFI_ERROR(SystemTable->BootServices->ExitBootServices(ImageHandle, MemoryMapKey)) && retries != 0) {
        // Free the memory map buffer (ignore errors, may be empty)
        SystemTable->BootServices->FreePool((void*)MemoryMap);
        DescriptorSize = 0;
        
        // Request the memory map once to obtain the size
        status = SystemTable->BootServices->GetMemoryMap(&MemoryMapSize,
                                                     (struct EFI_MEMORY_DESCRIPTOR*)MemoryMap,
                                                     &MemoryMapKey,
                                                     &DescriptorSize,
                                                     &DescriptorVersion);
        // The second conditional is to ignore the error given when the buffer is too small
        // This is because the buffer size starts with 0 in order to obtain the required size
        if (EFI_ERROR(status) && ((status & 0x5) == 0)) {
            print(L"Fatal: Error while requesting the memory map from firmware\r\n");
            print_hex(status, true);
            print(L"\r\n");
            while(1);
        }

        // Allocate a buffer to contain the map using the previously obtained size
        // 2 more page memory descriptors may be created while allocated the new pool of memory
        MemoryMapSize += (2 * DescriptorSize);
        status = SystemTable->BootServices->AllocatePool(EfiLoaderData,
                                                     MemoryMapSize,
                                                     (void**)&MemoryMap);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while allocating memory for the buffer to store the firmware memory map\r\n");
            print_hex(status, true);
            print(L"\r\n");
            while(1);
        }

        // Read the memory map into the previously allocated buffer
        status = SystemTable->BootServices->GetMemoryMap(&MemoryMapSize,
                                                     (struct EFI_MEMORY_DESCRIPTOR*)MemoryMap,
                                                     &MemoryMapKey,
                                                     &DescriptorSize,
                                                     &DescriptorVersion);
        if (EFI_ERROR(status)) {
            print(L"Fatal: Error while requesting the memory map from firmware\r\n");
            print_hex(status, true);
            print(L"\r\n");
            while(1);
        }

        retries--;
    }
    if (EFI_ERROR(status)) {
        print(L"Fatal: An unexpected error ocurred while exiting EFI boot services\r\n");
        print_hex(status, true);
        print(L"\r\n");
        while (1);
    }

    //
    // Save memory map for kernel
    //
    BootInfo.MemoryMap = MemoryMap;
    BootInfo.MemoryMapSize = MemoryMapSize;
    BootInfo.MemoryMapDescriptorSize = DescriptorSize;

    //
    // Enter Kernel
    //
    kernel_entry_point = (int (*)(KERNEL_BOOT_INFO* BootInfo))KernelEntryPoint;

    // The kernel is compiled using the System V calling convention which does not match the bootloader
    // BootInfo must therefore be passed into the RDI register directly
    __asm__ volatile ("mov %0, %%rdi" :: "r"(&BootInfo));
    
    // Can still pass as a parameter as a just in case measure
    int return_code = kernel_entry_point(&BootInfo);
    
    if (return_code == 0) {
        return EFI_SUCCESS;
    }
    else {
        return EFI_LOAD_ERROR;
    }
}
