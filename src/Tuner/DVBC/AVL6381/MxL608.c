
#include "MxL608.h"
#include "stdio.h"
#include <string.h>

AVL_uchar devId_608 = 0;

#define LogPrint(...) printf(__VA_ARGS__)

AVL6381_ErrorCode MxL608_Initialize(AVL_uint32 port, AVL_Tuner *pTuner)
{
   // AVL6381_ErrorCode r = AVL6381_EC_OK;
    MXL608_STATUS status;
    MXL608_BOOL singleSupply_3_3V;
    MXL608_XTAL_SET_CFG_T xtalCfg;
    MXL608_IF_OUT_CFG_T ifOutCfg;
    MXL608_AGC_CFG_T agcCfg;

    devId_608 = (AVL_uchar)pTuner->m_uiSlaveAddress;

    //Step 1 : Soft Reset MxL608
    status = MxLWare608_API_CfgDevSoftReset(port, devId_608);
    if (status != MXL608_SUCCESS)
    {
        printf("Error! MxLWare608_API_CfgDevSoftReset\n");
        return AVL6381_EC_GENERAL_FAIL;
    }

    //Step 2 : Overwrite Default
    //singleSupply_3_3V = MXL608_DISABLE;
    singleSupply_3_3V = MXL608_ENABLE;//old
    status = MxLWare608_API_CfgDevOverwriteDefaults(port, devId_608, singleSupply_3_3V);
    if (status != MXL608_SUCCESS)
    {
        printf("Error! MxLWare608_API_CfgDevOverwriteDefaults\n");
        return AVL6381_EC_GENERAL_FAIL;
    }
    //Step 3 : XTAL Setting
    xtalCfg.xtalFreqSel = MXL608_XTAL_16MHz;
    //xtalCfg.xtalCap = 10;
    xtalCfg.xtalCap = 28;//old
    //xtalCfg.clkOutEnable = MXL608_ENABLE;
    xtalCfg.clkOutEnable = MXL608_DISABLE; //old
    xtalCfg.clkOutDiv = MXL608_DISABLE;
    xtalCfg.clkOutExt = MXL608_DISABLE;
    //xtalCfg.singleSupply_3_3V = MXL608_DISABLE;
    xtalCfg.singleSupply_3_3V = MXL608_ENABLE;//old
    xtalCfg.XtalSharingMode = MXL608_DISABLE;
    status = MxLWare608_API_CfgDevXtal(port, devId_608, xtalCfg);
    //MxLWare608_API_CfgTunerLoopThrough(devId_608, MXL608_DISABLE, NULL, NULL);//ÁéÃô¶ÈÌáÉý1dB
    if (status != MXL608_SUCCESS)
    {
        //printf("Error! MxLWare608_API_CfgDevXtal\n");    
        return AVL6381_EC_GENERAL_FAIL;
    }

    //Step 4 : IF Out setting
    //ifOutCfg.ifOutFreq = MXL608_IF_4_1MHz;
    ifOutCfg.ifOutFreq = MXL608_IF_5MHz;//MXL608_IF_36MHz;//old
    //ifOutCfg.ifInversion = MXL608_DISABLE;
    ifOutCfg.ifInversion = MXL608_ENABLE;//old
    ifOutCfg.gainLevel = 11;
    ifOutCfg.manualFreqSet = MXL608_DISABLE;
    //ifOutCfg.manualIFOutFreqInKHz = 0;
    ifOutCfg.manualIFOutFreqInKHz = 5000;//36000; //old
    status = MxLWare608_API_CfgTunerIFOutParam(port, devId_608, ifOutCfg);
    if (status != MXL608_SUCCESS)
    {
        //printf("Error! MxLWare608_API_CfgTunerIFOutParam\n");    
        return AVL6381_EC_GENERAL_FAIL;
    }
    
    //Step 5 : AGC Setting
    agcCfg.agcType = MXL608_AGC_EXTERNAL;//MXL608_AGC_SELF;//MXL608_AGC_EXTERNAL;
    agcCfg.setPoint = 66;
    agcCfg.agcPolarityInverstion = MXL608_DISABLE;
    status = MxLWare608_API_CfgTunerAGC(port, devId_608, agcCfg);
    if (status != MXL608_SUCCESS)
    {
        printf("Error! MxLWare608_API_CfgTunerAGC\n");
        return AVL6381_EC_GENERAL_FAIL;
    }
    {
        MXL608_VER_INFO_T st_mxlDevVerInfo;
        memset(&st_mxlDevVerInfo,0,sizeof(MXL608_VER_INFO_T));
        MxLWare608_API_ReqDevVersionInfo(port, devId_608,&st_mxlDevVerInfo);
    }
    LogPrint("[WAVTUN_Init]: MxL608_Initialize  success !!\r\n");
    return AVL6381_EC_OK;
}

