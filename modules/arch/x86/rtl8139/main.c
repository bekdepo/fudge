#include <module.h>
#include <kernel/resource.h>
#include <base/base.h>
#include <base/network.h>
#include <arch/x86/pic/pic.h>
#include <arch/x86/io/io.h>
#include <arch/x86/pci/pci.h>

#define RTL8139_PCI_VENDOR              0x10EC
#define RTL8139_PCI_DEVICE              0x8139

enum rtl8139_register
{

    RTL8139_REGISTER_IDR0               = 0x00,
    RTL8139_REGISTER_IDR1               = 0x01,
    RTL8139_REGISTER_IDR2               = 0x02,
    RTL8139_REGISTER_IDR3               = 0x03,
    RTL8139_REGISTER_IDR4               = 0x04,
    RTL8139_REGISTER_IDR5               = 0x05,
    RTL8139_REGISTER_MAR0               = 0x08,
    RTL8139_REGISTER_MAR1               = 0x09,
    RTL8139_REGISTER_MAR2               = 0x0A,
    RTL8139_REGISTER_MAR3               = 0x0B,
    RTL8139_REGISTER_MAR4               = 0x0C,
    RTL8139_REGISTER_MAR5               = 0x0D,
    RTL8139_REGISTER_MAR6               = 0x0E,
    RTL8139_REGISTER_MAR7               = 0x0F,
    RTL8139_REGISTER_TSD0               = 0x10,
    RTL8139_REGISTER_TSD1               = 0x14,
    RTL8139_REGISTER_TSD2               = 0x18,
    RTL8139_REGISTER_TSD3               = 0x1C,
    RTL8139_REGISTER_TSAD0              = 0x20,
    RTL8139_REGISTER_TSAD1              = 0x24,
    RTL8139_REGISTER_TSAD2              = 0x28,
    RTL8139_REGISTER_TSAD3              = 0x2C,
    RTL8139_REGISTER_RBSTART            = 0x30,
    RTL8139_REGISTER_ERBCR              = 0x34,
    RTL8139_REGISTER_ERSR               = 0x36,
    RTL8139_REGISTER_CR                 = 0x37,
    RTL8139_REGISTER_CAPR               = 0x38,
    RTL8139_REGISTER_CBR                = 0x3A,
    RTL8139_REGISTER_IMR                = 0x3C,
    RTL8139_REGISTER_ISR                = 0x3E,
    RTL8139_REGISTER_TCR                = 0x40,
    RTL8139_REGISTER_RCR                = 0x44,
    RTL8139_REGISTER_TCTR               = 0x48,
    RTL8139_REGISTER_MPC                = 0x4C,
    RTL8139_REGISTER_9346CR             = 0x50,
    RTL8139_REGISTER_CONFIG0            = 0x51,
    RTL8139_REGISTER_CONFIG1            = 0x52,
    RTL8139_REGISTER_TIMERINT           = 0x54,
    RTL8139_REGISTER_MSR                = 0x58,
    RTL8139_REGISTER_CONFIG3            = 0x59,
    RTL8139_REGISTER_CONFIG4            = 0x5A,
    RTL8139_REGISTER_MULINT             = 0x5C,
    RTL8139_REGISTER_RERID              = 0x5E,
    RTL8139_REGISTER_TSAD               = 0x60,
    RTL8139_REGISTER_BMCR               = 0x62,
    RTL8139_REGISTER_BMSR               = 0x64,
    RTL8139_REGISTER_ANAR               = 0x66,
    RTL8139_REGISTER_ANLPAR             = 0x68,
    RTL8139_REGISTER_ANER               = 0x6A,
    RTL8139_REGISTER_DIS                = 0x6C

};

enum
{

    RTL8139_RCR_AAP                     = (1 << 0),
    RTL8139_RCR_APM                     = (1 << 1),
    RTL8139_RCR_AM                      = (1 << 2),
    RTL8139_RCR_AB                      = (1 << 3),
    RTL8139_RCR_AR                      = (1 << 4),
    RTL8139_RCR_AER                     = (1 << 5),
    RTL8139_RCR_WRAP                    = (1 << 7)

};

enum
{

