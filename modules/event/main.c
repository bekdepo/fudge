#include <fudge.h>
#include <kernel.h>
#include <system/system.h>
#include "event.h"

static struct system_node root;
static struct system_node send;
static struct system_node wm;

void event_notify(unsigned int type, unsigned int count, void *buffer)
{

    struct event_header header;

    header.destination = 0xFFFFFFFF;
    header.source = 0;
    header.type = type;
    header.count = count;

    scheduler_mailboxes_send(&wm.mailboxes, sizeof (struct event_header), &header);
    scheduler_mailboxes_send(&wm.mailboxes, count, buffer);

}

static unsigned int send_write(struct system_node *self, unsigned int offset, unsigned int count, void *buffer)
{

    return scheduler_mailboxes_send(&wm.mailboxes, count, buffer);

}

void module_init()
{

    system_initnode(&send, SYSTEM_NODETYPE_NORMAL, "send");

    send.write = send_write;

    system_initnode(&wm, SYSTEM_NODETYPE_MAILBOX, "wm");
    system_initnode(&root, SYSTEM_NODETYPE_GROUP, "event");
    system_addchild(&root, &send);
    system_addchild(&root, &wm);

}

void module_register()
{

    system_registernode(&root);

}

void module_unregister()
{

    system_unregisternode(&root);

}

