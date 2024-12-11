#include <efi_video.h>

void initializeVideoSysTableVar(EFI_SYSTEM_TABLE *SystemTable) {
    sys_table = SystemTable;
}

void print(CHAR16* s) {
    if (sys_table == NULL) {
        // This should only be reached on programatic error
        // Unless something is terribly wrong in the firmware
        // If this point is reached there is no way to notify the user 
        // Therefor the system will simply halt
        while (1);
    }
    sys_table->ConOut->OutputString(
        (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*)sys_table->ConOut,
        s);
}
