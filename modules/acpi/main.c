#include <lib/memory.h>
#include <kernel/log.h>
#include <kernel/mmu.h>
#include <modules/acpi/acpi.h>

static struct mmu_memory acpiMemory;

static void write_sdt(struct acpi_sdth *sdt)
{

    char signature[5];
    signature[4] = '\0';

    memory_copy(signature, sdt->signature, 4);

    log_write("[acpi] SDT Signature: %s\n", signature);
    log_write("[acpi] SDT Length: %d\n", sdt->length);
//    log_write("[acpi] SDT Revision: %d\n", sdt->revision);
//    log_write("[acpi] SDT Checksum: %d\n", sdt->checksum);
//    log_write("[acpi] SDT OEM: %s\n", sdt->oem);
//    log_write("[acpi] SDT OEMRevision: %d\n", sdt->oemRevision);
//    log_write("[acpi] SDT Creator: %s\n", sdt->creator);
//    log_write("[acpi] SDT CreatorRevision: %d\n", sdt->creatorRevision);

    if (memory_compare(sdt->signature, "RSDT", 4))
        return;

    void **tables = (void *)sdt + sizeof (struct acpi_sdth);
    unsigned int entries = (sdt->length - sizeof (struct acpi_sdth)) / 4;

    log_write("[acpi] RSDT Entries: %d\n", entries);

    unsigned int i;

    for (i = 0; i < entries; i++)
        write_sdt(tables[i]);

}

static void *get_rsdp()
{

    void *rsdp;
    char *signature = "RSD PTR ";

    for (rsdp = (void *)0x000E0000; rsdp < (void *)0x00100000; rsdp += 0x10)
    {

        if (!memory_compare(rsdp, signature, 8))
            return rsdp;

    }

    unsigned int ebda = *((unsigned int *)0x40E);
    ebda = ebda * 0x10 & 0x000FFFFF;

    for (rsdp = (void *)ebda; rsdp < (void *)(ebda + 0x400); rsdp += 0x10)
    {

        if (!memory_compare(rsdp, signature, 8))
            return rsdp;

    }

    return 0;

}

void init()
{

    struct acpi_rsdp *rsdp = get_rsdp();

    if (!rsdp)
        return;

    log_write("[acpi] RSDP Address: 0x%x\n", rsdp);
    log_write("[acpi] RSDP Checksum: %d\n", rsdp->checksum);
    log_write("[acpi] RSDP Revision: %d.0\n", rsdp->revision + 1);
    log_write("[acpi] RSDP OEM: %s\n", rsdp->oem);

    struct mmu_memory *memory = &acpiMemory;

    mmu_memory_init(memory, (void *)rsdp->rsdt, (void *)rsdp->rsdt, 0x10000); 
    mmu_map_kernel_memory(memory);
    mmu_reload_memory();

    write_sdt((struct acpi_sdth *)rsdp->rsdt);

}

void destroy()
{

}

