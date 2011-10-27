#include <lib/elf.h>
#include <lib/file.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <kernel/vfs.h>
#include <kernel/modules.h>
#include <kernel/runtime.h>
#include <kernel/arch/x86/arch.h>
#include <kernel/arch/x86/isr.h>
#include <kernel/arch/x86/mmu.h>
#include <kernel/arch/x86/syscall.h>
#include <modules/elf/elf.h>

static void *syscallRoutines[SYSCALL_ROUTINES_SIZE];

void syscall_register_handler(unsigned char index, void (*handler)(struct syscall_registers *registers))
{

    syscallRoutines[index] = handler;

}

void syscall_unregister_handler(unsigned char index)
{

    syscallRoutines[index] = 0;

}

void syscall_handler(struct syscall_registers *registers)
{

    void (*handler)(struct syscall_registers *registers) = syscallRoutines[registers->eax];

    if (handler)
        handler(registers);

}

static void syscall_open(struct syscall_registers *registers)
{

    char *path = (char *)registers->esi;

    struct runtime_task *task = runtime_get_running_task();    

    struct vfs_node *node = vfs_find(path);

    if (!node)
    {

        registers->eax = -1;

        return;

    }

    struct vfs_descriptor *descriptor = task->add_descriptor(task, node);

    if (!descriptor)
    {

        registers->eax = -1;

        return;

    }

    registers->eax = descriptor->index;

}

static void syscall_close(struct syscall_registers *registers)
{

    unsigned int fd = registers->ebx;

    struct runtime_task *task = runtime_get_running_task();    

    task->remove_descriptor(task, fd);

}

static void syscall_read(struct syscall_registers *registers)
{

    unsigned int fd = registers->ebx;
    char *buffer = (char *)registers->esi;
    unsigned int count = registers->ecx;

    struct runtime_task *task = runtime_get_running_task();    

    struct vfs_node *node = task->get_descriptor(task, fd)->node;

    if (!(node && node->operations.read))
    {

        registers->eax = 0;

        return;

    }

    registers->eax = node->operations.read(node, count, buffer);

}

static void syscall_write(struct syscall_registers *registers)
{

    unsigned int fd = registers->ebx;
    char *buffer = (char *)registers->esi;
    unsigned int count = registers->ecx;

    struct runtime_task *task = runtime_get_running_task();    

    struct vfs_node *node = task->get_descriptor(task, fd)->node;

    if (!(node && node->operations.write))
    {

        registers->eax = 0;

        return;

    }

    registers->eax = node->operations.write(node, count, buffer);

}

static void syscall_info(struct syscall_registers *registers)
{

    unsigned int fd = registers->ebx;
    struct file_info *info = (struct file_info *)registers->edi;
    
    struct runtime_task *task = runtime_get_running_task();

    struct vfs_node *node = task->get_descriptor(task, fd)->node;

    if (!node)
    {

        registers->eax = 0;

        return;

    }

    info->id = node->id;
    info->length = 0;

    registers->eax = 1;

}

static void syscall_execute(struct syscall_registers *registers)
{

    char *path = (char *)registers->esi;
    unsigned int argc = registers->ecx;
    char **argv = (char **)registers->ebx;

    struct runtime_task *oldtask = runtime_get_running_task();
    oldtask->eip = (void *)registers->eip;
    oldtask->esp = (void *)registers->useresp;
    oldtask->ebp = (void *)registers->ebp;

    struct runtime_task *task = runtime_get_free_task();
    task->parentpid = oldtask->pid;

    if (!task->load(task, path, argc, argv))
    {

        registers->eax = 0;

        return;

    }

    runtime_activate(task);

    registers->eip = (unsigned int)task->eip;
    registers->useresp = (unsigned int)task->esp;
    registers->ebp = (unsigned int)task->ebp;
    registers->eax = 1;

}

static void syscall_exit(struct syscall_registers *registers)
{

    struct runtime_task *oldtask = runtime_get_running_task();
    oldtask->unload(oldtask);

    struct runtime_task *task = runtime_get_task(oldtask->parentpid);

    runtime_activate(task);

    registers->eip = (unsigned int)task->eip;
    registers->useresp = (unsigned int)task->esp;
    registers->ebp = (unsigned int)task->ebp;
    registers->eax = 1;

}

static void syscall_reboot(struct syscall_registers *registers)
{

    arch_reboot();

}

static void syscall_load(struct syscall_registers *registers)
{

    char *path = (char *)registers->esi;

    struct vfs_node *node = vfs_find(path);

    struct elf_header *header = elf_get_header(node->physical);
    elf_print_sections(node->physical, header);

    struct elf_section_header *symHeader = elf_get_section_header_by_type(node->physical, header, ELF_SECTION_TYPE_SYMTAB);
    elf_print_symtab(node->physical, symHeader);

    elf_relocate(node->physical, header);

    void (*minit)() = node->physical + 0x40;
    minit();

    registers->eax = 1;

}

static void syscall_unload(struct syscall_registers *registers)
{

    registers->eax = 1;

}

void syscall_init()
{

    syscall_register_handler(SYSCALL_ROUTINE_OPEN, syscall_open);
    syscall_register_handler(SYSCALL_ROUTINE_CLOSE, syscall_close);
    syscall_register_handler(SYSCALL_ROUTINE_READ, syscall_read);
    syscall_register_handler(SYSCALL_ROUTINE_WRITE, syscall_write);
    syscall_register_handler(SYSCALL_ROUTINE_INFO, syscall_info);
    syscall_register_handler(SYSCALL_ROUTINE_EXECUTE, syscall_execute);
    syscall_register_handler(SYSCALL_ROUTINE_EXIT, syscall_exit);
    syscall_register_handler(SYSCALL_ROUTINE_LOAD, syscall_load);
    syscall_register_handler(SYSCALL_ROUTINE_UNLOAD, syscall_unload);
    syscall_register_handler(SYSCALL_ROUTINE_REBOOT, syscall_reboot);

}