AVL6381_ErrorCode MxL608_Lock(Avl6381_PortIndex_t ePortIndex, AVL_Tuner *pTuner)
{
    //AVL6381_ErrorCode r = AVL6381_EC_OK;
    MXL608_STATUS status; 
    MXL608_TUNER_MODE_CFG_T tunerModeCfg;
    MXL608_CHAN_TUNE_CFG_T chanTuneCfg;
    
    chanTuneCfg.freqInHz =  pTuner->m_uiFrequency_Hz;

    //Step 6 : Application Mode setting
    if( pTuner->m_ucDTVMode == AVL_DEMOD_MODE_DVBC )
    {
        tunerModeCfg.signalMode = MXL608_DIG_DVB_C;
    }
    else
    {
        tunerModeCfg.signalMode = MXL608_DIG_DVB_T_DTMB;
    }
    //tunerModeCfg.ifOutFreqinKHz = 4100;
    tunerModeCfg.ifOutFreqinKHz = 5000;//36000;//old
    tunerModeCfg.xtalFreqSel = MXL608_XTAL_16MHz;
    tunerModeCfg.ifOutGainLevel = 11;
    status = MxLWare608_API_CfgTunerMode(ePortIndex, devId_608, tunerModeCfg);
    if (status != MXL608_SUCCESS)
    {
        return AVL6381_EC_GENERAL_FAIL;
        printf("Error! MxLWare608_API_CfgTunerMode\n");    
    }

    chanTuneCfg.freqInHz = pTuner->m_uiFrequency_Hz;        //Unit:Hz
    chanTuneCfg.bandWidth = pTuner->uiBandwidthHz;;
    chanTuneCfg.signalMode = MXL608_DIG_DVB_T_DTMB;
    if( pTuner->m_ucDTVMode == AVL_DEMOD_MODE_DTMB )
    {
        chanTuneCfg.signalMode = MXL608_DIG_DVB_T_DTMB;

        if( 6 == pTuner->uiBandwidthHz )
        {
            chanTuneCfg.bandWidth = MXL608_TERR_BW_6MHz; 
        }
        else if( 7 == pTuner->uiBandwidthHz )
        {
            chanTuneCfg.bandWidth = MXL608_TERR_BW_7MHz;
        }
        else
        {
            chanTuneCfg.bandWidth = MXL608_TERR_BW_8MHz;
        }
    }
    else if( pTuner->m_ucDTVMode == AVL_DEMOD_MODE_DVBC )
    {
        chanTuneCfg.signalMode = MXL608_DIG_DVB_C;
        chanTuneCfg.bandWidth = MXL608_CABLE_BW_8MHz;
    }
    //chanTuneCfg.bandWidth = MXL608_TERR_BW_6MHz;//new
    chanTuneCfg.xtalFreqSel = MXL608_XTAL_16MHz;
    chanTuneCfg.startTune = MXL608_START_TUNE;
    status = MxLWare608_API_CfgTunerChanTune(ePortIndex, devId_608, chanTuneCfg);
    if( status != MXL608_SUCCESS )
    {
        return AVL6381_EC_GENERAL_FAIL;
        printf("Error! MxLWare608_API_CfgTunerChanTune\n");    
    }
    MxLWare608_OEM_Sleep(15);//new
    
    return AVL6381_EC_OK;
}

AVL6381_ErrorCode MxL608_GetStatus(AVL_uint32 Port, AVL_Tuner *pTuner)
{
    //AVL6381_ErrorCode r = AVL6381_EC_OK;
    MXL608_STATUS status; 
    MXL608_BOOL refLockPtr = MXL608_UNLOCKED;
    MXL608_BOOL rfLockPtr = MXL608_UNLOCKED;        

    status = MxLWare608_API_ReqTunerLockStatus(Port, devId_608, &rfLockPtr, &refLockPtr);
    if( status == MXL608_TRUE )
    {
        if( MXL608_LOCKED == rfLockPtr && MXL608_LOCKED == refLockPtr )
        {
            pTuner->enumLockStatus = AVL6381_TUNER_LOCKED;
            return AVL6381_EC_OK;
        }
        else
        {
            pTuner->enumLockStatus = AVL6381_TUNER_UNLOCK;
            return AVL6381_EC_GENERAL_FAIL;
        }
    }
    return AVL6381_EC_GENERAL_FAIL;
}

AVL6381_ErrorCode MxL608_GetRFStrength(AVL_uint32 Port, AVL_Tuner *pTuner, AVL_int32 *power)
{
    MXL608_STATUS status; 
    AVL_int16 RSSI = 0;

    status = MxLWare608_API_ReqTunerRxPower(Port, devId_608, &RSSI);
    *power = (AVL_int32)RSSI/100;

    if( status != MXL608_SUCCESS )
    {
        return AVL6381_EC_GENERAL_FAIL;
    }

    return AVL6381_EC_OK;
}

