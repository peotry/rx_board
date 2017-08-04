/*
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tuner_s2.c
* Description : tuner moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-02-29
* Record    :
*
*/

#include <stdio.h>
#include <unistd.h>


#include "FPGA.h"
#include "Lnbh24.h"
#include "tuner_s2.h"

#include "Bspdatadef.h"

#include "tuner.h"
#include "Avl6211Manager.h"
#include "wv_err.h"
#include "WV_log.h"
#include "parameters.h"

/* tuner type */
static TUNER_TYPE_enum u32S2ChipType = AVL6211;


static Tune_S2_Params_t stS2Params[DTV4U_TUNER_CHAN_NUM];
static Tune_S2_Params_t stS2OldParams[DTV4U_TUNER_CHAN_NUM];



U32 WAVTUN_TUNEFREQ_ABS(U32 A, U32 B)
{
    if (A > B)
    {
        return A-B;
    }
    else
    {
        return B-A;
    }
}


void Tuner_s2_ParamInit()
{
    U32 i =0;
    
    for(i=0; i<DTV4U_TUNER_CHAN_NUM; i++)
    {
        stS2Params[i].u32FrequencyKHz = 3840;
        stS2Params[i].u32SymbolRate = 6875;
        stS2Params[i].u32LNBFrequencyKHz = 5150;
        stS2Params[i].u32LNBVoltage = 0;
        stS2Params[i].u32LNB22k = 0;
    }
}

