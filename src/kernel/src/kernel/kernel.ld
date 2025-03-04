/* Size of the kernel stack */
KERNEL_STACK_SIZE = 0x4000;
/* Kernel entry point function */
ENTRY (main)
/* Program header definitions */
PHDRS
{
    phdr_text   PT_LOAD FLAGS(5);           /* read + execute */
    phdr_rodata PT_LOAD FLAGS(4);           /* read */
    phdr_data   PT_LOAD FLAGS(6);           /* read + write */
}
/* Memory segments */
SECTIONS
{
    .text :
    {
        _code = .;
        *(.text)
        . = ALIGN(4K);
    } :phdr_text 

    .rodata :
    {
        *(.rodata*)
        . = ALIGN(4K);
    } :phdr_rodata

    .data :
    {
        _data = .;
        *(.data)
        . = ALIGN(4K);
    } :phdr_data

    .bss :
    {
        _bss = .;
        *(COMMON)
        *(.bss)

        /* Stack "section" */
        . = ALIGN (16);
        stack_bottom = .;
        . += KERNEL_STACK_SIZE;
        stack_top = .;

        . = ALIGN (4K);
    }
}
