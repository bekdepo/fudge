#include <fudge.h>
#include <kernel.h>
#include <modules/system/system.h>
#include <modules/ethernet/ethernet.h>
#include <modules/ipv4/ipv4.h>

static struct ipv4_protocol ipv4protocol;

void ipv4protocol_notify(struct ethernet_interface *interface, void *buffer, unsigned int count)
{

    kernel_multicast(&ipv4protocol.datalinks, buffer, count);

}

void module_init(void)
{

    ipv4_initprotocol(&ipv4protocol, "icmp", 0x01, ipv4protocol_notify);

}

void module_register(void)
{

    ipv4_registerprotocol(&ipv4protocol);

}

void module_unregister(void)
{

    ipv4_unregisterprotocol(&ipv4protocol);

}

