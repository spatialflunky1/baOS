#include <kernel/messages.h>

int klog(FILTER_LEVEL level, bool prefix, const char* restrict format, ...) {
    if (level < MESSAGE_FILTER) { return 1; }
    int bytes_printed = 0;

    // Load the variable arguments
    va_list ap;
    va_start(ap, format);
    // Specify prefixes for message types
    if (prefix) {
        switch (level) {
            case FILTER_NONE:
                break;
            case DebugMsg:
                bytes_printed += vprintf("DEBUG: ", ap);
                break;
            case InfoMsg:
                bytes_printed += vprintf("INFO: ", ap);
                break;
            case WarningMsg:
                bytes_printed += vprintf("WARNING: ", ap);
                break;
            case ErrMsg:
                bytes_printed += vprintf("ERROR: ", ap);
                break;
            case CriticalMsg:
                bytes_printed += vprintf("Critical: ", ap);
                break;
        }
    }
    // Pass the string and arguments to vprintf
    bytes_printed += vprintf(format, ap);
    // Close the variable argument list
    va_end(ap);

    return bytes_printed;
}
