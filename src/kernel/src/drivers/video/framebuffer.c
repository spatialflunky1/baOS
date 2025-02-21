#include <drivers/video/framebuffer.h>

//
// Global Vars
//

// Framebuffer info
BOOT_VIDEO_MODE_INFO* FramebufferInfo = NULL;

// Framebuffer text info
uint16_t             text_col_px      = 0;
uint16_t             text_line        = 0;
uint16_t             max_text_line    = 0;
unsigned int         text_color       = rgb(255,255,255);
unsigned int         background_color = rgb(0,0,0);
unsigned int         font_size        = 0;
const unsigned char* font_array       = NULL;

void initialize_framebuffer(BOOT_VIDEO_MODE_INFO* VMI) {
    FramebufferInfo = VMI; 
    set_frambuffer_font_size(16);
    blank_framebuffer();
}

void blank_framebuffer(void) {
    uint32_t* pos;
    for (uint64_t row = 0; row < FramebufferInfo->VerticalResolution; row++) {
        for (uint64_t col = 0; col < FramebufferInfo->PixelsPerScanline; col++) {
            pos = FramebufferInfo->FramebufferPointer;
            // Set row
            pos += row * FramebufferInfo->PixelsPerScanline;
            // Set cold
            pos += col;

            // Set color
            *pos = rgb(0,0,0);
        }
    }
    text_line = 0;
    text_col_px = 0;
}

void set_frambuffer_font_size(int FontSize) {
    switch (FontSize) {
        case 8:
            font_array = vgafont8;
            break;
        case 14:
            font_array = vgafont14;
            break;
        case 16:
            font_array = vgafont16;
            break;
        default:
            return;
    }
    font_size = FontSize;
    max_text_line = (FramebufferInfo->VerticalResolution / font_size) - 1;
}

void scroll_down_framebuf(void) {
    // start at 2nd line
    uint32_t* pos = FramebufferInfo->FramebufferPointer;
    pos += font_size * FramebufferInfo->PixelsPerScanline;
    // Move all pixels up by font_size lines
    for (uint16_t row = 0; row < font_size * max_text_line; row++) {
        memcpy(pos,
              (pos - (font_size * FramebufferInfo->PixelsPerScanline)),
                FramebufferInfo->PixelsPerScanline * 4);
        pos += FramebufferInfo->PixelsPerScanline;
    }
    // Blank last line
    pos = FramebufferInfo->FramebufferPointer;
    pos += max_text_line * font_size * FramebufferInfo->PixelsPerScanline;
    for (uint16_t row = 0; row < font_size; row++) {
        for (uint16_t col = 0; col < FramebufferInfo->PixelsPerScanline; col++) {
            *pos = rgb(0, 0, 0);
            pos++;
        }
    }
}

static int rprintf_dec(uint64_t i, bool sign) {
    // Create a buffer acting as a stack for digits
    char StringStack[UINT64_DEC_MAXDIGIT];
    int16_t  StackTop = -1;
    if (sign) {
        if (i > UINT32_MAX) {
            // Value passed is 64 bits wide
            i ^= 0xFFFFFFFFFFFFFFFF;
            i++;
        }
        else {
            // Value passed is 32 bits wide
            i ^= 0xFFFFFFFF;
            i++;
        }
    }
    
    // Add 0 to the stack when the number is 0 as the loop is skipped
    if (i == 0) {
        ++StackTop;
        StringStack[StackTop] = '0';
    }

    // Store the base 10 digits in the stack
    while (i != 0) {
        ++StackTop;
        StringStack[StackTop] = (char)((i%10) + 48);
        i /= 10;
    }
    
    if (sign) {
        // Add negative symbol
        StackTop++;
        StringStack[StackTop] = '-';
    }

    // Reverse the character elements of the stack to correctly match the number
    int16_t  TempIndex = StackTop;
    char TempChar;
    for (; TempIndex > (StackTop / 2); --TempIndex) {
        TempChar = StringStack[StackTop - TempIndex];
        StringStack[StackTop - TempIndex] = StringStack[TempIndex];
        StringStack[TempIndex] = TempChar;
    }

    // Add the null byte
    StringStack[StackTop + 1] = '\0';
    
    // Print the created string
    return rprintf(StringStack, NULL);
}

