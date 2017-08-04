/*
* Copyright (c) 2017,HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName REG.h
* Description : logic_CPU_reg moudle
* 	XXX The file no allowed to be cited by other dir file
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-20
* Record    :
*
*/

#ifndef REG_H_
#define REG_H_

#include "AppGlobal.h"

//TODO must change addr from system.hdf
#define AXI_CPU_LOGIC_INFO          0x83C00000


U32 Xil_In32(U32 Addr);
void Xil_Out32(U32 Addr, U32 Value);
/*****************************************************************************
  Function:     AXI_CPU_LOGIC_INF_mWriteReg
  Description:	Write a value to a AXI_CPU_LOGIC_INF register. A 32 bit write is performed.
				If the component is implemented in a smaller width, only the least
				significant data is written.
  Input:        BaseAddress is the base address of the AXI_CPU_LOGIC_INF device.
                RegOffset is the register offset from the base to write to.
                Data is the data written to the register
  Output:       none
  Return:       none
*****************************************************************************/
#define AXI_CPU_LOGIC_INF_mWriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (U32)(Data))

/*****************************************************************************
  Function:     AXI_CPU_LOGIC_INF_mReadReg
  Description:	Read a value from a AXI_CPU_LOGIC_INF register. A 32 bit read is performed.
				If the component is implemented in a smaller width, only the least
				significant data is read from the register. The most significant data
				will be read as 0.
  Input:        BaseAddress is the base address of the AXI_CPU_LOGIC_INF device.
                RegOffset is the register offset from the base to write to.
  Output:       none
  Return:       Data is the data from the register.
*****************************************************************************/
#define AXI_CPU_LOGIC_INF_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

void REG_Map();
void REG_Read(U32 u32Address, U32 *pu32Data);
void REG_Write(U32 u32Address, U32 u32Data);

void REG_Read_Without_Delay(U32 u32Address, U32 *pu32Data);
void REG_Write_Without_Delay(U32 u32Address, U32 u32Data);

#endif
