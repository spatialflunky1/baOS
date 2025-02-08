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
    EFI_GUID                         gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    // Pointer to the structure that contains the function to open fs volumes
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystemProtocol;
    // To store the address to the root of the boot volume
    EFI_FILE_PROTOCOL*               RootFileSystem;
    // Address that the kernel is loaded into, and which the entry function is located 
    EFI_PHYSICAL_ADDRESS*            KernelEntryPoint                 = NULL;
    // Function pointer to load the function as referenced in the previous comment
    int (*kernel_entry_point)(void);
    // Contains info on the currently operating firmware memor map
    EFI_MEMORY_DESCRIPTOR*           MemoryMap                        = NULL;
    UINT64                           MemoryMapSize                    = 0;
    UINT64                           MemoryMapKey                     = 0;
    UINT64                           DescriptorSize                   = 0;
    UINT32                           DescriptorVersion                = 0;

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
        while(1);
    }
    
    //
    // Initialize Graphics Output Protocol
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Initializing the graphics output protocol service\r\n");
    #endif
    // Get all graphics service handle buffers for the active console
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
        while(1);
    }
    if (GraphicsService.handle_count == 0) {
        print(L"Fatal: Error locating any graphics output protocol handles\r\n");
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
        while(1);
    }
   
    // Reset the current text mode output device 
    status = SystemTable->ConOut->Reset((struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*)SystemTable->ConOut, true);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Error resetting console graphics device\r\n");
        while(1);
    }

    // Return when framebuffer is null
    if ((void*)GraphicsOutputProtocol->Mode->FrameBufferBase == NULL) {
        print(L"Fatal: No framebuffer address was found for the kernel\r\n");
        while(1);
    }
    
    //
    // Save Graphics Info for Later Use in the Kernel
    //
   

    //
    // Obtain ACPI XSDP Table
    //
    

    //
    // Initialize Simple File System Protocol
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Loading EFI filesystem\r\n");
    #endif
    status = SystemTable->BootServices->LocateProtocol(
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            (void**)&FileSystemProtocol);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Firmware error locating a protocol to load the EFI filesystem\r\n");
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
    status = load_kernel(RootFileSystem, KERNEL_FILEPATH, KernelEntryPoint, SystemTable);
    if (EFI_ERROR(status)) {
        print(L"Fatal: Unable to load baOS kernel\r\n");
        while(1);
    }

    //
    // Get the firmware memory map
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Saving the current firmware memory map and exiting EFI boot services\r\n");
    #endif
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
        while(1);
    }
    /*-------------------------------WARNING-------------------------------
     * FROM THIS POINT ANY MESSAGE PRINTED WILL AFFECT THE MemoryMapKey AND
     * CAUSE ExitBootServices TO FAIL
     *
     * Furthermore, any firmware print statements after a successful call to
     * ExitBootServices will cause a crash
     *---------------------------------------------------------------------*/

    //
    // Exit EFI boot services
    //
    status = SystemTable->BootServices->ExitBootServices(ImageHandle, MemoryMapKey);
    if (EFI_ERROR(status)) {
        print(L"Fatal: An unexpected error ocurred while exiting EFI boot services\r\n");
        print_hex(status, true);
        while (1);
    }    

    //
    // Enter Kernel
    //
    kernel_entry_point = (int (*)(void))*KernelEntryPoint;
    int return_code = kernel_entry_point();
    
    if (return_code == 0) {
        return EFI_SUCCESS;
    }
    else {
        return EFI_LOAD_ERROR;
    }
}
