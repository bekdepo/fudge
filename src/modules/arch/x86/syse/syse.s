.code32

.extern syse_syscall

.global syse_interrupt
syse_interrupt:
    pusha
    call syse_syscall
    popa
    sysexit
