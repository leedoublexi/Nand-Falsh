#ifndef __NAND_FLASH_MANAGER_H__
#define __NAND_FLASH_MANAGER_H__

class nand_flash_manager
{
    public:
        nand_flash_manager();
        ~nand_flash_manager();
        int flash();
        int verify();

    private:
};
#endif
