global gdt_init_asm
gdt_init_asm:
    ; RDI: Argument 1: GDT address
    ; RSI: Argument 2: GDT code segment offset (addressed as SI)
    ; RDX: Argument 3: GDT data segment offset (addressed as DX)
    ; RCX: Argument 4: Segment initialization function address
    lgdt [rdi]
    ; my otherworldly version of a long jump
    mov rax,rsp
    push rdx            ;iretq_ss
    push rax           ;iretq_rsp
    pushfq             ;iretq_flags
    push rsi            ;iretq_cs
    push rcx ;iretq_rip
    iretq

global segments_init
segments_init:
    ; RDX should be unchanged so the reference to DX should be the GDT data segment offset
    mov ax,dx
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    ret
