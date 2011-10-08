#include <lib/string.h>
#include <kernel/vfs.h>
#include <kernel/modules.h>
#include <arch/x86/irq.h>
#include <modules/io/io.h>
#include <modules/pci/pci.h>
#include <modules/rtl8139/rtl8139.h>

struct rtl8139_driver rtl8139Driver;

static void rtl8139_handler(struct isr_registers *registers)
{

}

void rtl8139_poweron(struct rtl8139_driver *driver)
{

    io_outb(driver->io + RTL8139_REGISTER_CONFIG1, 0x00);

}

void rtl8139_reset(struct rtl8139_driver *driver)
{

    io_outb(driver->io + RTL8139_REGISTER_CMD, 0x10);

    while (io_inb(driver->io + RTL8139_REGISTER_CMD) & 0x10);

}

void rtl8139_set_rx(struct rtl8139_driver *driver, unsigned int rcr)
{

    io_outd(driver->io + RTL8139_REGISTER_RBSTART, (unsigned int)driver->rx);
    io_outd(driver->io + RTL8139_REGISTER_RCR, rcr);

}

void rtl8139_set_interrupt_flags(struct rtl8139_driver *driver, unsigned short flags)
{

    io_outw(driver->io + RTL8139_REGISTER_IMR, flags);

}

void rtl8139_enable(struct rtl8139_driver *driver)
{

    io_outw(driver->io + RTL8139_REGISTER_CMD, 0x0C);

}

void rtl8139_init()
{

    struct pci_bus *bus = (struct pci_bus *)modules_get_bus(MODULES_BUS_TYPE_PCI);

    if (!bus)
        return;

    struct pci_device *device = bus->find_device(0x8139);

    if (!device)
        return;

    rtl8139Driver.base.device = &device->base;
    rtl8139Driver.io = (device->configuration.bar0 & ~1);

    irq_register_handler(device->configuration.interruptline, rtl8139_handler);

    rtl8139_poweron(&rtl8139Driver);
    rtl8139_reset(&rtl8139Driver);
    rtl8139_set_rx(&rtl8139Driver, 0x0F);
    rtl8139_set_interrupt_flags(&rtl8139Driver, 0x05);
    rtl8139_enable(&rtl8139Driver);

}

