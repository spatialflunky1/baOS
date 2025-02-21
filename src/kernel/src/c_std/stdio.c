#include <stdio.h>

int printf(const char *restrict format, ...) {
    va_list ap;
    va_start(ap, format);
    
    int bytes_printed = vprintf(format, ap); 

    va_end(ap);

    return bytes_printed;
}

int vprintf(const char *restrict format, va_list ap) {
    int bytes_printed = -1;

    if (get_video_mode() == FRAMEBUFFER_CON) {
        bytes_printed = rprintf(format, ap);
    }

    return bytes_printed;
}
