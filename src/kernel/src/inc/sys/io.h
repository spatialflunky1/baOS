#ifndef IO_H
#define IO_H

#include <datatypes.h>

// ---------------------------------
// Programmable Interrupt Controller
// ---------------------------------
#define PIC_COMMAND_MASTER 0x20
#define PIC_DATA_MASTER    0x21
#define PIC_COMMAND_SLAVE  0xA0
#define PIC_DATA_SLAVE     0xA1

// End of Interrupt command
#define PIC_EOI 0x20

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

#endif
