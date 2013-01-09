#include <fudge/module.h>
#include <kernel/arch/x86/mmu.h>
#include <system/system.h>
#include <base/base.h>
#include <video/video.h>
#include <arch/x86/io/io.h>
#include <arch/x86/pci/pci.h>
#include "bga.h"

static void write_register(unsigned short index, unsigned short data)
{

    io_outw(BGA_COMMAND, index);
    io_outw(BGA_DATA, data);

}

static void start(struct base_driver *self)
{

/*
    struct bga_driver *driver = (struct bga_driver *)self;

    mmu_map_kernel_memory(3, (unsigned int)driver->lfb, (unsigned int)driver->lfb, 0x00400000);
    mmu_reload_memory();
*/

}

static void attach(struct base_device *device)
{

    struct pci_device *pciDevice = (struct pci_device *)device;
    struct bga_driver *driver = (struct bga_driver *)device->driver;

    driver->lfb = (void *)pci_bus_ind(pciDevice->address, PCI_CONFIG_BAR0);

}

static unsigned int check(struct base_device *device)
{

    struct pci_device *pciDevice = (struct pci_device *)device;

    if (device->type != PCI_DEVICE_TYPE)
        return 0;

    return pci_bus_inw(pciDevice->address, PCI_CONFIG_VENDOR) == 0x1234 && pci_bus_inw(pciDevice->address, PCI_CONFIG_DEVICE) == 0x1111;

}

static void enable(struct video_interface *self)
{

    write_register(BGA_COMMAND_ENABLE, 0x00);
    write_register(BGA_COMMAND_XRES, self->xres);
    write_register(BGA_COMMAND_YRES, self->yres);
    write_register(BGA_COMMAND_BPP, self->bpp);
    write_register(BGA_COMMAND_ENABLE, 0x40 | 0x01);

}

static unsigned int read_data(struct video_interface *self, unsigned int offset, unsigned int count, void *buffer)
{

    struct bga_driver *driver = (struct bga_driver *)self->driver;
    unsigned int size = self->xres * self->yres * self->bpp / 4;

    return memory_read(buffer, count, driver->lfb, size, offset);

}

static unsigned int write_data(struct video_interface *self, unsigned int offset, unsigned int count, void *buffer)
{

    struct bga_driver *driver = (struct bga_driver *)self->driver;
    unsigned int size = self->xres * self->yres * self->bpp / 4;

    return memory_write(driver->lfb, size, buffer, count, offset);

}

void bga_init_driver(struct bga_driver *driver)
{

    memory_clear(driver, sizeof (struct bga_driver));
    base_init_driver(&driver->base, BGA_DRIVER_TYPE, "bga", start, check, attach);
    video_init_interface(&driver->interface, &driver->base, enable, read_data, write_data);

    driver->interface.xres = 800;
    driver->interface.yres = 600;
    driver->interface.bpp = BGA_BPP_32;
    driver->bank = (void *)0xA0000;

}

