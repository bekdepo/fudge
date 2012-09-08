#include <vfs.h>
#include <base/base.h>
#include <block/block.h>

static struct block_filesystem filesystem;

void block_register_interface(struct block_interface *interface, struct base_driver *driver)
{

    interface->driver = driver;

    filesystem.interfaces[filesystem.interfacesCount] = interface;
    filesystem.interfacesCount++;

}

void block_register_protocol(struct block_protocol *protocol)
{

    filesystem.protocols[filesystem.protocolsCount] = protocol;
    filesystem.protocolsCount++;

}

void block_unregister_interface(struct block_interface *interface)
{

}

void block_unregister_protocol(struct block_protocol *protocol)
{

}

struct vfs_interface *get_filesystem()
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

