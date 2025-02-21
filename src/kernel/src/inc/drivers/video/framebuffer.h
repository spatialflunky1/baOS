#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <datatypes.h>
#include <stdarg.h>
#include <string.h>
#include <drivers/video/framebuffer_font.h>

// Convert 0-255 RGB values into their bitfield representation
#define rgb(r,g,b) (r << 16) | (g << 8) | b

// Framebuffer info as obtained by the bootloader
typedef struct {
    void*    FramebufferPointer;
    uint32_t HorizontalResolution;
    uint32_t VerticalResolution;
    uint32_t PixelsPerScanline;
} BOOT_VIDEO_MODE_INFO;

void initialize_framebuffer(BOOT_VIDEO_MODE_INFO* VMI);
void blank_framebuffer(void);
void set_frambuffer_font_size(int FontSize);
void scroll_down_framebuf(void);

int rprintf(const char *restrict format, va_list ap);

int rputchar(const char c);

#endif
