#include <lib/memory.h>
#include <lib/string.h>
#include <kernel/modules.h>
#include <kernel/runtime.h>
#include <kernel/vfs/proc.h>

static struct runtime_task *tasks;
static struct modules_filesystem filesystem;

static unsigned int read(struct modules_filesystem *self, unsigned int id, unsigned int offset, unsigned int count, void *buffer)
{

    char *out = buffer;
    unsigned int off = 0;
    unsigned int i;

    for (i = 0; i < RUNTIME_TASK_SLOTS; i++)
    {

        struct runtime_task *task = &tasks[i];

        if (task->used)
        {

            string_write(out + off, "%d/\n", i);
            off += string_length(out);

        }

    }

    return off;

}

static unsigned int find(struct modules_filesystem *self, char *name)
{

    unsigned int length = string_length(name);

    if (!length)
        return 1;

    return 0;

}

void vfs_proc_init(struct runtime_task *t)
{

    tasks = t;

    modules_filesystem_init(&filesystem, 0x0001, "proc", 0, 0, read, 0, find, 0);
    modules_register_filesystem(&filesystem);
    filesystem.path = "/proc/";

}

