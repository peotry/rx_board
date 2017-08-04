/*****************************************************************************
* Copyright (c) 2007,WellAV Technologies Ltd. All rights reserved. 
* File Name£º   Avl6381Manager.c
* Description:
* History: 
               
<Author>    <Date>      <Version >   <Reason>
zhokun.zhang 2015/8/28     1.0       Create
*****************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "appGlobal.h"
#include "AVL6381_API.h"
#include "MxL608.h"
#include "Avl6381Manager.h"


//#define LogPrint(...) printf(__VA_ARGS__)

#define LogPrint(...)

#define TUNER_608

#ifdef TUNER_608
#define TUNER_608_MODE AVL_DEMOD_MODE_DVBC  //can set to DTMB or DVBC
#endif

#define DTMB_SYMBOL_RATE_6M   (5670) // KHZ
#define DTMB_SYMBOL_RATE_7M   (6615)
#define DTMB_SYMBOL_RATE_8M   (7560)



static AVL_uint32 s_dwLockFrequency[AVL6381_PORT_NUM] = {0};    //frequency aleady lock

#ifdef TUNER_608
static struct AVL_Tuner g_Tuner_608[AVL6381_PORT_NUM] = 
{
    {
        .m_uiSlaveAddress = 0x60,
        .m_uiI2C_BusClockSpeed_kHz = 200,
        .m_uiFrequency_Hz = 474*1000*1000,
        .uiIFHz = 5*1000*1000,
        .uiBandwidthHz = 8*1000*1000,
        .m_ucDTVMode = TUNER_608_MODE,

    },
    {
        .m_uiSlaveAddress = 0x60,
        .m_uiI2C_BusClockSpeed_kHz = 200,
        .m_uiFrequency_Hz = 474*1000*1000,
        .uiIFHz = 5*1000*1000,
        .uiBandwidthHz = 8*1000*1000,
        .m_ucDTVMode = TUNER_608_MODE,

    },
    {
        .m_uiSlaveAddress = 0x60,
        .m_uiI2C_BusClockSpeed_kHz = 200,
        .m_uiFrequency_Hz = 474*1000*1000,
        .uiIFHz = 5*1000*1000,
        .uiBandwidthHz = 8*1000*1000,
        .m_ucDTVMode = TUNER_608_MODE,

    },
    {
        .m_uiSlaveAddress = 0x60,
        .m_uiI2C_BusClockSpeed_kHz = 200,
        .m_uiFrequency_Hz = 474*1000*1000,
        .uiIFHz = 5*1000*1000,
        .uiBandwidthHz = 8*1000*1000,
        .m_ucDTVMode = TUNER_608_MODE,

    }
    
};
#endif

/* DEMOD I2C addr for four port */
AVL_uint16 DemodI2CAddr[AVL6381_PORT_NUM] = {0x14, 0x15, 0x14, 0x15}; //0x14 is 7bit address, change to 0x28 if 8bit is needed for I2C driver


static AVL6381_Para global_demod_config =
{
    .sStartupMode = TUNER_608_MODE,
    
#ifdef TUNER_608
    .sInputConfig = {
            .ueRefConfig = AVL6381_Xtal_30M,   //clock setting
            .ueDTMBInputPath = ADC_IF_I,
            .ueDVBCInputPath = ADC_IF_I,
            .uiDTMBIFFreqHz = 5*1000*1000,
            .uiDVBCIFFreqHz = 5*1000*1000,
            .uiDTMBSymbolRateHz = 7560*1000,
            .uiDVBCSymbolRateHz =6875*1000
         },
#endif    

    .sMPEGConfig = {
         .enumSerialParallel = AVL6381_MPEG_SERIAL,// parallel TS or serial TS config
         .enumRisingFalling = AVL6381_MPEG_RISING,// falling edge or rising edge TS sampling config
         .enumConti= AVL6381_MPEG_CONTINUOUS_DISABLE,//enable or disable continuous MPEG clock
    }
};


