#ifndef IDT_H
#define IDT_H

#include <datatypes.h>
#include <cpu/cpu.h>
#include <kernel/messages.h>
#include <sys/io.h>

#define MAX_IDT_ENTRIES 256

// --------------
// IDT structures
// --------------
typedef struct __attribute__((packed)) {
    uint16_t isr_low;    // ISR address low offset
    uint16_t kernel_cs;  // Segment selector that is loaded into CS
    uint8_t  ist;        // Interrupt Stack Table offset, will be set 0 to disable the IST mechanism
    uint8_t  attributes; // bits 0-3: Gate type, (0xE: interrupt gate
                         //                       0xF: trap gate)
                         //   bit    4: Reserved (set 0)
                         //   bits 5-6: DPL, defines accessible CPU privilege levels
                         //   bit    7: Present (must be 1)
    uint16_t isr_mid;    // ISR address mid offset
    uint32_t isr_high;   // ISR address high offset
    uint32_t reserved;   // Set to 0
} idt_entry_t;

// IDT Descriptor
typedef struct __attribute__((packed)) {
    uint16_t     limit;
    idt_entry_t* base;
} idt_desc_t;

extern void* isr_stub_table[];

// Global definition of the IDT and IDT descriptor
__attribute__((aligned(0x10))) static idt_entry_t idt[MAX_IDT_ENTRIES]; // idt: array of idt entries
static idt_desc_t idtr;

void idt_init(void* KernelBase);
// The interrupt handler to be used by all interrupts
cpu_status_t* interrupt_handler(cpu_status_t* cpu_status);

#endif
