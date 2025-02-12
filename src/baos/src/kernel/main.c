// The main kernel source file
// This is where baOS begins

#include <kernel/kernel.h>

int main(KERNEL_BOOT_INFO* BootInfo) {
    initialize_framebuffer(&BootInfo->VideoModeInfo);
    
    printf("baOS kernel successfully loaded\n"); 
  
    while(1);
    return 0;
}
