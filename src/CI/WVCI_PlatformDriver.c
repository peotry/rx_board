/**********************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WVCI_PlatformDriver.c
* Description : set of other moudles refer to REG moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-03-6
* Record    :
*
**********************************************************************/
#include "appGlobal.h"
#include "WV_CI.h"
#include "WVCI_PlatformDriver.h"
#include "FPGA.h"




#define     CI_DELAY_US_FOR_COMPATIBILITY       (500)

static int s_CAMLastStatusReg[MAX_CI_NUM] = {0x0};
static int s_CAMCurrStatusReg[MAX_CI_NUM] = {0x0};

volatile U32 s_CAMStatusCapture = 0x01; // when booting, let it check



/*****************************************************************************
  Function:     WVCI_GetINTEvent
  Description:  get insert or pull out event
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_GetINTEvent(void)
{
    int i;
    U32 u32Value = 0;

    FPGA_checkCiRegister(&u32Value);

    for(i=0; i<MAX_CI_NUM;i++)
    {
        s_CAMCurrStatusReg[i] = (u32Value >> i) & 0x01;

        if (s_CAMLastStatusReg[i] != s_CAMCurrStatusReg[i]) 
        {
            s_CAMStatusCapture |= 0x01;
            s_CAMLastStatusReg[i] = s_CAMCurrStatusReg[i];
        }
    }
    return s_CAMStatusCapture;
}


/*****************************************************************************
  Function:     WVCI_ClearINTEvent
  Description:  clear pull out/insert event
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void WVCI_ClearINTEvent(void)
{
    s_CAMStatusCapture = 0;
}


/*****************************************************************************
  Function:     WVCI_SYSGetCAMRegister
  Description:  check CAM insert status
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_SYSGetCAMRegister(void)
{
    U32 u32RegStatus = 0;
    
    FPGA_checkCiRegister(&u32RegStatus);

    return u32RegStatus;
}


/*****************************************************************************
  Function:     WVCI_SYSIsModuleStillPlugged
  Description:  check CAM is still plugged
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
BOOL WVCI_SYSIsModuleStillPlugged(int nSlot)
{
    // VARS
    U32   u32RegValue = 0;

    if ( nSlot > WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[WVCI_SYSIsModuleStillPlugged] This is bad params!! nSlot: %d \r\n", nSlot);
        return FALSE;
    }
    
    FPGA_checkCiRegister(&u32RegValue);
    
    u32RegValue = (u32RegValue >> nSlot) & 0x01;
    
    if (u32RegValue == 0x01)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*****************************************************************************
  Function:     WVCI_SYSIORead
  Description:  CI IO read data
  Input:        u32Slot -- CAM slot (slot0 or slot1)
                u32Address -- addr for read
  Output:       none
  Return:       U8  -- data read for u32Address
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_SYSIORead(U32 u32Slot, U32 u32Address, U8 *u8Data)
{
    // VARS
    U32  readData = 0x00;
    
    if (u32Slot > WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[WVCI_SYSPowerOnModule] BAD PARAMS!! \r\n");
        return WV_ERR_PARAMS;
    }

    // 增加检查CAM是否还存在
    if (!WVCI_SYSIsModuleStillPlugged(u32Slot))
    {
         CI_LOG_ERR("[WVCI_SYSIORead] cam%d, is removed! \r\n", u32Slot);
         return ERROR_CI_CAM_REMOVED;
    }
    
    // each access add more delay, or some cams not stable when use mmi function.
    usleep(CI_DELAY_US_FOR_COMPATIBILITY*2);
    
    FPGA_setCiSelect(u32Slot);
    
    FPGA_CiReadCmd(IO_READ); 
    FPGA_CiWriteAddr(u32Address);
    FPGA_CiReadEnable();
    usleep(5);
    FPGA_CiRead(&readData);
    FPGA_CiReadDisable();

    FPGA_clearCiSelect(u32Slot);

    *u8Data = readData & 0xFF;

    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     WVCI_SYSIOWrite
  Description:  CI IO write data
  Input:        u32Slot -- CAM slot (slot0 or slot1)
                u32Address -- addr for write
                data -- date for write
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_SYSIOWrite(U32 u32Slot, U32 u32Address, U8 u8Data)
{
    // CHECK PARAMS
    if (u32Slot > WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[WVCI_SYSPowerOnModule] BAD PARAMS!! \r\n");
        return WV_ERR_PARAMS;
    }

    // 增加检查CAM是否还存在
    if (!WVCI_SYSIsModuleStillPlugged(u32Slot))
    {
        CI_LOG_DEBG("[WVCI_SYSIOWrite] cam%d, is removed! \r\n", u32Slot);
        return ERROR_CI_CAM_REMOVED;
    }

    //  each access add more delay, or some cams not stable when use mmi function.
    usleep(CI_DELAY_US_FOR_COMPATIBILITY);
 
    FPGA_setCiSelect(u32Slot);
 
    FPGA_CiWrite(u32Address, u8Data);   
    FPGA_CiWriteEnable(IO_WRITE);
    usleep(5);
    FPGA_CiWriteDisable(); 
   
    FPGA_clearCiSelect(u32Slot);

    return WVCI_SUCCESS;
}


/*****************************************************************************
  Function:     WVCI_SYSMEMRead
  Description:  CI MEM read data
  Input:        u32Slot -- CAM slot (slot0 or slot1)
                u32Address -- addr for write
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U8 WVCI_SYSMEMRead(U32 u32Slot, U32 u32Address)
{
    // VARS
    U32      realData = 0x00;
    
    // CHECK PARAMS
    if (u32Slot > WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[WVCI_SYSPowerOnModule] BAD PARAMS!! \r\n");
        return WV_ERR_PARAMS;
    }

    // 增加检查CAM是否还存在
    if (!WVCI_SYSIsModuleStillPlugged(u32Slot))
    {
        CI_LOG_DEBG("[WVCI_SYSMEMRead] cam%d, is removed! \r\n", u32Slot);
        return 0xFF;
    }

  
    FPGA_setCiSelect(u32Slot);
 
    FPGA_CiReadCmd(MEM_READ);
    FPGA_CiWriteAddr(u32Address);
    FPGA_CiReadEnable();
    usleep(5);
    FPGA_CiRead(&realData);
    FPGA_CiReadDisable();
   
    FPGA_clearCiSelect(u32Slot);
   
    return (realData&0xFF);
}


/*****************************************************************************
  Function:     WVCI_SYSMEMRead
  Description:  CI MEM write data
  Input:        u32Slot -- CAM slot (slot0 or slot1)
                u32Address -- addr for write
                data -- date for write
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_SYSMEMWrite(U32 u32Slot, U32 u32Address, U8 u8Data)
{
    // CHECK PARAMS
    if (u32Slot > WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[WVCI_SYSPowerOnModule] BAD PARAMS!! \r\n");
        return WV_ERR_PARAMS;
    }

    // 增加检查CAM是否还存在
    if (!WVCI_SYSIsModuleStillPlugged(u32Slot))
    {
        CI_LOG_DEBG("[WVCI_SYSMEMWrite] cam%d, is removed! \r\n", u32Slot);
        return ERROR_CI_CAM_REMOVED;
    }
    
    FPGA_setCiSelect(u32Slot);

    FPGA_CiWrite(u32Address, u8Data);   
    FPGA_CiWriteEnable(MEM_WRITE);
    usleep(5);
    FPGA_CiWriteDisable();  

    FPGA_clearCiSelect(u32Slot);
 

    return WVCI_SUCCESS;
}


/*****************************************************************************
  Function:     WVCI_SYSPowerOnModule
  Description:  power on ci modules
  Input:        u32Slot   -- CAM slot (slot0 or slot1)
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_SYSPowerOnModule(U32 u32Slot)
{
    if (u32Slot > WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[WVCI_SYSPowerOnModule] BAD PARAMS!! \r\n");
        return WVCI_ERROR_PARAMS;
    }
      
    FPGA_setCiPower(u32Slot, CI_POW_ON);
    
    CI_LOG_DEBG("[WVCI_SYSPowerOnModule] CAM%d Power on... \r\n", u32Slot);

    return WVCI_SUCCESS;
}


/*****************************************************************************
  Function:     WVCI_SYSPowerOffModule
  Description:  power off ci modules
  Input:        u32Slot   -- CAM slot (slot0 or slot1)
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_SYSPowerOffModule(U32 u32Slot)
{
    if (u32Slot > WVCI_MAX_SLOT_NUM)
    {
         CI_LOG_ERR("[WVCI_SYSPowerOffModule] BAD PARAMS!! \r\n");
        return WVCI_ERROR_PARAMS;
    }  

    FPGA_setCiPower(u32Slot, CI_POW_OFF);  
    
    CI_LOG_DEBG("[WVCI_SYSPowerOffModule] CAM%d Power off... \r\n", u32Slot);  

   
   return WVCI_SUCCESS;
}


/*****************************************************************************
  Function:     WVCI_SYSResetModule
  Description:  reset ci modules
  Input:        u32Slot   -- CAM slot (slot0 or slot1)
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 WVCI_SYSResetModule(U32 u32Slot)
{

    if (u32Slot > WVCI_MAX_SLOT_NUM)
    {
        return WVCI_ERROR_PARAMS;
    }

    FPGA_CiReset(u32Slot);
        
    CI_LOG_DEBG("[WVCI_SYSResetModule] RESET CAM%d... \r\n", u32Slot);
    
    return WVCI_SUCCESS;
}


