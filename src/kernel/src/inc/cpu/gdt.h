#ifndef GDT_H
#define GDT_H

#include <datatypes.h>

// The structure of the GDT itself
typedef struct __attribute__((packed)) {
    // Null descriptor
    uint64_t NullValue;
    // Code segment descriptor
    uint16_t code_limit;   // limit (bits 0-15)
    uint16_t code_base_1;  // base  (bits 0-15)
    uint8_t  code_base_2;  // base  (bits 16-23)
    uint8_t  code_flags_1; // 1st flags + type flags
    uint8_t  code_flags_2; // 2nd flags + limit (FF/1111, bits 16-19)
    uint8_t  code_base_3;  // base (bits 23-31)
    // Data segment descriptor
    uint16_t data_limit;   // limit (bits 0-15)
    uint16_t data_base_1;  // base  (bits 0-15)
    uint8_t  data_base_2;  // base  (bits 16-23)
    uint8_t  data_flags_1; // 1st flags + type flags
    uint8_t  data_flags_2; // 2nd flags + limit (FF/1111, bits 16-19)
    uint8_t  data_base_3;  // base (bits 23-31)
} GDT_t;

// GDT desctiptor
typedef struct __attribute__((packed)) {
    uint16_t size;  // Size of GDT, 1 less than the true size
    uint64_t start; // Starting address of the GDT
} GDT_descriptor_t;

// Global definition of the GDT and GDT descriptor
static GDT_t            gdt;
static GDT_descriptor_t gdt_desc;

// Assembly defined function to load the GDT
extern void gdt_init_asm(void* gdt_desc_addr, uint16_t gdt_code_seg_offs, uint16_t gdt_data_seg_offs, void* seg_init_addr);
// The linker complains when the assembly uses label addressing so the address to the label will be passed as a parameter
extern void segments_init(void);

// Abstracted C function needed to call the assembly defined function to load the GDT
void gdt_init(void);

#endif