static int rprintf_hex(uint64_t h, bool upper, bool zero_x) {
    // Create a buffer acting as a stack for digits
    char StringStack[UINT64_DEC_MAXDIGIT];
    int16_t  StackTop = -1;
    
    // Add 0 to the stack when the number is 0 as the loop is skipped
    if (h == 0) {
        ++StackTop;
        StringStack[StackTop] = '0';
    }

    // Store the base 10 digits in the stack
    while (h != 0) {
        ++StackTop;
        if ((h%16) < 10) {
            StringStack[StackTop] = (char)((h%16) + 48);
        }
        else {
            // -10: The first alphabetic letter is located at 65 (A),
            //      10 marks the first so it needs to be subtracted
            //      before conversion
            if (upper) {
                StringStack[StackTop] = (char)((h%16) + (65 - 10));
            }
            else {
                StringStack[StackTop] = (char)((h%16) + (97 - 10));
            }
        }
        h /= 16;
    }

    // Add the 0x to the string when necessary
    // Added here in reverse order as the string is reversed
    if (zero_x) {
        StringStack[StackTop+1] = 'x';
        StringStack[StackTop+2] = '0';
        StackTop += 2;
    }
    
    // Reverse the character elements of the stack to correctly match the number
    int16_t TempIndex = StackTop;
    char    TempChar;
    for (; TempIndex > (StackTop / 2); --TempIndex) {
        TempChar = StringStack[StackTop - TempIndex];
        StringStack[StackTop - TempIndex] = StringStack[TempIndex];
        StringStack[TempIndex] = TempChar;
    }
    
    // Add the null byte
    StringStack[StackTop + 1] = '\0';
    
    // Print the created string
    return rprintf(StringStack, NULL);
}

static int rprintf_oct(uint64_t o) {
    // Create a buffer acting as a stack for digits
    char StringStack[UINT64_DEC_MAXDIGIT];
    int16_t  StackTop = -1;
    
    // Add 0 to the stack when the number is 0 as the loop is skipped
    if (o == 0) {
        ++StackTop;
        StringStack[StackTop] = '0';
    }

    // Store the base 10 digits in the stack
    while (o != 0) {
        ++StackTop;
        StringStack[StackTop] = (char)((o%8) + 48);
        o /= 8;
    }
    
    // Reverse the character elements of the stack to correctly match the number
    int16_t  TempIndex = StackTop;
    char TempChar;
    for (; TempIndex > (StackTop / 2); --TempIndex) {
        TempChar = StringStack[StackTop - TempIndex];
        StringStack[StackTop - TempIndex] = StringStack[TempIndex];
        StringStack[TempIndex] = TempChar;
    }

    // Add the null byte
    StringStack[StackTop + 1] = '\0';
    
    // Print the created string
    return rprintf(StringStack, NULL);
}

static int rprintf_float(double d, bool hex, bool upper) {

}

static int rprintf_sci(uint64_t i, bool upper) {

}

