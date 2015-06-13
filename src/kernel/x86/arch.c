#include <fudge.h>
#include <kernel.h>
#include "cpu.h"
#include "arch.h"
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "mmu.h"

#define CONTAINERS                      8
#define TASKS                           64
#define GDTDESCRIPTORS                  6
#define IDTDESCRIPTORS                  256
#define TSSDESCRIPTORS                  1
#define KERNELBASE                      0x00000000
#define KERNELSIZE                      0x00400000
#define KERNELLIMIT                     (KERNELBASE + KERNELSIZE)
#define MMUALIGN                        0x1000
#define CONTAINERDIRECTORYCOUNT         1
#define CONTAINERDIRECTORYBASE          KERNELLIMIT
#define CONTAINERDIRECTORYLIMIT         (CONTAINERDIRECTORYBASE + CONTAINERS * (MMUALIGN * CONTAINERDIRECTORYCOUNT))
#define CONTAINERTABLECOUNT             8
#define CONTAINERTABLEBASE              CONTAINERDIRECTORYLIMIT
#define CONTAINERTABLELIMIT             (CONTAINERTABLEBASE + CONTAINERS * (MMUALIGN * CONTAINERTABLECOUNT))
#define TASKDIRECTORYCOUNT              1
#define TASKDIRECTORYBASE               CONTAINERTABLELIMIT
#define TASKDIRECTORYLIMIT              (TASKDIRECTORYBASE + TASKS * (MMUALIGN * TASKDIRECTORYCOUNT))
#define TASKTABLECOUNT                  2
#define TASKTABLEBASE                   TASKDIRECTORYLIMIT
#define TASKTABLELIMIT                  (TASKTABLEBASE + TASKS * (MMUALIGN * TASKTABLECOUNT))
#define TASKCODEBASE                    0x01000000
#define TASKCODESIZE                    0x00080000
#define TASKSTACKBASE                   (TASKCODEBASE + TASKS * TASKCODESIZE)
#define TASKSTACKSIZE                   0x00010000
#define TASKVSTACKLIMIT                 0x80000000

static struct
{

    struct gdt_pointer pointer;
    struct gdt_descriptor descriptors[GDTDESCRIPTORS];

} gdt;

static struct
{

    struct idt_pointer pointer;
    struct idt_descriptor descriptors[IDTDESCRIPTORS];

} idt;

static struct
{

    struct tss_pointer pointer;
    struct tss_descriptor descriptors[TSSDESCRIPTORS];

} tss;

static struct
{

    unsigned short kcode;
    unsigned short kdata;
    unsigned short ucode;
    unsigned short udata;
    unsigned short tlink;

} selector;

static struct arch_container
{

    struct container base;
    struct mmu_directory *directory;
    struct mmu_table *table;

} containers[CONTAINERS];

static struct arch_task
{

    struct task base;
    struct mmu_directory *directory;
    struct mmu_table *table;
    unsigned int mapping[TASKTABLECOUNT];

} tasks[TASKS];

static struct
{

    struct container *container;
    struct task *task;

} current;

static void containermaptext(struct container *container)
{

    struct arch_container *acontainer = (struct arch_container *)container;

    mmu_map(acontainer->directory, &acontainer->table[0], KERNELBASE, KERNELBASE, KERNELSIZE, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);
    mmu_map(acontainer->directory, &acontainer->table[1], 0x00400000, 0x00400000, 0x00400000, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);
    mmu_map(acontainer->directory, &acontainer->table[2], 0x00800000, 0x00800000, 0x00400000, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);

}

static void taskmapcontainer(struct task *task, struct container *container)
{

    struct arch_task *atask = (struct arch_task *)task;
    struct arch_container *acontainer = (struct arch_container *)container;

    memory_copy(atask->directory, acontainer->directory, sizeof (struct mmu_directory));

}

static void taskmaptext(struct task *task, unsigned int address)
{

    struct arch_task *atask = (struct arch_task *)task;

    mmu_map(atask->directory, &atask->table[0], atask->mapping[0], address, TASKCODESIZE, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE | MMU_TFLAG_USERMODE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE | MMU_PFLAG_USERMODE);
    mmu_map(atask->directory, &atask->table[1], atask->mapping[1], TASKVSTACKLIMIT - TASKSTACKSIZE, TASKSTACKSIZE, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE | MMU_TFLAG_USERMODE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE | MMU_PFLAG_USERMODE);

}

static void taskactivate(struct task *task)
{

    struct arch_task *atask = (struct arch_task *)task;

    mmu_setdirectory(atask->directory);

}

static unsigned int spawn(struct container *container, struct task *task, void *stack)
{

    struct task *next = scheduler_findinactive();

    if (!next)
        return 0;

    kernel_copytask(task, next);
    kernel_setuptask(next, TASKVSTACKLIMIT);
    taskmapcontainer(next, container);
    scheduler_use(next);

    return 1;

}

static unsigned int despawn(struct container *container, struct task *task, void *stack)
{

    scheduler_unuse(task);

    return 1;

}

void arch_setinterrupt(unsigned char index, void (*callback)())
{

    idt_setdescriptor(&idt.pointer, index, callback, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT);

}

void arch_setmap(unsigned char index, unsigned int paddress, unsigned int vaddress, unsigned int size)
{

    struct arch_container *acontainer = (struct arch_container *)current.container;

    mmu_map(acontainer->directory, &acontainer->table[index], paddress, vaddress, size, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);
    mmu_setdirectory(acontainer->directory);

}

