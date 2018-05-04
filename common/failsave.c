/*
 * failsave.c
 *
 *  Created on: May 4, 2018
 *      Author: johann
 */

#include <common.h>
#include <spi_api.h>
#include <nand_api.h>
#include <flash.h>
#include <failsave.h>

/* Restore to default. */
void reset_to_default(void)
{
    ulong addr, size;

    addr = CFG_ENV_ADDR;
    size = CFG_CONFIG_SIZE;

    /* Erase U-Boot Env partition */
#if defined (CFG_ENV_IS_IN_NAND)
    /* Erase only one NAND block */
    size = CFG_BLOCKSIZE;
    ranand_erase((addr-CFG_FLASH_BASE), size);

    /* Erase 'Config' partition with NVRAM */
    if (CFG_NVRAM_ADDR > addr) {
        addr = CFG_NVRAM_ADDR;
        ranand_erase((addr-CFG_FLASH_BASE), size);
    }
#elif defined (CFG_ENV_IS_IN_SPI)
    printf("Erase 0x%08x size 0x%x\n", addr, size);
    raspi_erase((addr-CFG_FLASH_BASE), size);
#else
    printf("Erase 0x%08x size 0x%x\n", addr, size);
    flash_sect_protect(0, addr, addr+size-1);
    flash_sect_erase(addr, addr+size-1);
    flash_sect_protect(1, addr, addr+size-1);
#endif
}
