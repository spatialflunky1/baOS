// The main kernel source file
// This is where baOS begins

#include <kernel/kernel.h>

int main(KERNEL_BOOT_INFO* BootInfo) {
    initialize_framebuffer(&BootInfo->VideoModeInfo);
    
    printf("baOS kernel successfully loaded\n"); 
   
    while (true) {
        printf("%d\n", 123);
        printf("%d\n", 123456);
    }
    while(1);
    return 0;
}
