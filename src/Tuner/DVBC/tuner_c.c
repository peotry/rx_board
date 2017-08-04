/*
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tuner_c.c
* Description : tuner moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-02-25
* Record    :
*
*/

#include <stdio.h>
#include <unistd.h>
#include "FPGA.h"
#include "Avl6381Manager.h"
#include "AVL6381_Internal.h"
#include "tuner_c.h"

#include "tuner.h"
#include "WV_log.h"
#include "wv_err.h"
#include "parameters.h"

/* tuner type */
static TUNER_TYPE_enum u32ChipType = AVL6381;

static Tuner_C_Params_t stCParams[DTV4U_TUNER_CHAN_NUM];
static Tuner_C_Params_t stCOldParams[DTV4U_TUNER_CHAN_NUM];


void Tuner_c_ParamInit()
{
    U32 i = 0;
     
     for(i=0; i<DTV4U_TUNER_CHAN_NUM; i++)
     {
        stCParams[i].u32FrequencyKHz = 208000;
        stCParams[i].u32SymbolRate = 6875;
        stCParams[i].u32QamMode = 2;
        stCParams[i].u32Mode = 0;
     }
}

/*****************************************************************************
  Function:     Tuner_c_StoreParam
  Description:  Tuner_c_StoreParam
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_c_StoreParam(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    enErrCode = PARAM_WriteFile(FILE_NAME_TUNER_C_CONFIG, (U8 *)stCParams, sizeof(stCParams));
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]PARAM_WriteFile error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Tuner_c_RestoreParam
  Description:  Tuner_c_RestoreParam
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_c_RestoreParam(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8TryTimes = 0;
    U8 i = 0;

    memset(&stCParams, 0, sizeof(stCParams));
    memset(&stCOldParams, 0, sizeof(stCOldParams));
    
    do
    {
        enErrCode = PARAM_ReadFile(FILE_NAME_TUNER_C_CONFIG, (U8 *)stCParams, sizeof(stCParams));
        if (WV_SUCCESS == enErrCode) 
        {
            memcpy(&stCOldParams, &stCParams, sizeof(stCOldParams));   
            break;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TUNER,
            "[%s:%d]PARAM_ReadFile error,u8TryTimes[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u8TryTimes, enErrCode);

        usleep(100000);
        
        u8TryTimes++;
        
    }while (u8TryTimes < 2);

    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TUNER,
            "[%s:%d]Get c tuner configuration error, set to default\r\n",
            __FUNCTION__, __LINE__, u8TryTimes, enErrCode);
    
        Tuner_c_ParamInit();
        Tuner_c_StoreParam();
    }

    for (i = 0; i < DTV4U_TUNER_CHAN_NUM; i++)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TUNER, 
            "C_Tuner[%u]:u32FrequencyKHz[%u],u32SymbolRate[%u],u32QamMode[%u],u32Mode[%u]\r\n",
            i,
            stCParams[i].u32FrequencyKHz,
            stCParams[i].u32SymbolRate,
            stCParams[i].u32QamMode,
            stCParams[i].u32Mode);
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
U32 Tuner_c_Default(void)
{
    Tuner_c_ParamInit();
    return Tuner_c_StoreParam();
}

/*****************************************************************************
  Function:     Tuner_setLockFrequency
  Description:  tuner c init
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_c_initial(U8 u8PortIndex)
{
    LOG_PRINTF(LOG_LEVEL_DEBUG,LOG_MODULE_TUNER, "DVBC Init.");
    U32 ret;

    switch(u32ChipType)
    {
        case AVL6381:
        {
            ret = AVL6381TunerInit(u8PortIndex, AVL_DEMOD_MODE_DVBC);
            break;
        }
        default:
        {
            break;
        }
    }

    return ret;
}


/*****************************************************************************
  Function:     Tuner_setLockFrequency
  Description:  tuner c lock signal
  Input:        u8PortIndex -- tuner port index
                *stLockPara -- parameter for lock
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_c_lockSignal(U8 u8PortIndex)
{
    U32 ret = 0;

    switch(u32ChipType)
    {
        case AVL6381:
        {
            Avl6381_SearchParams_t stAvl6381LockPara;
            stAvl6381LockPara.dwFrequencyKHz =  stCParams[u8PortIndex].u32FrequencyKHz;
            stAvl6381LockPara.dwSymbolRate =  stCParams[u8PortIndex].u32SymbolRate;

            ret = AVL6381TunerLockSignal(u8PortIndex, &stAvl6381LockPara);
            break;
        }
        default:
        {
            break;
        }
    }

    return ret;
}


/*****************************************************************************
  Function:     Tuner_c_getStatus
  Description:  get tuner status
  Input:        ePortIndex  -- tuner port index
                status -- status argument
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_c_getStatus(Avl6381_PortIndex_t ePortIndex, Tuner_SrcStatus_t *status)
{
    if(NULL == status)
    {
        return WV_ERR_PARAMS;
    }

    switch(u32ChipType)
    {
        case AVL6381:
        {
            status->SourceType = 0;     
            status->bLock = AVL6381TunerGetLockStatus(ePortIndex);
 
            AVL6381TunerGetFECCodeRate(ePortIndex, &status->u8FECCodeRates);
            status->u8IsMultiStream = 0;

            if(status->bLock)
            {
               FPGA_getTunerTotalBitRate(ePortIndex, &status->totalbitrate);      
               FPGA_getTunerEffectBitRate(ePortIndex, &status->effecbitrate);
            }
            else
            {
                status->totalbitrate = 0;
                status->effecbitrate = 0;
            }
           
            AVL6381TunerGetSignalQuality(ePortIndex, &status->s32C_N);
            AVL6381TunerGetSignalStrength(ePortIndex, &status->s32RFLevel);

            status->u32TunerFrequency = AVL6381TunerGetLockFrequency(ePortIndex);
            status->u32FrequencyOffset = AVL6381TunerGetTunFeqOffset(ePortIndex);
            AVL6381TunerGetBER(ePortIndex, &status->uBerCnt, &status->uBerExponent);
            AVL6381TunerGetPER(ePortIndex, &status->packetErrorRate);

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
  Function:     Tuner_c_CheckLockStatus
  Description:  check lock status
  Input:        ePortIndex  -- tuner port index
                status -- status argument
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_c_CheckLockStatus(Avl6381_PortIndex_t ePortIndex, BOOL *b_isLock)
{
    if(NULL == b_isLock)
    {
        return WV_ERR_PARAMS;
    }

    switch(u32ChipType)
    {
        case AVL6381:
        {
            *b_isLock = AVL6381TunerGetLockStatus(ePortIndex);
            break;
        }
        default:
        {
            break;
        }

    }
    return 0;
}


BOOL Tuner_c_IsLockParamChange(U8 ePortIndex)
{
    BOOL b_lockPramChange = FALSE;
    
    if ((stCOldParams[ePortIndex].u32FrequencyKHz != stCParams[ePortIndex].u32FrequencyKHz))
    {
        b_lockPramChange = TRUE;
        stCOldParams[ePortIndex].u32FrequencyKHz = stCParams[ePortIndex].u32FrequencyKHz;
    }

     return b_lockPramChange;
     
}


U32 Tuner_c_GetLockParam(U8 u8PortIndex, Tuner_C_Params_t *lockParam )
{
    if(u8PortIndex >= DTV4U_TUNER_CHAN_NUM)
    {
        return WV_ERR_PARAMS;
    }

    lockParam->u32FrequencyKHz = stCParams[u8PortIndex].u32FrequencyKHz;
    lockParam->u32SymbolRate   =  stCParams[u8PortIndex].u32SymbolRate;
    lockParam->u32QamMode    =  stCParams[u8PortIndex].u32QamMode;
    lockParam->u32Mode     = stCParams[u8PortIndex].u32Mode;

    return WV_SUCCESS;
}


U32 Tuner_c_SetLockParam(U8 u8PortIndex, Tuner_C_Params_t *lockParam )
{
    if(u8PortIndex >= DTV4U_TUNER_CHAN_NUM)
    {
        return WV_ERR_PARAMS;
    }

    stCParams[u8PortIndex].u32FrequencyKHz = lockParam->u32FrequencyKHz;
    stCParams[u8PortIndex].u32SymbolRate = lockParam->u32SymbolRate;
    stCParams[u8PortIndex].u32QamMode = lockParam->u32QamMode;
    stCParams[u8PortIndex].u32Mode = lockParam->u32Mode;

    Tuner_c_StoreParam();

    return WV_SUCCESS;
}