/*****************************************************************************
  Function:     PrintVersion
  Description:  print demod version
  Input:        ePortIndex
  Output:       
  Return:       
  Others:       
  Create:       2015/9/1
*****************************************************************************/ 
void PrintVersion(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_VerInfo stVerInfo;

    GetVersion_6381(&stVerInfo,ePortIndex);
    LogPrint("Firmware :%d--%d--%d\n",stVerInfo.m_Patch.m_Major,stVerInfo.m_Patch.m_Minor,stVerInfo.m_Patch.m_Build);
}

/*****************************************************************************
  Function:     AVL6381_GetDTMBSignalInfo
  Description:  Get DTMB signal infomation
  Input:        ePortIndex
  Output:       
  Return:       AVL6381_ErrorCode
  Others:       
  Create:       2015/9/1
*****************************************************************************/ 
AVL6381_ErrorCode AVL6381_GetDTMBSignalInfo(Avl6381_PortIndex_t ePortIndex,U8 *pCodeRate)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_DTMB_SignalInfo SignalInfoDTMB;

    AVL_uint32 uiModulationCode;
#if 1
    char *CarrierMode[2] = { "Multi", "Single" };
    char *ModulationDTMB[5] = { "4 QAM", "16 QAM", "32 QAM", "64 QAM", "4 QAM-NR" };
    char *FEC_Rate[3] = { "2/5", "3/5", "4/5" };
    char *Interleave[2] = { "240", "720"};
    char *HeaderMode[4] = { "Unknown", "PN420", "PN595", "PN945" };
#endif
    r = DTMB_GetSignalInfo_6381(&SignalInfoDTMB, ePortIndex);
    if(r != AVL6381_EC_OK)
    {
        LogPrint("Failed to DTMB signal info!\n");
        return r;
    }

    if((SignalInfoDTMB.m_Modulation == RX_DTMB_4QAM) && (SignalInfoDTMB.m_MappingMode == RX_NR_ON))
    {
        uiModulationCode = 4;
    }
    else
    {
        uiModulationCode = (AVL_uint32)SignalInfoDTMB.m_Modulation;
    }

    LogPrint("%s-Carrier, %s, FEC Rate %s, Interleave %s, Header Mode %s.\n\n",
    CarrierMode[SignalInfoDTMB.m_CarrierMode],
    ModulationDTMB[uiModulationCode],
    FEC_Rate[SignalInfoDTMB.m_CodeRate],
    Interleave[SignalInfoDTMB.m_InterleaveMode],
    HeaderMode[SignalInfoDTMB.m_HeaderMode]);

    *pCodeRate = SignalInfoDTMB.m_CodeRate;

    return r;
}

/*****************************************************************************
  Function:     AVL6381_GetDVBCSignalInfo
  Description:  Get DVBC signal infomation
  Input:        ePortIndex
  Output:       
  Return:       AVL6381_ErrorCode
  Others:       
  Create:       2015/9/1
*****************************************************************************/ 
static AVL6381_ErrorCode AVL6381_GetDVBCSignalInfo(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_DVBC_SignalInfo SignalInfoDVBC;

    char *ModulationDVBC[5] = { "16 QAM", "32 QAM", "64 QAM", "128 QAM", "256 QAM" };
    
    r = DVBC_GetSignalInfo_6381(&SignalInfoDVBC, ePortIndex);
    if (r != AVL6381_EC_OK)
    {
        LogPrint("Failed to get DVBC signal info!\n");
        return r;
    }
    LogPrint("%s.\n\n",ModulationDVBC[SignalInfoDVBC.m_Modulation]);

    return r;
}

