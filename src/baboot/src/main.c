/*
 * The bootloader for baOS
 */

#include <efi.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    // Bootloader vars
    EFI_STATUS status = EFI_SUCCESS;

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
    print_hex(0xBADBEEF, true);
    while (1);

    //
    // Initialize Graphics Output Protocol
    //
    

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
