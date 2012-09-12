#include <memory.h>
#include <error.h>
#include <kernel.h>
#include <vfs.h>
#include <binary.h>
#include <ramdisk.h>
#include <runtime.h>
#include <syscall.h>

void kernel_register_interface(struct kernel_interface *interface)
{

    struct vfs_interface *ramdisk;
    struct runtime_task *task;
    struct runtime_mount *mount;
    unsigned int id;

    interface->setup(interface);

    syscall_setup();
    runtime_setup();
    ramdisk = ramdisk_setup(interface->ramdiskc, interface->ramdiskv);

    id = ramdisk->walk(ramdisk, ramdisk->rootid, 8, "bin/init");

    task = runtime_get_task(1);
    runtime_task_init(task, 1, binary_get_entry(ramdisk, id));

    binary_copy_program(ramdisk, id);

    task->status.used = 1;

    mount = runtime_get_task_mount(task, 1);
    runtime_mount_init(mount, ramdisk, 9, "/ramdisk/");

    interface->enter_usermode(task->registers.ip, task->registers.sp);

}

void kernel_interface_init(struct kernel_interface *interface, void (*setup)(struct kernel_interface *self), void (*enter_usermode)(unsigned int ip, unsigned int sp), unsigned int ramdiskc, void **ramdiskv)
{

    memory_clear(interface, sizeof (struct kernel_interface));

    interface->setup = setup;
    interface->enter_usermode = enter_usermode;
    interface->ramdiskc = ramdiskc;
    interface->ramdiskv = ramdiskv;

}