/*****************************************************************************
  Function:     AVL6381TunerInit
  Description:  Init tuner and demod
  Input:        ePortIndex
  Output:       
  Return:       return_code
  Others:       
  Create:       2015/9/1
*****************************************************************************/ 
int  AVL6381TunerInit(Avl6381_PortIndex_t ePortIndex, unsigned char ucMode)
{
    int return_code = AVL6381_EC_OK;
    int ret = 0;

    /* init status params */
    s_dwLockFrequency[ePortIndex]  = 0;

    /* set tuner mode c or dtmb */
    global_demod_config.sStartupMode = ucMode;

    /* initialize demod and tuner */
    return_code = AVL6381_Initialize( DemodI2CAddr[ePortIndex], &global_demod_config, ePortIndex );
    
    if(return_code != AVL6381_EC_OK)
    {
        LogPrint("[AVL6381TunerInit] Failed to 1!\n");
        return return_code;
    }
    
    LogPrint("[AVL6381TunerInit] Booted OK mode %s!\n", (ucMode == AVL_DEMOD_MODE_DVBC)?"DVBC":"DTMB");
    
    PrintVersion(ePortIndex);
    AVL6381_I2CBypassOn(ePortIndex);


    /* If another tuner is used, please use the tuner initialization 
     * function to replace following part */
#ifdef TUNER_608
    g_Tuner_608[ePortIndex].m_ucDTVMode = ucMode; // Set mode c or dtmb.
    ret = MxL608_Initialize(ePortIndex, &g_Tuner_608[ePortIndex]);
	Tuner_dtmb_lockSignal(ePortIndex);
    LogPrint("[AVL6381TunerInit] MxL608_Initialize return ret:%d !!\r\n",ret);
#endif

    AVL6381_I2CBypassOff(ePortIndex);

    return AVL_SUCCESS;
}


/*****************************************************************************
  Function:     AVL6381TunerLockSignal
  Description:      Lock the signal
  Input:        
  Output:       
  Return:       0 - Locked;   1 - unLocked;   -1  - Error
  Others:       
  Create:       2015/9/1
*****************************************************************************/  
int  AVL6381TunerLockSignal(Avl6381_PortIndex_t ePortIndex, Avl6381_SearchParams_t *pSearchParams)
{
    AVL_uint16 times_out_cnt = 0;
    AVL_uint32 demod_lock_status = 0;
    AVL6381_ErrorCode r = AVL6381_EC_OK; 

    AVL_uint32 dtmbSymbolRate = 0;
    
    //lock tuner
    AVL6381_I2CBypassOn(ePortIndex);
   
#ifdef TUNER_608
    s_dwLockFrequency[ePortIndex] = pSearchParams->dwFrequencyKHz * 1000;
    g_Tuner_608[ePortIndex].m_uiFrequency_Hz = pSearchParams->dwFrequencyKHz  * 1000;
    g_Tuner_608[ePortIndex].enumLockStatus = AVL6381_TUNER_UNLOCK;
   // g_Tuner_608[ePortIndex].uiBandwidthHz = pSearchParams->ucBandWidthMode;

    g_Tuner_608[ePortIndex].uiBandwidthHz = 8; // 8M

    MxL608_Lock(ePortIndex, &g_Tuner_608[ePortIndex]);
    AVL6381_IBSP_Delay(15);
    MxL608_GetStatus(ePortIndex, &g_Tuner_608[ePortIndex]);
    
    LogPrint("[AVL6381TunerLockSignal]: MxL608_Initialize Status:%d ePortIndex:%d\r\n",g_Tuner_608[ePortIndex].enumLockStatus,ePortIndex);
    
    if(g_Tuner_608[ePortIndex].enumLockStatus == AVL6381_TUNER_UNLOCK)
    {
        LogPrint("Mxl608 tuner Lock fail:%d  khz.\n", g_Tuner_608[ePortIndex].m_uiFrequency_Hz/1000);
    }
    else
    {
        LogPrint("Mxl608 Tuner locked :%d  khz\n", g_Tuner_608[ePortIndex].m_uiFrequency_Hz/1000);
    }
    
#endif
    AVL6381_I2CBypassOff(ePortIndex);

    if( gsAVL6381Chip[ePortIndex].ucStartupMode == AVL_DEMOD_MODE_DVBC )
    {
       DVBC_SetSymbolRate_6381(pSearchParams->dwSymbolRate * 1000, ePortIndex);
    }
    else
    {
       if( pSearchParams->ucBandWidthMode == 6)
       {
           dtmbSymbolRate = DTMB_SYMBOL_RATE_6M;
       }
       else if( pSearchParams->ucBandWidthMode == 7)
       {
           dtmbSymbolRate = DTMB_SYMBOL_RATE_7M;
       }
       else
       {
           dtmbSymbolRate = DTMB_SYMBOL_RATE_8M;
       } 
       
       DTMB_SetSymbolRate_6381(dtmbSymbolRate * 1000, ePortIndex);
    }

    /*auto lock*/
    AVL6381_AutoLock(ePortIndex);

    // waiting for demod lock
    times_out_cnt = 0;
    demod_lock_status = 0;
    while(times_out_cnt < 300)//this time-out window can be customized, 300*10 = 3000ms for this time
    {
        //get the lock status
        r = AVL6381_GetLockStatus(&demod_lock_status, ePortIndex);
        if(demod_lock_status == 1)
        {
            break;//locked, no need to wait any more
        }
        
        AVL6381_IBSP_Delay(10);
        times_out_cnt++;
    }
    
    if(times_out_cnt >= 300)
    {
        r = AVL6381_EC_GENERAL_FAIL;
    }
    LogPrint("\n");

    return r;
}


