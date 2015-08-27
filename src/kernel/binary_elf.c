#include <fudge.h>
#include <format/elf.h>
#include "resource.h"
#include "vfs.h"
#include "binary.h"

static struct binary_protocol protocol;

static unsigned int protocol_match(struct vfs_channel *channel, unsigned int id)
{

    struct elf_header header;

    if (!channel->protocol->read(channel->backend, id, 0, ELF_HEADER_SIZE, &header))
        return 0;

    return elf_validate(&header);

}

static unsigned long protocol_findsymbol(struct vfs_channel *channel, unsigned int id, unsigned int count, char *symbol)
{

    struct elf_header header;
    struct elf_sectionheader sectionheader[32];
    unsigned int i;

    channel->protocol->read(channel->backend, id, 0, ELF_HEADER_SIZE, &header);

    if (header.shcount > 32)
        return 0;

    channel->protocol->read(channel->backend, id, header.shoffset, header.shsize * header.shcount, sectionheader);

    for (i = 0; i < header.shcount; i++)
    {

        struct elf_symbol symbols[1024];
        char strings[4096];
        unsigned long address;
        struct elf_sectionheader *symbolheader;
        struct elf_sectionheader *stringheader;

        if (sectionheader[i].type != ELF_SECTION_TYPE_SYMTAB)
            continue;

        symbolheader = &sectionheader[i];
        stringheader = &sectionheader[symbolheader->link];

        if (symbolheader->size > sizeof (struct elf_symbol) * 1024)
            return 0;

        if (stringheader->size > 4096)
            return 0;

        channel->protocol->read(channel->backend, id, symbolheader->offset, symbolheader->size, symbols);
        channel->protocol->read(channel->backend, id, stringheader->offset, stringheader->size, strings);

        address = elf_findsymbol(&header, sectionheader, symbolheader, symbols, strings, count, symbol);

        if (address)
            return address;

    }

    return 0;

}

static unsigned long protocol_findentry(struct vfs_channel *channel, unsigned int id)
{

    struct elf_header header;

    channel->protocol->read(channel->backend, id, 0, ELF_HEADER_SIZE, &header);

    return header.entry;

}

static unsigned long protocol_findbase(struct vfs_channel *channel, unsigned int id, unsigned long address)
{

    struct elf_header header;
    struct elf_programheader programheader[8];
    unsigned int i;

    channel->protocol->read(channel->backend, id, 0, ELF_HEADER_SIZE, &header);

    if (header.phcount > 8)
        return 0;

    channel->protocol->read(channel->backend, id, header.phoffset, header.phsize * header.phcount, programheader);

    for (i = 0; i < header.phcount; i++)
    {

        if (programheader[i].vaddress <= address && programheader[i].vaddress + programheader[i].msize > address)
            return programheader[i].vaddress;

    }

    return 0;

}

static void protocol_copyprogram(struct vfs_channel *channel, unsigned int id)
{

    struct elf_header header;
    struct elf_programheader programheader[8];
    unsigned int i;

    channel->protocol->read(channel->backend, id, 0, ELF_HEADER_SIZE, &header);

    if (header.phcount > 8)
        return;

    channel->protocol->read(channel->backend, id, header.phoffset, header.phsize * header.phcount, programheader);

    for (i = 0; i < header.phcount; i++)
    {

        memory_clear((void *)(programheader[i].vaddress + programheader[i].offset), programheader[i].msize);

        channel->protocol->read(channel->backend, id, programheader[i].offset, programheader[i].fsize, (void *)programheader[i].vaddress);

    }

}

static unsigned int protocol_relocate(struct vfs_channel *channel, unsigned int id, unsigned int address)
{

    struct elf_header header;
    struct elf_sectionheader sectionheader[32];
    unsigned int i;

    channel->protocol->read(channel->backend, id, 0, ELF_HEADER_SIZE, &header);

    if (header.shcount > 32)
        return 0;

    channel->protocol->read(channel->backend, id, header.shoffset, header.shsize * header.shcount, sectionheader);

    for (i = 0; i < header.shcount; i++)
    {

        struct elf_relocation relocations[1024];
        struct elf_symbol symbols[1024];
        struct elf_sectionheader *relocationheader;
        struct elf_sectionheader *dataheader;
        struct elf_sectionheader *symbolheader;

        sectionheader[i].address += address;

        if (sectionheader[i].type != ELF_SECTION_TYPE_REL)
            continue;

        relocationheader = &sectionheader[i];
        dataheader = &sectionheader[relocationheader->info];
        symbolheader = &sectionheader[relocationheader->link];

        if (relocationheader->size > sizeof (struct elf_relocation) * 1024)
            return 0;

        if (symbolheader->size > sizeof (struct elf_symbol) * 1024)
            return 0;

        channel->protocol->read(channel->backend, id, relocationheader->offset, relocationheader->size, relocations);
        channel->protocol->read(channel->backend, id, symbolheader->offset, symbolheader->size, symbols);
        elf_relocatesection(sectionheader, relocationheader, dataheader, relocations, symbols, address);

    }

    channel->protocol->write(channel->backend, id, header.shoffset, header.shsize * header.shcount, sectionheader);

    return 1;

}

void binary_setupelf(void)
{

    binary_initprotocol(&protocol, protocol_match, protocol_findsymbol, protocol_findentry, protocol_findbase, protocol_copyprogram, protocol_relocate);
    resource_register(&protocol.resource);

}

