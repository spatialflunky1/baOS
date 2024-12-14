/*
 * The bootloader for baOS
 */

#include <efi.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    //
    // Bootloader vars
    //
    
    // To store the returned status of various functions
    EFI_STATUS                    status                 = EFI_SUCCESS;
    // To store graphics handles
    EfiGraphicsService            GraphicsService;
    // To store the address of the opened protocol
    EFI_GRAPHICS_OUTPUT_PROTOCOL* GraphicsOutputProtocol = NULL;


    //
    // Code Initialization
    //

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
        return status;
    }


    //
    // Initialize Graphics Output Protocol
    //
    #ifdef __DEBUG__
        print(L"DEBUG: Initializing the graphics output protocol service\r\n");
    #endif

    // Get the graphics service handle buffers
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
        return status;
    }
    if (GraphicsService.handle_count == 0) {
        print(L"Fatal: Error locating any graphics output protocol handles\r\n");
        return status;
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
        while(1);
    }
    while (1);


    //
    // Open the Graphics Output Protocol
    //
    

    //
    // Save Graphics Info for Later Use in the Kernel
    //
   

    //
    // Obtain ACPI XSDP Table
    //
    

    //
    // Initialize Simple File System Protocol
    //
    

    //
    // Open EFI Boot Volume
    //
    

    //
    // Load Kernel
    //
    

    //
    // Exit EFI
    //
    

    //
    // Enter Kernel
    //
    

    // Return on error as the system should never reach this point
    return EFI_LOAD_ERROR;
}
