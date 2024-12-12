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

void print_num(UINT64 n) {
    // Create a buffer acting as a stack for digits
    CHAR16 StringStack[UINT64_DEC_MAXDIGIT];
    INT16  StackTop = -1;
    
    // Add 0 to the stack when the number is 0 as the loop is skipped
    if (n == 0) {
        ++StackTop;
        StringStack[StackTop] = L'0';
    }

    // Store the base 10 digits in the stack
    while (n != 0) {
        ++StackTop;
        StringStack[StackTop] = (CHAR16)((n%10) + 48);
        n /= 10;
    }
    
    // Reverse the character elements of the stack to correctly match the number
    INT16  TempIndex = StackTop;
    CHAR16 TempChar;
    for (; TempIndex > (StackTop / 2); --TempIndex) {
        TempChar = StringStack[StackTop - TempIndex];
        StringStack[StackTop - TempIndex] = StringStack[TempIndex];
        StringStack[TempIndex] = TempChar;
    }
    
    // Add the null byte
    StringStack[StackTop + 1] = L'\0';
    
    // Print the created string
    print(StringStack);
}

void print_hex(UINT64 h, bool ZeroX) {
    // Create a buffer acting as a stack for digits
    CHAR16 StringStack[UINT64_DEC_MAXDIGIT];
    INT16  StackTop = -1; 
    
    // Add 0 to the stack when the number is 0 as the loop is skipped
    if (h == 0) {
        ++StackTop;
        StringStack[StackTop] = L'0';
    }

    // Store the base 10 digits in the stack
    while (h != 0) {
        ++StackTop;
        if ((h%16) < 10) {
            StringStack[StackTop] = (CHAR16)((h%16) + 48);
        }
        else {
            // -10: The first alphabetic letter is located at 65 (A),
            //      10 marks the first so it needs to be subtracted
            //      before conversion
            StringStack[StackTop] = (CHAR16)((h%16) + (65 - 10));
        }
        h /= 16;
    }

    // Add the 0x to the string when necessary
    // Added here in reverse order as the string is reversed
    if (ZeroX) {
        StringStack[StackTop+1] = L'x';
        StringStack[StackTop+2] = L'0';
        StackTop += 2;
    }
    
    // Reverse the character elements of the stack to correctly match the number
    INT16  TempIndex = StackTop;
    CHAR16 TempChar;
    for (; TempIndex > (StackTop / 2); --TempIndex) {
        TempChar = StringStack[StackTop - TempIndex];
        StringStack[StackTop - TempIndex] = StringStack[TempIndex];
        StringStack[TempIndex] = TempChar;
    }
    
    // Add the null byte
    StringStack[StackTop + 1] = L'\0';
    
    // Print the created string
    print(StringStack);
}
