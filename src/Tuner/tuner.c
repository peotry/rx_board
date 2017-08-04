/*
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tuner.c
* Description : tuner moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-02-22
* Record    :
*
*/
#include <stdio.h>
#include <unistd.h>

#include "appGlobal.h"
#include "FPGA.h"
#include "Avl6381Manager.h"
#include "REG.h"
#include "Tuner.h"
#include "tuner_c.h"
#include "tuner_s2.h"
#include "WV_LOG.h"
#include "wv_err.h"
#include "wv_ci.h"
#include "PIN.h"
#include "LicenseParse.h"
#include "boardType.h"


/* tuner status */
//static Tuner_SrcStatus_t SourceStatus[DTV4U_TUNER_CHAN_NUM];

static BOOL b_IsLockFreqChange[DTV4U_TUNER_CHAN_NUM] = {FALSE, FALSE, FALSE, FALSE};


static LOCK_enum b_LockStatuts[DTV4U_TUNER_CHAN_NUM] = {UNLOCK, UNLOCK, UNLOCK, UNLOCK};

static LOCK_enum b_LastLockStatuts[DTV4U_TUNER_CHAN_NUM] = {UNLOCK, UNLOCK, UNLOCK, UNLOCK};


/* tuner type */
static TUNER_TYPE_enum u32TunerType = TUENR_TYPE_UNKNOW;

static char * strTunerType[] =
{
    "DVBC",
    "DVBS2",
    "DTMB"
};


TUNER_TYPE_enum Tuner_GetTunerType()
{
    return u32TunerType;
}

/*****************************************************************************
  Function:     Tuner_ParamInit
  Description:  param init
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
S32 Tuner_ParamInit()
{
   U32 u32TunerType = Tuner_GetTunerType();

    switch(u32TunerType)
    {  
        case TUNER_C:
        {
            Tuner_c_RestoreParam();
            break;
        }
        case TUNER_S2:
        {
            Tuner_s2_RestoreParam();
            Tuner_s2_SetParamAfterInit();
            break;
        }
        case TUNER_DTMB:
        {
            Tuner_dtmb_RestoreParam();
            break;
        }
        default:
        {
            break;
        }
    }
   
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Tuner_c_Default
  Description:  Default parameter of tuner c
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_Default(void)
{
    U32 u32TunerType = Tuner_GetTunerType();

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_PARAM,
        "******* Tuner_Default *******\r\n");
    
    switch(u32TunerType)
    {  
        case TUNER_C:
        {
            Tuner_c_Default();
            break;
        }
        case TUNER_S2:
        {
            Tuner_s2_Default();
            break;
        }
        case TUNER_DTMB:
        {
            Tuner_dtmb_Default();
            break;
        }
        default:
        {
            break;
        }
    }
   
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Tuner_IsLockFreqChange
  Description:  flag indicate lock freq change, need for rescan program
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
BOOL Tuner_IsLockFreqChange(U8 u8PortIndex)
{
    return b_IsLockFreqChange[u8PortIndex];
}


/*****************************************************************************
  Function:     Tuner_SetLockFreqChangeFlag
  Description:  set flag when lock freq change, need for rescan program
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Tuner_SetLockFreqChangeFlag(U8 u8PortIndex)
{
     b_IsLockFreqChange[u8PortIndex] = TRUE;
}


/*****************************************************************************
  Function:     Tuner_ClearLockFreqChangeFlag
  Description:  set flag when lock freq change, need for rescan program
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Tuner_ClearLockFreqChangeFlag(U8 u8PortIndex)
{
     b_IsLockFreqChange[u8PortIndex] = FALSE;
}


/*****************************************************************************
  Function:     Tuner_checkType
  Description:  check tuner type
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
TUNER_TYPE_enum Tuner_checkType(void)
{
    /*检测tuner类型*/
    U32 u32Type = 0x0;
    
    LicenseInfo_t *pstLicenseInfo = BMN_GetLicenseInfo_Handle();
    
    u32Type = FPGA_GetTunerType();
    switch(u32Type)
    {
        case TUNER_BOARD_C_OR_DTMB:
        {  
            if( pstLicenseInfo->SubboardID == CMP_RECEIVER_DTMB )
            {
                u32TunerType = TUNER_DTMB;
            }
            else
            {
                u32TunerType = TUNER_C;   
            }

            break;
        }
        case TUNER_BOARD_S2:
        {
            u32TunerType = TUNER_S2;
            break;
        }
        default:
            break;
    }
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TUNER, "Tuner_checkType  HW value:%d, type %s[0x%x] !\r\n"      
                                           , u32Type, strTunerType[u32TunerType], u32TunerType );

    return u32TunerType;
}


