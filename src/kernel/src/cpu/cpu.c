#include <cpu/cpu.h>

void cpu_freeze(void) {
    klog(CriticalMsg, false, "CPU Frozen");
    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}
