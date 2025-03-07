// The main kernel source file
// This is where baOS begins

#include <kernel/kernel.h>

int main(KERNEL_BOOT_INFO* BootInfo) {
    // --------------------------
    // Basic video initialization
    // --------------------------
    initialize_framebuffer(&BootInfo->VideoModeInfo);
    printf("baOS kernel successfully loaded\n");
    
    // --------------------------------------------
    // Switch from the firmware GDT to the kernel's
    // -------------------------------------------- 
    klog(InfoMsg, true, "Initializing GDT... ");
    gdt_init(); 
    klog(InfoMsg, false, "Done!\n");
    
    // ----------------------------------------
    // Initialize the IDT and enable interrupts
    // ----------------------------------------
    klog(InfoMsg, true, "Initializing IDT and enabling interrupts... ");
    idt_init(BootInfo->KernelBase);
    klog(InfoMsg, false, "Done!");
  
    // Halt execution
    while (true) { 
        __asm__ volatile ("hlt");
    }
    return 0;
}