/*****************************************************************************
  Function:     Tuner_s2_StoreParam
  Description:  Store parameter of tuner s2
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_s2_StoreParam(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    enErrCode = PARAM_WriteFile(FILE_NAME_TUNER_S2_CONFIG, (U8 *)stS2Params, sizeof(stS2Params));
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]PARAM_WriteFile error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Tuner_s2_SetParamAfterInit
  Description:  Set Param After Init
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_s2_SetParamAfterInit(void)
{
    U8 u8PortIndex = 0;

    for (u8PortIndex = 0; u8PortIndex < DTV4U_TUNER_CHAN_NUM; u8PortIndex++)
    {
        Tuner_s2_LNBSetting(u8PortIndex, stS2Params[u8PortIndex].u32LNBVoltage);
        Tuner_s2_SetLNB22KONOFF(u8PortIndex, stS2Params[u8PortIndex].u32LNB22k);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Tuner_s2_RestoreParam
  Description:  restore parameter of tuner s2
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_s2_RestoreParam(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8TryTimes = 0;
    U8 i = 0;

    memset(&stS2Params, 0, sizeof(stS2Params));
    memset(&stS2OldParams, 0, sizeof(stS2OldParams));
    
    do
    {
        enErrCode = PARAM_ReadFile(FILE_NAME_TUNER_S2_CONFIG, (U8 *)stS2Params, sizeof(stS2Params));
        if (WV_SUCCESS == enErrCode) 
        {
            memcpy(&stS2OldParams, &stS2Params, sizeof(stS2OldParams));
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
                "[%s:%d]Get s2 tuner configuration error, set to default\r\n",
                __FUNCTION__, __LINE__, u8TryTimes, enErrCode);    
    
        Tuner_s2_ParamInit();
        Tuner_s2_StoreParam();
    }

    for (i = 0; i < DTV4U_TUNER_CHAN_NUM; i++)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TUNER, 
            "S2_Tuner[%u]:Fre[%u],SymRate[%u],LNBFre[%u],LNBVoltage[%u],LNB22k[%u]\r\n",
            i,
            stS2Params[i].u32FrequencyKHz,
            stS2Params[i].u32SymbolRate,
            stS2Params[i].u32LNBFrequencyKHz,
            stS2Params[i].u32LNBVoltage,
            stS2Params[i].u32LNB22k);
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Tuner_s2_Default
  Description:  Default parameter of tuner s2
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_s2_Default(void)
{
    Tuner_s2_ParamInit();
    return Tuner_s2_StoreParam();
}

/*****************************************************************************
  Function:     Tuner_setLockFrequency
  Description:  tuner c init
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_s2_initial(U8 u8PortIndex)
{
    LOG_PRINTF(LOG_LEVEL_DEBUG,LOG_MODULE_TUNER, "DVB-S2 Init.");
    U32 ret;

    switch(u32S2ChipType)
    {
        case AVL6211:
        {
            ret = AVLTunerInit(u8PortIndex);
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
  Description:  tuner s2 lock signal
  Input:        u8PortIndex -- tuner port index
                *stLockPara -- parameter for lock
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_s2_lockSignal(U8 u8PortIndex)
{
    U32 ret = 0;

    switch(u32S2ChipType)
    {
        case AVL6211:
        {
            Avl6211_SearchParams_t stAvl6211LockPara;
            stAvl6211LockPara.dwFrequency =  stS2Params[u8PortIndex].u32FrequencyKHz;
            stAvl6211LockPara.dwSymbolRate = stS2Params[u8PortIndex].u32SymbolRate;
            stAvl6211LockPara.dwLNBFrequencyKHz = stS2Params[u8PortIndex].u32LNBFrequencyKHz;

            AVLTunerLockSignal(u8PortIndex, &stAvl6211LockPara);
            
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
  Function:     Tuner_s2_LNBSetting
  Description:  tuner s2 lock signal
  Input:        u32Port --  port index
                eLNBVoltage -- voltage 13/18/off
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
int  Tuner_s2_LNBSetting(U32 u32Port, LNB_Voltage_t eLNBVoltage)
{
   LNB_Polarization_t   ePolarization;
   
   if (eLNBVoltage == LNB_VOLTAGE_OFF)
   {
       ePolarization = NOPOLARIZATION;
   }
   else
   if (eLNBVoltage == LNB_VOLTAGE_13)
   {
       ePolarization = VERTICAL;
   }
   else
   if (eLNBVoltage == LNB_VOLTAGE_18)
   {
       ePolarization = HORIZONTAL;
   }
   else
   {
       return -1;
   }

   switch(u32S2ChipType)
    {
        case AVL6211:
        {
            LNBH24_SetPolarization(u32Port,  ePolarization);
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
  Function:     Tuner_s2_SetLNB22KONOFF
  Description:  tuner s2 lock signal
  Input:        u32Port --  port index
                LnbTone -- 22k / off
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
int Tuner_s2_SetLNB22KONOFF(U32 u32Port, U8 LnbTone)
{

    //LogPrint(LOG_TYPE_DEBUG, LOG_MODULE_TUNER, "[WAVTUN_SetLNB22KONOFF] (tuner:%d, lnb:%d) \r\n", pParams->uPortIndex, pParams->ucLNB22K));    
    switch(u32S2ChipType)
    {
        case AVL6211:
        {
            LNBH24_SetLnb(u32Port,  LnbTone);
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
  Function:     Tuner_c_getStatus
  Description:  get tuner status
  Input:        ePortIndex  -- tuner port index
                status -- status argument
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Tuner_s2_getStatus(Avl6211_PortIndex_t ePortIndex, Tuner_SrcStatus_t *status)
{
    if(NULL == status)
    {
        return WV_ERR_PARAMS;
    }

    switch(u32S2ChipType)
    {
        case AVL6211:
        {
   
            status->SourceType = 0;
            status->bLock = AVLTunerGetTunerStatus(ePortIndex);
            AVLTunerGetFECCodeRate(ePortIndex, &status->u8FECCodeRates);
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

            AVLTunerGetSignalQuality(ePortIndex, &status->s32C_N);
            AVLTunerGetSignalStrength(ePortIndex, &status->s32RFLevel);

            status->u32TunerFrequency = AVLTunerGetLockFrequency(ePortIndex);
            status->u32FrequencyOffset = AVLTunerGetTunFeqOffset(ePortIndex);
            AVLTunerGetBER_PER(ePortIndex, &status->uBerCnt, &status->uBerExponent);
            AVLTunerGetPER(ePortIndex, &status->packetErrorRate);

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
U32 Tuner_s2_CheckLockStatus(Avl6211_PortIndex_t ePortIndex, BOOL *b_isLock)
{
    if(NULL == b_isLock)
    {
        return WV_ERR_PARAMS;
    }

    switch(u32S2ChipType)
    {
        case AVL6211:
        {
            *b_isLock = AVLTunerGetTunerStatus(ePortIndex);
            break;
        }
        default:
        {
            break;
        }

    }
    return 0;
}



BOOL Tuner_IsS2LockParamChange(Avl6211_PortIndex_t ePortIndex)
{
    BOOL b_lockPramChange = FALSE;
    
    if ((stS2OldParams[ePortIndex].u32FrequencyKHz != stS2Params[ePortIndex].u32FrequencyKHz)
     || (stS2OldParams[ePortIndex].u32SymbolRate != stS2Params[ePortIndex].u32SymbolRate)
     || (stS2OldParams[ePortIndex].u32LNBFrequencyKHz != stS2Params[ePortIndex].u32LNBFrequencyKHz))
     {
        b_lockPramChange = TRUE;

         stS2OldParams[ePortIndex].u32FrequencyKHz  = stS2Params[ePortIndex].u32FrequencyKHz;
         stS2OldParams[ePortIndex].u32SymbolRate  = stS2Params[ePortIndex].u32SymbolRate;
         stS2OldParams[ePortIndex].u32LNBFrequencyKHz = stS2Params[ePortIndex].u32LNBFrequencyKHz;
     }

     return b_lockPramChange;
     
}


wvErrCode Tuner_s2GetParameter(Avl6211_PortIndex_t ePortIndex, Tune_S2_Params_t *s2_params)
{

    s2_params->u32FrequencyKHz = stS2Params[ePortIndex].u32FrequencyKHz;
    s2_params->u32SymbolRate = stS2Params[ePortIndex].u32SymbolRate;
    s2_params->u32LNBFrequencyKHz = stS2Params[ePortIndex].u32LNBFrequencyKHz;
    s2_params->u32LNBVoltage = stS2Params[ePortIndex].u32LNBVoltage;
    s2_params->u32LNB22k = stS2Params[ePortIndex].u32LNB22k;

    return WV_SUCCESS;
}




wvErrCode Tuner_s2SetParameter(Avl6211_PortIndex_t ePortIndex, Tune_S2_Params_t *s2_params)
{
    BOOL bLNBVoltageChanegFlag = FALSE;

    stS2Params[ePortIndex].u32FrequencyKHz = s2_params->u32FrequencyKHz;
    stS2Params[ePortIndex].u32SymbolRate = s2_params->u32SymbolRate;
    stS2Params[ePortIndex].u32LNBFrequencyKHz = s2_params->u32LNBFrequencyKHz;

    
    stS2Params[ePortIndex].u32LNBVoltage = s2_params->u32LNBVoltage;
    if (stS2OldParams[ePortIndex].u32LNBVoltage != stS2Params[ePortIndex].u32LNBVoltage)
    {
        stS2OldParams[ePortIndex].u32LNBVoltage = stS2Params[ePortIndex].u32LNBVoltage;
        Tuner_s2_LNBSetting(ePortIndex, stS2Params[ePortIndex].u32LNBVoltage);
        bLNBVoltageChanegFlag = TRUE;
    }

    stS2Params[ePortIndex].u32LNB22k = s2_params->u32LNB22k;
    if ((stS2OldParams[ePortIndex].u32LNB22k != stS2Params[ePortIndex].u32LNB22k)
        || bLNBVoltageChanegFlag)
    {
        stS2OldParams[ePortIndex].u32LNB22k = stS2Params[ePortIndex].u32LNB22k;
        Tuner_s2_SetLNB22KONOFF(ePortIndex, stS2Params[ePortIndex].u32LNB22k);
    }

    Tuner_s2_StoreParam();

    return WV_SUCCESS;
}



