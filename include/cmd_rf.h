/*
 * cmd_rf.h
 *
 *  Created on: May 4, 2018
 *      Author: johann
 */

#ifndef INCLUDE_CMD_RF_H_
#define INCLUDE_CMD_RF_H_

#include <common.h>
#include <command.h>

#if defined(RALINK_RW_RF_REG_FUN)
#if defined(RALINK_CMDLINE)
int do_rw_rf(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif /* defined(RALINK_CMDLINE) */

int rw_rf_reg(int write, int reg, int *data);
#endif /* defined(RALINK_RW_RF_REG_FUN) */

#endif /* INCLUDE_CMD_RF_H_ */
