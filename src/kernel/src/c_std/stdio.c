#include <stdio.h>

int printf(const char* restrict format, ...) {
    // Load the variable arguments
    va_list ap;
    va_start(ap, format);
    // Pass the string and arguments to vprintf
    int bytes_printed = vprintf(format, ap); 
    // Close the variable argument list
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
