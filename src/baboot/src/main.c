#include <efi.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    // Assign statically defined sys_table variable for use with EFI functions
    initializeVideoSysTableVar(SystemTable); 
    print(L"Booting baOS...\r\n");
    print_hex(0xBEEF, true);
    while(1);
    // Bootloader vars

    //
    // Disable Watchdog Timer (5 min "inactivity" == reboot)
    //
    

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
