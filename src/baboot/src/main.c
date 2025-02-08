/*
 * The bootloader for baOS
 */

#include <firmware/efi.h>
#include <kernel_loader.h>

#define KERNEL_FILEPATH L"\\baos_kernel.elf"

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
    // Address that the kernel is loaded into 
    EFI_PHYSICAL_ADDRESS*            KernelEntryPoint                 = NULL;

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
    while (1);
    

    //
    // Exit EFI boot services
    //
    

    //
    // Enter Kernel
    //
    

    // Return on error as the system should never reach this point
    return EFI_LOAD_ERROR;
}
