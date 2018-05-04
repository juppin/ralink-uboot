/*
 * cmd_rf.c
 *
 *  Created on: May 4, 2018
 *      Author: johann
 */

#include <common.h>
#include <cmd_rf.h>

#if defined(RALINK_RW_RF_REG_FUN)
#if defined(RALINK_CMDLINE)
int do_rw_rf(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int write, reg, data;

    if ((argv[1][0] == 'r' || argv[1][0] == 'R') && (argc == 3))
    {
        write = 0;
        reg = (int)simple_strtoul(argv[2], NULL, 10);
        data = 0;
    }
    else if ((argv[1][0] == 'w' || argv[1][0] == 'W') && (argc == 4))
    {
        write = 1;
        reg = (int)simple_strtoul(argv[2], NULL, 10);
        data = (int)simple_strtoul(argv[3], NULL, 16);
    }
    else
    {
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    rw_rf_reg(write, reg, &data);
    if (!write)
    printf("rf reg <%d> = 0x%x\n", reg, data);
    return 0;
}

U_BOOT_CMD(
    rf,     4,     1,      do_rw_rf,
    "rf      - read/write rf register\n",
    "usage:\n"
    "rf r <reg>        - read rf register\n"
    "rf w <reg> <data> - write rf register (reg: decimal, data: hex)\n"
);
#endif /* defined(RALINK_CMDLINE) */

#if defined(MT7620_ASIC_BOARD)
#define RF_CSR_CFG      0xb0180500
#define RF_CSR_KICK     (1<<0)
int rw_rf_reg(int write, int reg, int *data)
{
    u32 rfcsr, i = 0;

    while (1)
    {
        rfcsr = RALINK_REG(RF_CSR_CFG);
        if (! (rfcsr & (u32)RF_CSR_KICK) )
        break;
        if (++i > 10000)
        {
            puts("Warning: Abort rw rf register: too busy\n");
            return -1;
        }
    }
    rfcsr = (u32)(RF_CSR_KICK | ((reg & 0x3f) << 16) | ((*data & 0xff) << 8));
    if (write)
    rfcsr |= 0x10;

    RALINK_REG(RF_CSR_CFG) = cpu_to_le32(rfcsr);
    i = 0;
    while (1)
    {
        rfcsr = RALINK_REG(RF_CSR_CFG);
        if (! (rfcsr & (u32)RF_CSR_KICK) )
        break;
        if (++i > 10000)
        {
            puts("Warning: still busy\n");
            return -1;
        }
    }

    rfcsr = RALINK_REG(RF_CSR_CFG);
    if (((rfcsr & 0x3f0000) >> 16) != (reg & 0x3f))
    {
        puts("Error: rw register failed\n");
        return -1;
    }
    *data = (int)( (rfcsr & 0xff00) >> 8);
    return 0;
}
#else /* defined (MT7620_ASIC_BOARD) */
#define RF_CSR_CFG      0xb0180500
#define RF_CSR_KICK     (1<<17)
int rw_rf_reg(int write, int reg, int *data)
{
    u32 rfcsr, i = 0;

    while (1)
    {
        rfcsr = RALINK_REG(RF_CSR_CFG);
        if (! (rfcsr & (u32)RF_CSR_KICK) )
        break;
        if (++i > 10000)
        {
            puts("Warning: Abort rw rf register: too busy\n");
            return -1;
        }
    }

    rfcsr = (u32)(RF_CSR_KICK | ((reg & 0x3f) << 8) | (*data & 0xff));
    if (write)
    rfcsr |= 0x10000;

    RALINK_REG(RF_CSR_CFG) = cpu_to_le32(rfcsr);

    i = 0;
    while (1)
    {
        rfcsr = RALINK_REG(RF_CSR_CFG);
        if (! (rfcsr & (u32)RF_CSR_KICK) )
        break;
        if (++i > 10000)
        {
            puts("Warning: still busy\n");
            return -1;
        }
    }

    rfcsr = RALINK_REG(RF_CSR_CFG);

    if (((rfcsr&0x1f00) >> 8) != (reg & 0x1f))
    {
        puts("Error: rw register failed\n");
        return -1;
    }
    *data = (int)(rfcsr & 0xff);

    return 0;
}
#endif /* defined (MT7620_ASIC_BOARD) */
#endif /* defined(RALINK_RW_RF_REG_FUN) */
