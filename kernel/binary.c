#include <fudge/format/elf.h>
#include "vfs.h"
#include "binary.h"

unsigned int binary_find_symbol(struct vfs_interface *interface, unsigned int id, char *symbol)
{

    struct elf_header header;
    struct elf_section_header sectionHeader[20];
    unsigned int symbolHeaderIndex;
    struct elf_symbol symbolTable[400];
    char stringTable[0x1000];

    interface->read(interface, id, 0, ELF_HEADER_SIZE, &header);

    if (!elf_validate(&header))
        return 0;

    interface->read(interface, id, header.shoffset, header.shsize * header.shcount, sectionHeader);

    symbolHeaderIndex = elf_find_section(&header, sectionHeader, ELF_SECTION_TYPE_SYMTAB);

    interface->read(interface, id, sectionHeader[symbolHeaderIndex].offset, sectionHeader[symbolHeaderIndex].size, symbolTable);
    interface->read(interface, id, sectionHeader[sectionHeader[symbolHeaderIndex].link].offset, sectionHeader[sectionHeader[symbolHeaderIndex].link].size, stringTable);

    return elf_find_symbol(&header, sectionHeader, symbolHeaderIndex, symbolTable, stringTable, symbol);

}

unsigned int binary_copy_program(struct vfs_interface *interface, unsigned int id)
{

    struct elf_header header;
    struct elf_program_header programHeader[8];
    unsigned int i;

    interface->read(interface, id, 0, ELF_HEADER_SIZE, &header);

    if (!elf_validate(&header))
        return 0;

    interface->read(interface, id, header.phoffset, header.phsize * header.phcount, programHeader);

    for (i = 0; i < header.phcount; i++)
        interface->read(interface, id, programHeader[i].offset, programHeader[i].fsize, (void *)programHeader[i].vaddress);

    return header.entry;

}

unsigned int binary_relocate(struct vfs_interface *interface, unsigned int id, unsigned int address)
{

    struct elf_header header;
    struct elf_section_header sectionHeader[20];
    unsigned int i;

    interface->read(interface, id, 0, ELF_HEADER_SIZE, &header);

    if (!elf_validate(&header))
        return 0;

    interface->read(interface, id, header.shoffset, header.shsize * header.shcount, sectionHeader);

    for (i = 0; i < header.shcount; i++)
    {

        struct elf_relocate relocateTable[200];
        struct elf_symbol symbolTable[400];

        sectionHeader[i].address += address;

        if (sectionHeader[i].type != ELF_SECTION_TYPE_REL)
            continue;

        interface->read(interface, id, sectionHeader[i].offset, sectionHeader[i].size, relocateTable);
        interface->read(interface, id, sectionHeader[sectionHeader[i].link].offset, sectionHeader[sectionHeader[i].link].size, symbolTable);

        elf_relocate_section(sectionHeader, i, sectionHeader[i].info, relocateTable, symbolTable, address);

        interface->write(interface, id, header.shoffset, header.shsize * header.shcount, sectionHeader);

    }

    return 1;

}
