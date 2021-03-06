#include <fudge.h>
#include "resource.h"
#include "binary.h"

struct binary_format *binary_findformat(struct binary_node *node)
{

    struct resource *current = 0;

    while ((current = resource_findtype(current, RESOURCE_BINARYFORMAT)))
    {

        struct binary_format *format = current->data;

        if (format->match(node))
            return format;

    }

    return 0;

}

void binary_initformat(struct binary_format *format, unsigned int (*match)(struct binary_node *node), unsigned long (*findsymbol)(struct binary_node *node, unsigned int count, char *symbol), unsigned long (*findentry)(struct binary_node *node), unsigned long (*findbase)(struct binary_node *node, unsigned long address), unsigned int (*copyprogram)(struct binary_node *node), unsigned int (*relocate)(struct binary_node *node))
{

    resource_init(&format->resource, RESOURCE_BINARYFORMAT, format);

    format->match = match;
    format->findsymbol = findsymbol;
    format->findentry = findentry;
    format->findbase = findbase;
    format->copyprogram = copyprogram;
    format->relocate = relocate;

}