    RTL8139_CR_REMPTY                   = (1 << 0),
    RTL8139_CR_TENABLE                  = (1 << 2),
    RTL8139_CR_RENABLE                  = (1 << 3),
    RTL8139_CR_RESET                    = (1 << 4)

};

enum
{

    RTL8139_ISR_ROK                     = (1 << 0),
    RTL8139_ISR_RER                     = (1 << 1),
    RTL8139_ISR_TOK                     = (1 << 2),
    RTL8139_ISR_TER                     = (1 << 3)

};

enum
{

    RTL8139_HEADERFLAG_ROK              = (1 << 0),
    RTL8139_HEADERFLAG_FAE              = (1 << 1),
    RTL8139_HEADERFLAG_CRC              = (1 << 2),
    RTL8139_HEADERFLAG_LONG             = (1 << 3),
    RTL8139_HEADERFLAG_RUNT             = (1 << 4),
    RTL8139_HEADERFLAG_ISE              = (1 << 5),
    RTL8139_HEADERFLAG_BAR              = (1 << 13),
    RTL8139_HEADERFLAG_PAM              = (1 << 14),
    RTL8139_HEADERFLAG_MAR              = (1 << 15)

};

struct rtl8139_header
{

    unsigned short flags;
    unsigned short length;

};

static struct base_driver driver;
static struct base_network_interface inetwork;
static unsigned short io;
static unsigned int mmio;
static unsigned char rx[0x2600];
static unsigned char tx0[0x800];
static unsigned char tx1[0x800];
static unsigned char tx2[0x800];
static unsigned char tx3[0x800];
static unsigned short rxp;
static unsigned short txp;

static void poweron()
{

    io_outb(io + RTL8139_REGISTER_CONFIG1, 0x00);

}

static void reset()
{

    io_outb(io + RTL8139_REGISTER_CR, RTL8139_CR_RESET);

    while (io_inb(io + RTL8139_REGISTER_CR) & RTL8139_CR_RESET);

}

static void enable()
{

    io_outb(io + RTL8139_REGISTER_CR, RTL8139_CR_RENABLE | RTL8139_CR_TENABLE);

}

static void setup_interrupts(unsigned short flags)
{

    io_outw(io + RTL8139_REGISTER_IMR, flags);

}

static void setup_receiver()
{

    io_outd(io + RTL8139_REGISTER_RBSTART, (unsigned long)rx);
    io_outd(io + RTL8139_REGISTER_RCR, RTL8139_RCR_AAP | RTL8139_RCR_APM | RTL8139_RCR_AM | RTL8139_RCR_AB | RTL8139_RCR_WRAP);

}

static void setup_transmitter()
{

    io_outd(io + RTL8139_REGISTER_TSAD0, (unsigned long)tx0);
    io_outd(io + RTL8139_REGISTER_TSAD1, (unsigned long)tx1);
    io_outd(io + RTL8139_REGISTER_TSAD2, (unsigned long)tx2);
    io_outd(io + RTL8139_REGISTER_TSAD3, (unsigned long)tx3);

}

/*
static void log(char *text, unsigned int count)
{

    unsigned int i;

    for (i = 0; i < count; i++)
    {

        memory_copy((void *)(0xB8640 + i * 2 + 1), " ", 1);
        memory_copy((void *)(0xB8640 + i * 2), text + i, 1);

    }

}
*/

static void *get_packet(struct base_device *device)
{

    return rx + rxp;

}

static void dump_packet(struct base_device *device)
{

    struct rtl8139_header *header = get_packet(device);

    rxp += (header->length + 4 + 3) & ~3;

    io_outw(io + RTL8139_REGISTER_CAPR, rxp);

}

