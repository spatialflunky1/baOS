#include <sys/io.h>

void outb(uint16_t port, uint8_t value) {
    __asm__("out %%al,%%dx"
            : // No outputs
            : "a" (value), "d" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ ("in %%dx,%%al"
            : "=a"(value)
            : "d"(port));
    return value;
}
