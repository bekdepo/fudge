void arch_halt();
void arch_usermode(unsigned int code, unsigned int data, unsigned int ip, unsigned int sp);
void arch_isr_genfault();
void arch_isr_pagefault();
void arch_isr_syscall();
void arch_setup(unsigned int modulesc, void **modulesv);