/*****************************************************************************
  Function:     Tuner_reset
  Description:  reset tuner 
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_reset(U8 u8PortIndex)
{
    if(u8PortIndex >= DTV4U_TUNER_CHAN_NUM)
    {
        return WV_ERR_PARAMS;
    }
    
    FPGA_SetTunerReset(u8PortIndex);

    return 0;
}


/*****************************************************************************
  Function:     Tuner_initial
  Description:  init tuner
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_initial(U8 u8PortIndex)
{   
    if(u8PortIndex >= DTV4U_TUNER_CHAN_NUM)
    {
        return WV_ERR_PARAMS;
    }

	u32TunerType = TUNER_DTMB;
    switch(u32TunerType)
    {  
        case TUNER_C:
        {
            Tuner_c_initial(u8PortIndex);
            break;
        }
        case TUNER_S2:
        {
            Tuner_s2_initial(u8PortIndex);
            break;
        }
        case TUNER_DTMB:
        {
            Tuner_dtmb_initial(u8PortIndex);
            break;
        }
        default:
        {
            break;
        }
    }
    
    return 0;
}


/*****************************************************************************
  Function:     Tuner_lockSignal
  Description:  lock signal
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_lockSignal(U8 u8PortIndex)
{
    if(u8PortIndex >= DTV4U_TUNER_CHAN_NUM)
    {
        return WV_ERR_PARAMS;
    }
    
    switch(u32TunerType)
    {  
        case TUNER_C:
        { 
            Tuner_c_lockSignal(u8PortIndex);
            break;
        }
        case TUNER_S2:
        {
            Tuner_reset(u8PortIndex);
            Tuner_initial(u8PortIndex);
            
            Tuner_s2_lockSignal(u8PortIndex);
            break;
        }
        case TUNER_DTMB:
        {
            Tuner_dtmb_lockSignal(u8PortIndex);
            break;
        }
        default:
        {
            break;
        }
    }
    
    return 0;
}


BOOL Tuner_IslockParamChange(U8 u8PortIndex)
{
    BOOL b_isLockParamChage = FALSE;
    
    switch(u32TunerType)
    {  
        case TUNER_C:
        { 
            b_isLockParamChage = Tuner_c_IsLockParamChange(u8PortIndex);
            break;
        }
        case TUNER_S2:
        {
            b_isLockParamChage = Tuner_IsS2LockParamChange(u8PortIndex);
            break;
        }
        case TUNER_DTMB:
        {
            b_isLockParamChage = Tuner_dtmb_IsLockParamChange(u8PortIndex);
            break;
        }
        default:
        {
            break;
        }
    }
    
    return b_isLockParamChage;
}


/*****************************************************************************
  Function:     Tuner_getStatus
  Description:  get tuner status
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_getStatus(U8 u8PortIndex, Tuner_SrcStatus_t *pstTunerStatus)
{
    if(u8PortIndex >= DTV4U_TUNER_CHAN_NUM)
    {
        return WV_ERR_PARAMS;
    }

    switch(u32TunerType)
    {
        case TUNER_C:
        {
            Tuner_c_getStatus(u8PortIndex, pstTunerStatus);
            break;
        }
        case TUNER_S2:
        {  
            Tuner_s2_getStatus(u8PortIndex, pstTunerStatus);
            break;
        }
        case TUNER_DTMB:
        {
            Tuner_dtmb_getStatus(u8PortIndex, pstTunerStatus);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}


/*****************************************************************************
  Function:     Tuner_checkChannelLock
  Description:  check  tuner  channle lock
  Input:        u8PortIndex  -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
BOOL Tuner_isChannelLock(U8 u8PortIndex)
{
    BOOL b_lock = FALSE;
    if(u8PortIndex >= DTV4U_TUNER_CHAN_NUM)
    {
        return WV_ERR_PARAMS;
    }

    switch(u32TunerType)
    {
        case TUNER_C:
        {
            Tuner_c_CheckLockStatus(u8PortIndex, &b_lock);
            break;
        }
        case TUNER_S2:
        {
            Tuner_s2_CheckLockStatus(u8PortIndex, &b_lock);
            break;
        }
        case TUNER_DTMB:
        {
            Tuner_dtmb_CheckLockStatus(u8PortIndex, &b_lock);
            break;
        }
        default:
        {
            break;
        }
    }

    return b_lock;
}


/*****************************************************************************
  Function:     Tuner_init
  Description:  init tuner, reset and dow fw
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Tuner_init()
{
    U8 u8TunerIdx = 0;
    
    for(u8TunerIdx = 0; u8TunerIdx < DTV4U_TUNER_CHAN_NUM; u8TunerIdx++)
    {
        Tuner_reset(u8TunerIdx);
        usleep(20);
    }

    for(u8TunerIdx = 0; u8TunerIdx < DTV4U_TUNER_CHAN_NUM; u8TunerIdx++)
    {
        Tuner_initial(u8TunerIdx);
    }

    Tuner_ParamInit();
}

/*****************************************************************************
  Function:     Tuner_process
  Description:  tuner process task
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_process(void)
{
    U8 u8TunerIdx = 0;
    BOOL bUnlockFlag = FALSE;
    
    while(1)
    {
        bUnlockFlag = FALSE;
        
        for(u8TunerIdx = 0; u8TunerIdx < DTV4U_TUNER_CHAN_NUM; u8TunerIdx++)
        {
            //参数变更，重新锁频
            if (Tuner_IslockParamChange(u8TunerIdx))
            {
                Tuner_lockSignal(u8TunerIdx);
                
                Tuner_SetLockFreqChangeFlag(u8TunerIdx);
            }

            /*检测tuner是否失锁*/
            if(Tuner_isChannelLock(u8TunerIdx))
            {
                b_LockStatuts[u8TunerIdx] = LOCK;
                
                if(b_LastLockStatuts[u8TunerIdx] != b_LockStatuts[u8TunerIdx])
                {
                    b_LastLockStatuts[u8TunerIdx] = b_LockStatuts[u8TunerIdx];
                    LOG_PRINTF(LOG_LEVEL_ALL, LOG_MODULE_TUNER, "tuner port %d lock\r\n", u8TunerIdx);
                }
            }
            else
            {
                bUnlockFlag = TRUE;
                
                b_LockStatuts[u8TunerIdx] = UNLOCK;
                
                if(b_LastLockStatuts[u8TunerIdx] != b_LockStatuts[u8TunerIdx])
                {
                    b_LastLockStatuts[u8TunerIdx] = b_LockStatuts[u8TunerIdx];
                    LOG_PRINTF(LOG_LEVEL_ALL, LOG_MODULE_TUNER, "tuner port %d unlock\r\n", u8TunerIdx);
                }

                Tuner_lockSignal(u8TunerIdx);
            }
        }

        if (bUnlockFlag)
        {
            PIN_LEDOrangeLight();
        }
        else
        {
            PIN_LEDGreenLight();
        }

        usleep(300*1000);
    }
}