/*****************************************************************************
  Function:         AVL6381TunerGetTunFeqOffset
  Description:      Get tune frequency offset
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/04
*****************************************************************************/  
int AVL6381TunerGetTunFeqOffset(Avl6381_PortIndex_t ePortIndex)
{
    AVL_int32 iRFOff;
    
    AVL6381_ErrorCode r = GetCarrierFreqOffset_6381(&iRFOff, ePortIndex);

    if(r == AVL6381_EC_OK)
        return iRFOff;
    else
        return 0;
}

/*****************************************************************************
  Function:         AVL6381TunerGetLockFrequency
  Description:      Get the real frequency of lock now.
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
U32 AVL6381TunerGetLockFrequency(Avl6381_PortIndex_t ePortIndex)
{
    AVL_uint32 u32Freq = 0;
    
    u32Freq = (s_dwLockFrequency[ePortIndex] + (AVL_uint32)AVL6381TunerGetTunFeqOffset(ePortIndex))/1000;
    
    LogPrint("==>[%s] u32Freq:%d.\r\n", __FUNCTION__, u32Freq);
    return u32Freq;
}

/*****************************************************************************
  Function:         AVL6381TunerGetFECCodeRate
  Description:      Get the Packet error rate
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
int  AVL6381TunerGetFECCodeRate(Avl6381_PortIndex_t ePortIndex, U8 *pCodeRate)
{
    AVL6381_ErrorCode uRet = AVL6381_EC_OK;
    
    uRet = AVL6381_GetDTMBSignalInfo(ePortIndex, pCodeRate);
    LogPrint("==>[%s] pCodeRate:%d.\r\n", __FUNCTION__, *pCodeRate);

    return ( (uRet == AVL6381_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         AVL6381TunerGetPER
  Description:      Get the Packet error rate
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
int  AVL6381TunerGetPER(Avl6381_PortIndex_t ePortIndex, float *fpuPER)
{
    AVL_uint32 per_1e9 = 0;
    //static AVL_uint32 count = 0 ;
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    
    //the returned value of per_1e9 is 1e9 times as actual PER
    r = AVL6381_GetPER(&per_1e9,ePortIndex);
    if(r != AVL6381_EC_OK)
    {
        LogPrint("PER ERR.\n");
        return AVL_ERROR;
    }
    *fpuPER = (float)((per_1e9)*1.0e-9); 

    return ( (r == AVL6381_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         GetBerValueExpo
  Description:      Get the bit error rate Value
  Input:            
  Output:           
  Return:           
  Others:       this funtion is copy from 0900 tuner.    
  Create:       2015/9/1
*****************************************************************************/  
static void GetBerValueExpo(U32 u32Ber, U32 *pu32Value, U32 *pu32Expo)
{
    U32 Cnt = 0; 
    U32 u32TmpBer = u32Ber;

    while (u32TmpBer > 9)
    {
        u32TmpBer = u32TmpBer/10;  
        Cnt++; 
    }

    *pu32Expo = (Cnt > 9) ? 9 : (9 - Cnt); 
    *pu32Value = (u32TmpBer == 0) ? 1 : u32TmpBer; 
}

