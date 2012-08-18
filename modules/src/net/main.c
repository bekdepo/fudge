#include <modules.h>
#include <net/net.h>

static struct net_filesystem filesystem;

void net_register_interface(struct net_interface *interface, struct modules_base *module, unsigned int (*read)(struct net_interface *self, unsigned int offset, unsigned int count, void *buffer), unsigned int (*write)(struct net_interface *self, unsigned int offset, unsigned int count, void *buffer))
{

    filesystem.register_interface(&filesystem, interface, module, read, write);

}

void net_register_protocol(struct net_protocol *protocol, char *name)
{

    filesystem.register_protocol(&filesystem, protocol, name);

}

void net_unregister_interface(struct net_interface *interface)
{

    filesystem.unregister_interface(&filesystem, interface);

}

void net_unregister_protocol(struct net_protocol *protocol)
{

    filesystem.unregister_protocol(&filesystem, protocol);

}

struct modules_filesystem *get_filesystem()
{

    return &filesystem.base;

}

void init()
{

    net_filesystem_init(&filesystem);
    modules_register_filesystem(&filesystem.base);

}

void destroy()
{

    modules_unregister_filesystem(&filesystem.base);

}

