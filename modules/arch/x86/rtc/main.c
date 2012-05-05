#include <lib/memory.h>
#include <kernel/modules.h>
#include <modules/nodefs/nodefs.h>
#include <modules/rtc/rtc.h>

static struct rtc_device device;
static struct nodefs_node timestamp;

static unsigned int timestamp_read(struct nodefs_node *self, unsigned int offset, unsigned int count, void *buffer)
{

    memory_copy(buffer, 0, 0);

    return 0;

}

void init()
{

    rtc_device_init(&device, RTC_IRQ);
    modules_register_device(&device.base);

    struct nodefs_driver *nodefsDriver = (struct nodefs_driver *)modules_get_driver(NODEFS_DRIVER_TYPE);

    if (!nodefsDriver)
        return;

    nodefsDriver->register_node(nodefsDriver, &timestamp, "rtc/timestamp", &device.base.base, timestamp_read, 0);

}

void destroy()
{

    modules_unregister_device(&device.base);

}

