#include <error.h>
#include <event.h>
#include <mmu.h>
#include <runtime.h>
#include <arch/x86/idt.h>
#include <arch/x86/isr.h>

static void (*routines[ISR_TABLE_SLOTS])(struct runtime_task *task, struct isr_registers *registers);

static void load_state(struct runtime_task *task, struct isr_registers *registers)
{

    registers->interrupt.eip = task->registers.ip;
    registers->interrupt.esp = task->registers.sp;
    registers->general.ebp = task->registers.sb;

}

static void save_state(struct runtime_task *task, struct isr_registers *registers)
{

    task->registers.ip = registers->interrupt.eip;
    task->registers.sp = registers->interrupt.esp;
    task->registers.sb = registers->general.ebp;

}

static void isr_handle_undefined(struct runtime_task *task, struct isr_registers *registers)
{

    error_register(0, registers->index);
    error_register(1, registers->extra);
    error_panic("UNHANDLED INTERRUPT", __FILE__, __LINE__);

}

static void isr_raise(unsigned int index, struct runtime_task *task, struct isr_registers *registers)
{

    routines[index](task, registers);

}

void isr_register_routine(unsigned int index, void (*routine)(struct runtime_task *task, struct isr_registers *registers))
{

    routines[index] = routine;

}

void isr_unregister_routine(unsigned int index)
{

    routines[index] = 0;

}

void isr_handle(struct isr_registers *registers)
{

    struct runtime_task *task;

    task = runtime_schedule();

    save_state(task, registers);

    isr_raise(registers->index, task, registers);
    event_raise(registers->index, task);

    task = runtime_schedule();

    load_state(task, registers);

    mmu_load_user_memory(task->id);

}

void isr_setup()
{

    unsigned int i;

    for (i = 0; i < ISR_TABLE_SLOTS; i++)
        isr_register_routine(i, isr_handle_undefined);

    idt_set_gate(ISR_INDEX_DE, isr_routine00, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_DB, isr_routine01, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_NI, isr_routine02, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_BP, isr_routine03, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_OF, isr_routine04, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_BR, isr_routine05, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_UD, isr_routine06, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_NM, isr_routine07, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_DF, isr_routine08, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_CO, isr_routine09, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_TS, isr_routine0A, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_NP, isr_routine0B, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_SS, isr_routine0C, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_GP, isr_routine0D, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_PF, isr_routine0E, 0x08, 0x8E);
    idt_set_gate(0x0F, isr_routine0F, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_MF, isr_routine10, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_AC, isr_routine11, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_MC, isr_routine12, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_XM, isr_routine13, 0x08, 0x8E);
    idt_set_gate(0x14, isr_routine14, 0x08, 0x8E);
    idt_set_gate(0x15, isr_routine15, 0x08, 0x8E);
    idt_set_gate(0x16, isr_routine16, 0x08, 0x8E);
    idt_set_gate(0x17, isr_routine17, 0x08, 0x8E);
    idt_set_gate(0x18, isr_routine18, 0x08, 0x8E);
    idt_set_gate(0x19, isr_routine19, 0x08, 0x8E);
    idt_set_gate(0x1A, isr_routine1A, 0x08, 0x8E);
    idt_set_gate(0x1B, isr_routine1B, 0x08, 0x8E);
    idt_set_gate(0x1C, isr_routine1C, 0x08, 0x8E);
    idt_set_gate(0x1D, isr_routine1D, 0x08, 0x8E);
    idt_set_gate(0x1E, isr_routine1E, 0x08, 0x8E);
    idt_set_gate(0x1F, isr_routine1F, 0x08, 0x8E);
    idt_set_gate(0x20, isr_routine20, 0x08, 0x8E);
    idt_set_gate(0x21, isr_routine21, 0x08, 0x8E);
    idt_set_gate(0x22, isr_routine22, 0x08, 0x8E);
    idt_set_gate(0x23, isr_routine23, 0x08, 0x8E);
    idt_set_gate(0x24, isr_routine24, 0x08, 0x8E);
    idt_set_gate(0x25, isr_routine25, 0x08, 0x8E);
    idt_set_gate(0x26, isr_routine26, 0x08, 0x8E);
    idt_set_gate(0x27, isr_routine27, 0x08, 0x8E);
    idt_set_gate(0x28, isr_routine28, 0x08, 0x8E);
    idt_set_gate(0x29, isr_routine29, 0x08, 0x8E);
    idt_set_gate(0x2A, isr_routine2A, 0x08, 0x8E);
    idt_set_gate(0x2B, isr_routine2B, 0x08, 0x8E);
    idt_set_gate(0x2C, isr_routine2C, 0x08, 0x8E);
    idt_set_gate(0x2D, isr_routine2D, 0x08, 0x8E);
    idt_set_gate(0x2E, isr_routine2E, 0x08, 0x8E);
    idt_set_gate(0x2F, isr_routine2F, 0x08, 0x8E);
    idt_set_gate(ISR_INDEX_SYSCALL, isr_routine80, 0x08, 0xEE);

}

