struct ata_driver
{

    struct base_driver base;
    struct base_block_interface iblock;

};

void ata_init_driver(struct ata_driver *driver);