unsigned short arch_schedule(struct cpu_interrupt *interrupt)
{

    if (current.task)
    {

        current.task->state.registers.ip = interrupt->eip;
        current.task->state.registers.sp = interrupt->esp;

    }

    current.task = scheduler_findactive();

    if (current.task)
    {

        interrupt->cs = selector.ucode;
        interrupt->ss = selector.udata;
        interrupt->eip = current.task->state.registers.ip;
        interrupt->esp = current.task->state.registers.sp;

        taskactivate(current.task);

    }

    else
    {

        interrupt->cs = selector.kcode;
        interrupt->ss = selector.kdata;
        interrupt->eip = (unsigned int)cpu_halt;
        interrupt->esp = KERNELLIMIT;

    }

    return interrupt->ss;

}

unsigned short arch_generalfault(void *stack)
{

    struct {struct cpu_general general; unsigned int selector; struct cpu_interrupt interrupt;} *registers = stack;

    return arch_schedule(&registers->interrupt);

}

unsigned short arch_pagefault(void *stack)
{

    struct {struct cpu_general general; unsigned int type; struct cpu_interrupt interrupt;} *registers = stack;
    /*
    unsigned int address = cpu_getcr2();
    */

    if (current.task)
    {

        taskmaptext(current.task, 0x08048000);
        kernel_copyprogram(current.task);

    }

    return arch_schedule(&registers->interrupt);

}

unsigned short arch_syscall(void *stack)
{

    struct {struct cpu_general general; struct cpu_interrupt interrupt;} *registers = stack;

    registers->general.eax = kernel_call(registers->general.eax, current.container, current.task, (void *)(registers->interrupt.esp + 16));

    return arch_schedule(&registers->interrupt);

}

static void setupcontainer(struct arch_container *container, unsigned int index)
{

    container_init(&container->base);

    container->directory = (struct mmu_directory *)CONTAINERDIRECTORYBASE + index * CONTAINERDIRECTORYCOUNT;
    container->table = (struct mmu_table *)CONTAINERTABLEBASE + index * CONTAINERTABLECOUNT;

}

static struct container *setupcontainers()
{

    unsigned int index;

    for (index = 0; index < CONTAINERS; index++)
        setupcontainer(&containers[index], index);

    return &containers[0].base;

}

static void setuptask(struct arch_task *task, unsigned int index)
{

    task_init(&task->base);

    task->directory = (struct mmu_directory *)TASKDIRECTORYBASE + index * TASKDIRECTORYCOUNT;
    task->table = (struct mmu_table *)TASKTABLEBASE + index * TASKTABLECOUNT;
    task->mapping[0] = TASKCODEBASE + index * TASKCODESIZE;
    task->mapping[1] = TASKSTACKBASE + index * TASKSTACKSIZE;

    scheduler_registertask(&task->base);

}

static struct task *setuptasks()
{

    unsigned int index;

    for (index = 0; index < TASKS; index++)
        setuptask(&tasks[index], index);

    return &tasks[0].base;

}

static void arch_complete()
{

    struct cpu_interrupt interrupt;

    interrupt.eflags = cpu_geteflags() | 0x200;

    arch_schedule(&interrupt);
    cpu_leave(interrupt);

}

void arch_setup(struct vfs_backend *backend)
{

    struct container *container;
    struct task *task;

    gdt_initpointer(&gdt.pointer, GDTDESCRIPTORS, gdt.descriptors);
    idt_initpointer(&idt.pointer, IDTDESCRIPTORS, idt.descriptors);
    tss_initpointer(&tss.pointer, TSSDESCRIPTORS, tss.descriptors);

    selector.kcode = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_KCODE, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW | GDT_ACCESS_EXECUTE, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.kdata = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_KDATA, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.ucode = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_UCODE, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW | GDT_ACCESS_EXECUTE, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.udata = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_UDATA, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.tlink = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_TLINK, (unsigned long)tss.pointer.descriptors, (unsigned long)tss.pointer.descriptors + tss.pointer.limit, GDT_ACCESS_PRESENT | GDT_ACCESS_EXECUTE | GDT_ACCESS_ACCESSED, GDT_FLAG_32BIT);

    idt_setdescriptor(&idt.pointer, IDT_INDEX_GF, arch_isrgeneralfault, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT);
    idt_setdescriptor(&idt.pointer, IDT_INDEX_PF, arch_isrpagefault, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT);
    idt_setdescriptor(&idt.pointer, IDT_INDEX_SYSCALL, arch_isrsyscall, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT | IDT_FLAG_RING3);
    tss_setdescriptor(&tss.pointer, TSS_INDEX_DEFAULT, selector.kdata, KERNELLIMIT);
    cpu_setgdt(&gdt.pointer, selector.kcode, selector.kdata);
    cpu_setidt(&idt.pointer);
    cpu_settss(selector.tlink);
    kernel_setup(spawn, despawn);

    container = setupcontainers();
    task = setuptasks();

    kernel_setupramdisk(container, task, backend);
    kernel_copytask(task, task);
    kernel_setuptask(task, TASKVSTACKLIMIT);
    scheduler_use(task);
    containermaptext(container);
    taskmapcontainer(task, container);
    taskactivate(task);
    mmu_setup();

    current.container = container;

    arch_complete();

}
