#include <fudge/kernel.h>
#include "vfs.h"

static struct list protocols;
static struct list backends;

unsigned int vfs_findnext(unsigned int count, const char *path)
{

    unsigned int i;

    for (i = 0; i < count; i++)
    {

        if (path[i] == '/')
            return i + 1;

    }

    return count;

}

unsigned int vfs_isparent(unsigned int count, const char *path)
{

    return (count >= 3) ? memory_match(path, "../", 3) : 0;

}

struct vfs_protocol *vfs_find_protocol(struct vfs_backend *backend)
{

    struct list_item *current;

    for (current = protocols.head; current; current = current->next)
    {

        struct vfs_protocol *protocol = current->self;

        if (protocol->match(backend))
            return protocol;

    }

    return 0;

}

void vfs_register_backend(struct vfs_backend *backend)
{

    list_add(&backends, &backend->item);

}

void vfs_register_protocol(struct vfs_protocol *protocol)
{

    list_add(&protocols, &protocol->item);

}

void vfs_unregister_backend(struct vfs_backend *backend)
{

    list_remove(&backends, &backend->item);

}

void vfs_unregister_protocol(struct vfs_protocol *protocol)
{

    list_remove(&protocols, &protocol->item);

}

void vfs_init_backend(struct vfs_backend *backend, unsigned int (*read)(struct vfs_backend *self, unsigned int offset, unsigned int count, void *buffer), unsigned int (*write)(struct vfs_backend *self, unsigned int offset, unsigned int count, void *buffer))
{

    memory_clear(backend, sizeof (struct vfs_backend));
    list_init_item(&backend->item, backend);

    backend->read = read;
    backend->write = write;

}

void vfs_init_protocol(struct vfs_protocol *protocol, unsigned int (*match)(struct vfs_backend *backend), unsigned int (*root)(struct vfs_backend *backend), unsigned int (*open)(struct vfs_backend *backend, unsigned int id), unsigned int (*close)(struct vfs_backend *backend, unsigned int id), unsigned int (*read)(struct vfs_backend *backend, unsigned int id, unsigned int offset, unsigned int count, void *buffer), unsigned int (*write)(struct vfs_backend *backend, unsigned int id, unsigned int offset, unsigned int count, void *buffer), unsigned int (*parent)(struct vfs_backend *backend, unsigned int id), unsigned int (*walk)(struct vfs_backend *backend, unsigned int id, unsigned int count, const char *path), unsigned int (*get_physical)(struct vfs_backend *backend, unsigned int id))
{

    memory_clear(protocol, sizeof (struct vfs_protocol));
    list_init_item(&protocol->item, protocol);

    protocol->match = match;
    protocol->root = root;
    protocol->open = open;
    protocol->close = close;
    protocol->read = read;
    protocol->write = write;
    protocol->parent = parent;
    protocol->walk = walk;
    protocol->get_physical = get_physical;

}

void vfs_setup()
{

    list_init(&protocols);
    list_init(&backends);

}

