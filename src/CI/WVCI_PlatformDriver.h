/**********************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WVCI_PlatformDriver.h
* Description : set of other moudles refer to REG moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-03-6
* Record    :
*
**********************************************************************/

#ifndef __WVCI_PLATFORM_DRIVER_H_
#define __WVCI_PLATFORM_DRIVER_H_

#include "WV_log.h"


/*所有CI的log打印采用宏定义替换，增强可移植性*/

#define CI_LOG_INF(...)    log_printf(LOG_LEVEL_ALL, LOG_MODULE_CI,__VA_ARGS__)
#define CI_LOG_ERR(...)    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_CI,__VA_ARGS__)
#define CI_LOG_DEBG(...)   log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_CI,__VA_ARGS__)
#define CI_LOG_WARR(...)   log_printf(LOG_LEVEL_ALARM, LOG_MODULE_CI,__VA_ARGS__)


typedef enum
{
    CI_SLOT0,
    CI_SLOT1,
    CI_SLOT2,
    CI_SLOT3,

    MAX_CI_NUM
}CI_SLOT;


U32 WVCI_GetINTEvent(void);
void WVCI_ClearINTEvent(void);
U32 WVCI_SYSGetCAMRegister(void);
BOOL WVCI_SYSIsModuleStillPlugged(int nSlot);
U32 WVCI_SYSIORead(U32 u32Slot, U32 u32Address, U8 *u8Data);
U32 WVCI_SYSIOWrite(U32 u32Slot, U32 u32Address, U8 u8Data);
U8 WVCI_SYSMEMRead(U32 u32Slot, U32 u32Address);
U32 WVCI_SYSMEMWrite(U32 u32Slot, U32 u32Address, U8 u8Data);
U32 WVCI_SYSPowerOnModule(U32 u32Slot);
U32 WVCI_SYSPowerOffModule(U32 u32Slot);
U32 WVCI_SYSResetModule(U32 u32Slot);

#endif // __WVCI_PLATFORM_DRIVER_H_
