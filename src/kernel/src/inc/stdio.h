#ifndef STDIO_H
#define STDIO_H

#include <drivers/video/video.h>
#include <stdarg.h>

int printf(const char *restrict format, ...);
int vprintf(const char *restrict format, va_list ap);

#endif
