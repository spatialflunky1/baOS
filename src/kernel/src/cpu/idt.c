#include <cpu/idt.h>

// Normal non-interrupt function forward declarations
void idt_set_descriptor(uint8_t vect, void* isr, uint8_t flags);
void init_isr_stub_table(void);

void idt_init(void* KernelBase) {
    // ------------------------
    // Initialize IDT structure
    // ------------------------
    idtr.base = &idt[0];
    idtr.limit = sizeof(idt_entry_t) * MAX_IDT_ENTRIES - 1;

    // Prepare the ISR stub table structure
    //init_isr_stub_table();
    
    // Set reserved interrupts (0-31) and IRQs (32-47)
    for (uint8_t vect = 0; vect < 48; ++vect) { 
        idt_set_descriptor(vect, (void*)((uint64_t)isr_stub_table[vect] + (uint64_t)KernelBase), 0x8E);
    }

    // -----------------------------
    // Prepare and enable interrupts
    // -----------------------------
    
    // Mask PIC interrupts
    outb(PIC_DATA_MASTER, 0xFF);
    outb(PIC_DATA_SLAVE, 0xFF);
    
    // Load the IDT structure (by the IDT descriptor)
    __asm__ volatile ("lidt %0" :: "m"(idtr));
    
    // Enable interrupts (set sti flag)
    __asm__ volatile ("sti");
}

void idt_set_descriptor(uint8_t vect, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vect];
    uint64_t isr_addr = (uint64_t)isr;
    descriptor->isr_low    = isr_addr & 0xFFFF;
    descriptor->kernel_cs  = 0x08;
    descriptor->ist        = 0;
    descriptor->attributes = flags; 
    descriptor->isr_mid    = (isr_addr >> 16) & 0xFFFF;
    descriptor->isr_high   = (isr_addr >> 32) & 0xFFFFFFFF;
    descriptor->reserved   = 0;
}

cpu_status_t* interrupt_handler(cpu_status_t* cpu_status) {
    if (cpu_status->error) {
        if (cpu_status->int_vector == 0x0E) {
            // Page fault
            return cpu_status;
        }
        klog(CriticalMsg, false, "\nFlosp OS has encountered an exception and cannot continue\n");
        switch (cpu_status->int_vector) {
            case 0x0D:
                klog(CriticalMsg, true, "General Protection Fault\n");
                break;
            default:
                klog(ErrMsg, false, "Interrupt vector: %lX\n", cpu_status->int_vector);
                break;
        }
        klog(ErrMsg, false, "Error Code: %lX\n", cpu_status->error_code);
        cpu_freeze();
    }
    // Reach here on non-error interrupts
    switch (cpu_status->int_vector) {
        case 0x20:
            // System Timer Interrupt
            break;
        case 0x21:
            // Keyboard interrupt
            break;
        case 0x2C:
            // Mouse interrupt
            break;
        default:
            klog(WarningMsg, true, "Unexpected Interrupt: %lX\n", cpu_status->int_vector);
            break;
    }
    // Send EOI to PIC
    // Also send EOI to slave pic if vector is at or above IRQ 8
    if (cpu_status->int_vector >= 8+32) {
        outb(PIC_COMMAND_SLAVE, PIC_EOI);
    }
    outb(PIC_COMMAND_MASTER, PIC_EOI);
    return cpu_status;
}