/*****************************************************************************
  Function:         AVL6381TunerGetBER
  Description:      Get the bit error rate 
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
int  AVL6381TunerGetBER(Avl6381_PortIndex_t ePortIndex, U32 *pu32BerCnt, U32 *pu32Expo)
{
    AVL_uint32 per_1e9 = 0;

    AVL6381_ErrorCode r = AVL6381_EC_OK;
   
    //the returned value of per_1e9 is 1e9 times as actual PER        
    r = GetBER_6381(&per_1e9, FINAL_BER, ePortIndex);
    if(r != AVL6381_EC_OK)
    {
        LogPrint("BER ERR.\n");
        return AVL_ERROR;
    }
    
    GetBerValueExpo(per_1e9, pu32BerCnt, pu32Expo);
        
    return ( (r == AVL6381_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         AVL6381TunerGetSignalQuality
  Description:      Get the signal quality
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
int  AVL6381TunerGetSignalQuality(Avl6381_PortIndex_t ePortIndex, int  *pnSignalQuality)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    
    //the range for returned signal_quality is 0 to 100
    r = AVL6381_GetSQI(pnSignalQuality, ePortIndex);
    if(r != AVL6381_EC_OK)
    {
        LogPrint("SQI ERR.\n");
        return AVL_ERROR;
    }
    LogPrint("SQI is %d\n",*pnSignalQuality);

    return ( (r == AVL6381_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         AVL6381TunerGetSignalStrength
  Description:      Get the tuner signal strength
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
int  AVL6381TunerGetSignalStrength(Avl6381_PortIndex_t ePortIndex, int  *pnSignalStrength)
{
    AVL_uint32 signal_strength = 0;// range 0 - 65535
  
    // range 0 - 100
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    
    //the range for returned signal_strength is 0 - 65535
    r =  AVL6381_GetSSI(&signal_strength, ePortIndex);
    if(r != AVL6381_EC_OK)
    {
        LogPrint("SSI ERR.\n");
        return AVL_ERROR;
    }
    //uiSSI = signal_strength*100/65535;//scale to 0 - 100
    
    *pnSignalStrength = signal_strength;
    LogPrint("RF Level is %d\n",*pnSignalStrength);

    return ( (r == AVL6381_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         AVL6381TunerGetChipId
  Description:      Get the tuner chip id
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
int  AVL6381TunerGetChipId(Avl6381_PortIndex_t ePortIndex)
{
    AVL_uint32 uiChipID = 0;
    int return_code = AVL6381_EC_OK;

    
    return_code = AVL6381_GetChipID(ePortIndex, 0x14,&uiChipID);
    if(return_code!=AVL6381_EC_OK)
    {
        LogPrint( "Failed to get demod chip ID!\n");
        return return_code;
    }
    LogPrint( "111 Get chip id:0x%x\n",uiChipID);
    if(uiChipID == 0x6)// the chip ID of AVL6381 is 0x6
    {
        LogPrint( "Availink AVL6381 Demod!\n");
    }
    else
    {
        LogPrint( "Not Availink AVL6381 Demod!\n");
    }
    return return_code;
}

/*****************************************************************************
  Function:         AVL6381TunerGetLockStatus
  Description:      Get the tuner Status
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2015/9/1
*****************************************************************************/  
BOOL AVL6381TunerGetLockStatus(Avl6381_PortIndex_t ePortIndex)
{
    AVL_uchar u8TryTimes = 3;
    AVL_uint32 demod_lock_status = 0;
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    do
    {
        //This function should be called to check the lock status of the demod.
        r = AVL6381_GetLockStatus(&demod_lock_status, ePortIndex);
        
        if ((AVL6381_EC_OK == r) && (1 == demod_lock_status))
        {
            break;
        }

        AVL6381_IBSP_Delay(10); //Wait 10ms for demod to lock the channel.
    }while(--u8TryTimes);

    LogPrint("***************port %d, r= %d, demod lock status %d\r\n"
                , ePortIndex,r, demod_lock_status);
    
    return (1 == demod_lock_status) ? TRUE : FALSE;
}


