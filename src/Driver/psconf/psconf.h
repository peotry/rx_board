/**********************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName   PSCONF.h
* Description : The Module of Control QAM160T
* Author    : jie.zhan
* Modified  :
* Reviewer  :
* Date      : 2017-02-26
* Record    :
*
**********************************************************************/

#ifndef PSCONF_H_ 
#define PSCONF_H_ 

#include "appGlobal.h"
#include "err/wv_err.h"

// 子板类型
#define PSCONF_CHIP_ALTERA_GEN          (0x00)
#define PSCONF_CHIP_XILINX_GEN          (0x01)

#define PSCONF_SENDUNIT_LENGTH          (256)

#define PSCONF_START_MASK               (0x80000000)
#define PSCONF_PLATFORM_XILINX_MASK     (0x00000000)
#define PSCONF_PLATFORM_ALTERA_MASK     (0x40000000)
#define ALTERA_PLATFORM                 (0)
#define XILINX_PLATFORM                 (1)

//PS config
#define REG_PSCONF_BUSY                 0x0C0 //ps_write_enable
#define REG_PSCONF_ENABLE               0x0C1 //ps_write_cpu
#define REG_PSCONF_ADDR                 0x0C2 //ps_write_addr_cpu
#define REG_PSCONF_DATA                 0x0C3 //ps_write_data_cpu
#define REG_PSCONF_DONE                 0x0C4 //config_done_reg

wvErrCode PSConf_FWConfigGeneralFPGA32Bit( U8 *u8PucData, U32 nDataLen, U32 nPlatform);

#endif/* PSCONF_H_ */
