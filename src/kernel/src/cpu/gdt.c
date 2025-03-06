#include <cpu/gdt.h>

void gdt_init(void) {
    // ---------------------------------
    // Populate the globally defined GDT
    // ---------------------------------

    // Null descriptor
    // ---------------
    // An exception is raised when this value is addressed
    gdt.NullValue    = 0x0;

    // Code segment descriptor
    // -----------------------
    // base  = 0x0
    // limit = 0xffff
    // 1st flags : 1001b
    //     (1: present) (00: privilege) (1: descriptor type)
    // type flags: 1010b
    //     (1: code) (0: conforming) (1: readable) (0: accessed)
    // 2nd flags : 1010b
    //     (1: granularity) (0: 32-bit default) (1: 64-bit segment) (0: AVL/availiable for system use)
    gdt.code_limit   = 0xFFFF;
    gdt.code_base_1  = 0x0;
    gdt.code_base_2  = 0x0;
    gdt.code_flags_1 = 0b10011010;
    gdt.code_flags_2 = 0b10101111;
    gdt.code_base_3  = 0x0;

    // Data segment descriptor
    // -----------------------
    // base  = 0x0
    // limit = 0xffff
    // 1st flags : 1001b
    //     (1: present) (00: privilege) (1: descriptor type)
    // type flags: 0010b
    //     (0: code) (0: expand down) (1: writable) (0: accessed)
    // 2nd flags : 1100b
    //     (1: granularity) (1: 32-bit default) (0: 64-bit segment) (0: AVL/availiable for system use)
    gdt.data_limit   = 0xFFFF;
    gdt.data_base_1  = 0x0;
    gdt.data_base_2  = 0x0;
    gdt.data_flags_1 = 0b10010010;
    gdt.data_flags_2 = 0b11001111;
    gdt.data_base_3  = 0x0;

    // --------------------------------------------
    // Populate the globally defined GDT descriptor
    // --------------------------------------------
    
    gdt_desc.size  = sizeof(GDT_t) - 1;
    gdt_desc.start = (uint64_t)&gdt;

    // ----------------
    // Load the new GDT
    // ----------------
    gdt_init_asm(&gdt_desc, 
                 (uint16_t)(((uint64_t)&gdt.code_limit) - ((uint64_t)&gdt)),
                 (uint16_t)(((uint64_t)&gdt.data_limit) - ((uint64_t)&gdt)),
                 segments_init);
}
