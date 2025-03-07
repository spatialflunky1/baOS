#ifndef CPU_H
#define CPU_H

#include <datatypes.h>
#include <kernel/messages.h>

typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t int_vector;
    uint64_t error;
    uint64_t error_code;

    uint64_t iretq_rip;
    uint64_t iretq_cs;
    uint64_t iretq_flags;
    uint64_t iretq_rsp;
    uint64_t iretq_ss;
} cpu_status_t;

void cpu_freeze(void);

#endif
