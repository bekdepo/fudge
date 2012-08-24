#include <vfs.h>
#include <modules/modules.h>
#include <block/block.h>

static struct block_filesystem filesystem;

void block_register_interface(struct block_interface *interface, struct modules_driver *driver, unsigned int (*read)(struct block_interface *self, unsigned int offset, unsigned int count, void *buffer), unsigned int (*write)(struct block_interface *self, unsigned int offset, unsigned int count, void *buffer))
{

    filesystem.register_interface(&filesystem, interface, driver, read, write);

}

void block_register_protocol(struct block_protocol *protocol, char *name)
{

    filesystem.register_protocol(&filesystem, protocol, name);

}

void block_unregister_interface(struct block_interface *interface)
{

    filesystem.unregister_interface(&filesystem, interface);

}

void block_unregister_protocol(struct block_protocol *protocol)
{

    filesystem.unregister_protocol(&filesystem, protocol);

}

struct vfs_filesystem *get_filesystem()
{

    return &filesystem.base;

}

void init()
{

    block_filesystem_init(&filesystem);

}

void destroy()
{

}

