#include <lib/types.h>
#include <lib/io.h>
#include <kernel/regs.h>
#include <kernel/idt.h>
#include <kernel/irq.h>

void *irqRoutines[IRQ_ROUTINES_SIZE];

void irq_register_handler(unsigned char index, void (*handler)(registers_t *r))
{

    irqRoutines[index] = handler;

}

void irq_unregister_handler(unsigned char index)
{

    irqRoutines[index] = 0;

}

void irq_remap()
{

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);

}

void irq_handler(registers_t *r)
{

    void (*handler)(registers_t *r) = irqRoutines[r->int_no - 32];

    if (handler)
        handler(r);

    if (r->int_no >= 40)
        outb(0xA0, 0x20);

    outb(0x20, 0x20);

}

void irq_init()
{

    irq_remap();

    idt_set_gate(0x20, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(0x21, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(0x22, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(0x23, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(0x24, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(0x25, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(0x26, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(0x27, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(0x28, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(0x29, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(0x2A, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(0x2B, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(0x2C, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(0x2D, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(0x2E, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(0x2F, (uint32_t)irq15, 0x08, 0x8E);

}

