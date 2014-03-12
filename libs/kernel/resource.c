#include <kernel.h>
#include "resource.h"

static struct list resources;

struct resource_item *resource_find_item(struct resource_iterator *iterator, struct resource_item *item)
{

    struct list_item *current = (item) ? item->item.next : resources.head;

    for (; current; current = current->next)
    {

        struct resource_item *a = current->data;

        if (iterator->match(a))
            return a;

    }

    return 0;

}

void resource_register_item(struct resource_item *item)
{

    list_add(&resources, &item->item);

}

void resource_init_item(struct resource_item *item, enum resource_type type, void *data)
{

    memory_clear(item, sizeof (struct resource_item));
    list_init_item(&item->item, item);

    item->type = type;
    item->data = data;

}

void resource_init_iterator(struct resource_iterator *iterator, unsigned int (*match)(struct resource_item *item))
{

    memory_clear(iterator, sizeof (struct resource_iterator));

    iterator->match = match;

}

void resource_setup()
{

    list_init(&resources);

}

