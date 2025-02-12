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

int rprintf(const char *restrict format, va_list ap) {
    int bytes_printed = -1;
    int current_arg = 0;
    char c = 0;

    if (FramebufferInfo == NULL || FramebufferInfo->FramebufferPointer == NULL) {
        return -1;
    }

    while (c = *(format++), c != 0) {
        if (rputchar(c) == -1) {
            return -1;
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