int rprintf(const char *restrict format, va_list ap) {
    int bytes_printed = 0;
    char c = 0;

    if (FramebufferInfo == NULL || FramebufferInfo->FramebufferPointer == NULL) {
        return -1;
    }

    while (c = *(format++), c != 0) {
        // %[argument$][flags][width][.precision][length modifier]conversion
        // Conversion specifier
        if (c == '%') {
            switch (*format) {
                // Signed decimal integer
                case 'd':
                    bytes_printed += rprintf_dec(va_arg(ap, uint64_t), true);
                    break;
                // Signed decimal integer
                case 'i':
                    bytes_printed += rprintf_dec(va_arg(ap, uint64_t), true);
                    break;
                // Unsigned decimal integer
                case 'u':
                    bytes_printed += rprintf_dec(va_arg(ap, uint64_t), false);
                    break;
                // Unsigned octal
                case 'o':
                    bytes_printed += rprintf_oct(va_arg(ap, uint64_t));
                    break;
                // Unsigned hexadecimal integer
                case 'x':
                    bytes_printed += rprintf_hex(va_arg(ap, uint64_t), false, true);
                    break;
                // Unsigned hexadecimal integer (uppercase)
                case 'X':
                    bytes_printed += rprintf_hex(va_arg(ap, uint64_t), true, true);
                    break;
                // Decimal floating point, lowercase
                case 'f':
                    bytes_printed += rprintf_float(va_arg(ap, double), false, false);
                    break;
                // Decimal floating point, uppercase
                case 'F':
                    bytes_printed += rprintf_float(va_arg(ap, double), false, true);
                    break;
                // Scientific notation (mantissa/exponent), lowercase
                case 'e':
                    bytes_printed += rprintf_sci(va_arg(ap, uint64_t), false);
                    break;
                // Scientific notation (mantissa/exponent), uppercase
                case 'E':
                    bytes_printed += rprintf_sci(va_arg(ap, uint64_t), false);
                    break;
                // Use the shortest representation: %e or %f
                case 'g':
                    break;
                // Use the shortest representation: %E or %F
                case 'G':
                    break;
                // Hexadecimal floating point, lowercase
                case 'a':
                    bytes_printed += rprintf_float(va_arg(ap, double), true, false);
                    break;
                // Hexadecimal floating point, uppercase
                case 'A':
                    bytes_printed += rprintf_float(va_arg(ap, double), true, true);
                    break;
                // Character
                case 'c':
                    rputchar(va_arg(ap, int));
                    bytes_printed++;
                    break;
                // String of characters
                case 's':
                    bytes_printed += rprintf(va_arg(ap, char*), NULL);
                    break;
                // Pointer address
                case 'p':
                    bytes_printed += rprintf_hex(va_arg(ap, uint64_t), true, true);
                    break;
                // Nothing printed
                // The corresponding argument must be a pointer to a signed int
                // The number of characters written so far is stored in the printed location
                case 'n':
                    *(va_arg(ap, int*)) = bytes_printed;
                    break;
                // Default ignore case for printing the character
                case '%':
                    rputchar('%');
                    bytes_printed++;
                    break;
            }
            // Skip the next character
            ++format;
        }
        else {
            if (rputchar(c) == -1) {
                return -1;
            }
        }
        bytes_printed++;
    }

    return bytes_printed;
}

int rputchar(const char c) {
    if (font_array == NULL || font_size == 0) {
        return -1;
    }
    if (c == 0) {
        return 0;
    }
    if (c == '\n') {
        text_col_px = 0;
        if (text_line == max_text_line) {
            scroll_down_framebuf();
        }
        else {
            text_line += 1;
        }
        return '\n';
    }
    uint32_t* cursor_pos = FramebufferInfo->FramebufferPointer;
    // Advance the cursor to the current line
    cursor_pos += text_line * font_size * FramebufferInfo->PixelsPerScanline;
    // Advance the cursor to the current column
    cursor_pos += (text_col_px + 8) - 1; // Last pixel of the line with a font width of 8
    // Position saved for the beginning of the line
    uint32_t* last_pos = cursor_pos;
    const unsigned char* ch_addr = &font_array[c * font_size];
    // Print the character to the framebuffer
    for (unsigned int i = 0; i < font_size; i++) { 
        unsigned char ch = *ch_addr;
        for (uint8_t j = 0; j < 8; j++) {
            if (ch % 2 == 1) { // last bit is set
                *cursor_pos = text_color;
            }
            else {
                *cursor_pos = background_color;
            }
            ch = ch >> 1;
            cursor_pos--;
        }
        ch_addr++;
        last_pos += FramebufferInfo->PixelsPerScanline;
        cursor_pos = last_pos;
    }
    text_col_px += 9; // 9 instead of 8 to add a space in between chars printed

    return (int)c;
}
