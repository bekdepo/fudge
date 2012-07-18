.intel_syntax noprefix

.set CALL_INTERRUPT,     0x80
.set CALL_INDEX_OPEN,    0x01
.set CALL_INDEX_CLOSE,   0x02
.set CALL_INDEX_READ,    0x03
.set CALL_INDEX_WRITE,   0x04
.set CALL_INDEX_EXECUTE, 0x05
.set CALL_INDEX_EXIT,    0x06
.set CALL_INDEX_WAIT,    0x07
.set CALL_INDEX_LOAD,    0x08
.set CALL_INDEX_UNLOAD,  0x09
.set CALL_INDEX_ATTACH,  0x0A
.set CALL_INDEX_DETACH,  0x0B
.set CALL_INDEX_MOUNT,   0x0C

.section .text

.align 4

.global call_open
call_open:
    mov eax, CALL_INDEX_OPEN
    int CALL_INTERRUPT
    ret

.align 4

.global call_close
call_close:
    mov eax, CALL_INDEX_CLOSE
    int CALL_INTERRUPT
    ret

.align 4

.global call_read
call_read:
    mov eax, CALL_INDEX_READ
    int CALL_INTERRUPT
    ret

.align 4

.global call_write
call_write:
    mov eax, CALL_INDEX_WRITE
    int CALL_INTERRUPT
    ret

.align 4

.global call_execute
call_execute:
    mov eax, CALL_INDEX_EXECUTE
    int CALL_INTERRUPT
    ret

.align 4

.global call_exit
call_exit:
    mov eax, CALL_INDEX_EXIT
    int CALL_INTERRUPT
    ret

.align 4

.global call_wait
call_wait:
    mov eax, CALL_INDEX_WAIT
    int CALL_INTERRUPT
    ret

.align 4

.global call_load
call_load:
    mov eax, CALL_INDEX_LOAD
    int CALL_INTERRUPT
    ret

.align 4

.global call_unload
call_unload:
    mov eax, CALL_INDEX_UNLOAD
    int CALL_INTERRUPT
    ret

.align 4

.global call_attach
call_attach:
    mov eax, CALL_INDEX_ATTACH
    int CALL_INTERRUPT
    ret

.align 4

.global call_detach
call_detach:
    mov eax, CALL_INDEX_DETACH
    int CALL_INTERRUPT
    ret

.align 4

.global call_mount
call_mount:
    mov eax, CALL_INDEX_MOUNT
    int CALL_INTERRUPT
    ret

