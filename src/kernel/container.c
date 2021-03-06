#include <fudge.h>
#include "resource.h"
#include "binary.h"
#include "service.h"
#include "container.h"

void container_init(struct container *container)
{

    unsigned int i;

    resource_init(&container->resource, RESOURCE_CONTAINER, container);

    for (i = 0; i < CONTAINER_MOUNTS; i++)
        resource_init(&container->mounts[i].resource, RESOURCE_CONTAINERMOUNT, &container->mounts[i]);

}

