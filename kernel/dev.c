#include <lib/call.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <lib/vfs.h>
#include <kernel/dev.h>

struct vfs_node dev;
struct vfs_node *devEntries[32];

struct vfs_node devStdin;
struct vfs_node devStdout;
struct vfs_node devRtc;

static struct vfs_node *dev_walk(struct vfs_node *node, unsigned int index)
{

    if (index < dev.length)
        return devEntries[index];
    else
        return 0;

}

static unsigned int dev_write(struct vfs_node *node, unsigned int offset, unsigned int count, void *buffer)
{

    devEntries[dev.length] = (struct vfs_node *)buffer;
    dev.length++;

    return count;

}

void dev_init()
{

    memory_set(&dev, 0, sizeof (struct vfs_node));
    string_copy(dev.name, "dev");
    dev.write = dev_write;
    dev.walk = dev_walk;

    memory_set(&devStdin, 0, sizeof (struct vfs_node));
    string_copy(devStdin.name, "stdin");

    memory_set(&devStdout, 0, sizeof (struct vfs_node));
    string_copy(devStdout.name, "stdout");

    memory_set(&devRtc, 0, sizeof (struct vfs_node));
    string_copy(devRtc.name, "rtc");

    vfs_write(&dev, 0, 1, &devStdin);
    vfs_write(&dev, 0, 1, &devStdout);
    vfs_write(&dev, 0, 1, &devRtc);

    struct vfs_node *root = call_vfs_find(".");
    vfs_write(root, 0, 1, &dev);

}