static unsigned int receive(struct base_device *device, unsigned int count, void *buffer)
{

    struct rtl8139_header *header = get_packet(device);
    unsigned int k;

    /*
    char b[1000];
    unsigned int c = 0;
    unsigned short total = io_inw(io + RTL8139_REGISTER_CBR);

    c += memory_write(b, 1000, "rec", 3, c);
    c += memory_write(b, 1000, " ", 1, c);
    c += memory_write(b, 1000, "total: ", 7, c);
    c += ascii_write_value(b, 1000, total, 16, c);
    c += memory_write(b, 1000, " ", 1, c);
    c += memory_write(b, 1000, "hf: ", 4, c);
    c += ascii_write_value(b, 1000, header->flags, 16, c);
    c += memory_write(b, 1000, " ", 1, c);
    c += memory_write(b, 1000, "hl: ", 4, c);
    c += ascii_write_value(b, 1000, header->length, 16, c);
    c += memory_write(b, 1000, " ", 1, c);
    c += memory_write(b, 1000, "rxp: ", 5, c);
    c += ascii_write_value(b, 1000, rxp, 16, c);

    log(b, c);

    */

    k = memory_read(buffer, count, rx + rxp + sizeof (struct rtl8139_header), header->length, 0);

    dump_packet(device);

    return k;

}

static unsigned int send(struct base_device *device, unsigned int count, void *buffer)
{

    unsigned int status = (0x3F << 16) | (count & 0x1FFF);

    switch (txp)
    {

    case 0:
        memory_write(tx0, 0x800, buffer, count, 0);
        io_outd(io + RTL8139_REGISTER_TSD0, status);

        break;

    case 1:
        memory_write(tx1, 0x800, buffer, count, 0);
        io_outd(io + RTL8139_REGISTER_TSD1, status);

        break;

    case 2:
        memory_write(tx2, 0x800, buffer, count, 0);
        io_outd(io + RTL8139_REGISTER_TSD2, status);

        break;

    case 3:
        memory_write(tx3, 0x800, buffer, count, 0);
        io_outd(io + RTL8139_REGISTER_TSD3, status);

        break;

    }

    txp++;
    txp %= 4;

    return count;

}

static void handle_irq(struct base_device *device)
{

    unsigned short status = io_inw(io + RTL8139_REGISTER_ISR);

    if (status & RTL8139_ISR_ROK)
        io_outw(io + RTL8139_REGISTER_ISR, RTL8139_ISR_ROK);

    if (status & RTL8139_ISR_TOK)
        io_outw(io + RTL8139_REGISTER_ISR, RTL8139_ISR_TOK);

}

static void attach(struct base_device *device)
{

    struct pci_device *pciDevice = (struct pci_device *)device;
    unsigned int bar0 = pci_device_ind(pciDevice, PCI_CONFIG_BAR0);
    unsigned int bar1 = pci_device_ind(pciDevice, PCI_CONFIG_BAR1);
    unsigned short command = pci_device_inw(pciDevice, PCI_CONFIG_COMMAND);

    io = bar0 & ~1;
    mmio = bar1;

    base_network_init_interface(&inetwork, receive, send, get_packet, dump_packet);
    base_network_register_interface(&inetwork, device);
    pci_device_outw(pciDevice, PCI_CONFIG_COMMAND, command | (1 << 2));
    pic_set_routine(device, handle_irq);
    poweron();
    reset();
    setup_interrupts(RTL8139_ISR_ROK | RTL8139_ISR_TOK);
    setup_receiver();
    setup_transmitter();
    enable();

    inetwork.mac[0] = io_inb(io + RTL8139_REGISTER_IDR0);
    inetwork.mac[1] = io_inb(io + RTL8139_REGISTER_IDR1);
    inetwork.mac[2] = io_inb(io + RTL8139_REGISTER_IDR2);
    inetwork.mac[3] = io_inb(io + RTL8139_REGISTER_IDR3);
    inetwork.mac[4] = io_inb(io + RTL8139_REGISTER_IDR4);
    inetwork.mac[5] = io_inb(io + RTL8139_REGISTER_IDR5);

}

static void detach(struct base_device *device)
{

    base_network_unregister_interface(&inetwork);
    pic_unset_routine(device);

}

static unsigned int check(struct base_device *device)
{

    struct pci_device *pciDevice = (struct pci_device *)device;

    if (device->type != PCI_DEVICE_TYPE)
        return 0;

    return pci_device_inw(pciDevice, PCI_CONFIG_VENDOR) == RTL8139_PCI_VENDOR && pci_device_inw(pciDevice, PCI_CONFIG_DEVICE) == RTL8139_PCI_DEVICE;

}

void init()
{

    base_init_driver(&driver, "rtl8139", check, attach, detach);
    base_register_driver(&driver);

}

void destroy()
{

    base_unregister_driver(&driver);

}

