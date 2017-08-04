/*
 *           Copyright 2007-2015 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */

#include <stdio.h>
#include "AVL6381_API.h"
#include "AVL6381Manager.h"

#define LogPrint(...) printf( __VA_ARGS__)


static struct Signal_Level  DTMBSignalLevel [47]=
{
    {4525,  -92},
    {5217,  -90},
    {6147,  -88},
    {7194,  -86},
    {8377,  -84},
    {9824,  -82},
    {10893, -80},
    {12273, -78},
    {13567, -76},
    {14759, -74},
    {15898, -72},
    {17010, -70},
    {18068, -68},
    {18973, -66},
    {19863, -64},
    {21583, -62},
    {22930, -60},
    {24185, -58},
    {25256, -56},
    {26087, -54},
    {26966, -52},
    {27932, -50},
    {28936, -48},
    {29922, -46},
    {31028, -44},
    {32234, -42},
    {33282, -40},
    {34503, -38},
    {35715, -36},
    {37717, -34},
    {39126, -32},
    {40157, -30},
    {41059, -28},
    {41720, -26},
    {42756, -24},
    {43020, -22},
    {43696, -20},
    {44293, -18},
    {45051, -16},
    {46077, -14},
    {47504, -12},
    {48922, -10},
    {50182, -8},
    {51380, -6},
    {52613, -4},
    {53438, -2},
    {53799, -1}
};

AVL6381_ErrorCode AVL6381_Initialize(AVL_uint16 DemodAddr, AVL6381_Para *Para, Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTimeoutCount = 20;
    AVL_uint32 uiDelayUnit = 10; /* Time-out window 10*20 ms */
    AVL_uint32 uiCounter = 0;
    AVL_uchar ucPLLIndex = 0;
    AVL_uint32 uiChipID = 0;

    gAVL6381freezeflag[ePortIndex] = 0;
    
    r = AVL6381_GetChipID(ePortIndex, DemodAddr, &uiChipID);
    
    LogPrint("[AVL6381_Initialize]  r:%d DemodI2CAddr:0x%x uiChipID:0x%x\n", r, DemodAddr, uiChipID);
    
    if(uiChipID == 0xe)
    {
        gAVL6381freezeflag[ePortIndex] = 1;
    }
    gsAVL6381Chip[ePortIndex].m_SlaveAddr = DemodAddr;
    gsAVL6381Chip[ePortIndex].ucStartupMode = Para->sStartupMode;
    gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig = Para->sInputConfig.ueRefConfig;
    gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBIFFreqHz = Para->sInputConfig.uiDTMBIFFreqHz;
    gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBSymbolRateHz = Para->sInputConfig.uiDTMBSymbolRateHz;
    gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDTMBInputPath = Para->sInputConfig.ueDTMBInputPath;
    gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDVBCIFFreqHz = Para->sInputConfig.uiDVBCIFFreqHz;
    gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDVBCSymbolRateHz = Para->sInputConfig.uiDVBCSymbolRateHz;
    gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDVBCInputPath = Para->sInputConfig.ueDVBCInputPath;
    gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumRisingFalling = Para->sMPEGConfig.enumRisingFalling;
    gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel = Para->sMPEGConfig.enumSerialParallel;
    gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumConti = Para->sMPEGConfig.enumConti;
    
    gsAVL6381Chip[ePortIndex].m_current_demod_mode = Para->sStartupMode;
    
    r = InitSemaphore_6381(ePortIndex);

    if(Para->sStartupMode == AVL_DEMOD_MODE_DTMB)
    {
        ucPLLIndex = Para->sInputConfig.ueRefConfig * 2;
    }
    else
    {
        ucPLLIndex = Para->sInputConfig.ueRefConfig * 2 + 1;
    }

    r |= IBase_Initialize_6381(&(gAVL6381PLLConfig[ePortIndex][ucPLLIndex]),ePortIndex);
    
    AVL6381_IBSP_Delay(uiDelayUnit);
    for(uiCounter = 0; uiCounter < uiTimeoutCount; uiCounter++)
    {
        r = CheckChipReady_6381 (ePortIndex);
        if(r == AVL6381_EC_OK)
        {
            break;
        }
        AVL6381_IBSP_Delay(uiDelayUnit);
    }

    /* Receiver Init */
    r |= IRx_Initialize_6381(ePortIndex);
    
    r |= DTMB_SetSymbolRate_6381(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBSymbolRateHz,ePortIndex);
    
    r |= SetMpegMode_6381(ePortIndex);
    r |= SetMpegSerialPin_6381(geAVL6381SerialPin[ePortIndex],ePortIndex);
    r |= SetMpegSerialOrder_6381(geAVL6381SerialOrder[ePortIndex],ePortIndex);
    r |= SetMpegSerialSyncPulse_6381(geAVL6381SerialSync[ePortIndex],ePortIndex);
    r |= SetMpegErrorBit_6381(geAVL6381ErrorBit[ePortIndex],ePortIndex);
    r |= SetMpegErrorPola_6381(geAVL6381ErrorPola[ePortIndex],ePortIndex);
    r |= SetMpegValidPola_6381(geAVL6381ValidPola[ePortIndex],ePortIndex);
    r |= SetMpegPacketLen_6381(geAVL6381PacketLen[ePortIndex],ePortIndex);
    
    if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumConti == AVL6381_MPEG_CONTINUOUS_ENABLE)
    {
        if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel == AVL6381_MPEG_SERIAL)
        {
            if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DTMB_EnableMpegContinuousMode_6381(guiDTMBSerialMPEGContinuousHz[ePortIndex],ePortIndex);
            }
            else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DVBC_EnableMpegContinuousMode_6381(guiDVBCSerialMPEGContinuousHz[ePortIndex],ePortIndex);
            }
        }
        else
        {
            if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DTMB_EnableMpegContinuousMode_6381(guiParallelMPEGContinuousHz[ePortIndex],ePortIndex);
            }
            else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DVBC_EnableMpegContinuousMode_6381(guiParallelMPEGContinuousHz[ePortIndex],ePortIndex);
            }
        }
    }
    
    if(gucDisableMPEGOutput[ePortIndex] == 0)
    {
        r |= EnableMpegOutput_6381(ePortIndex);
    }

    /* Reset tuner I2C module */
    r |= TunerI2C_Initialize_6381(ePortIndex);

    /* Set AGC */
    r |= SetAGCPola_6381(geAVL6381AGCPola[ePortIndex],ePortIndex);
    if(gucDisableAGC[ePortIndex] == 0)
    {
        r |= EnableAGC_6381(ePortIndex);
    }

    /* Error statistics Setting */
    r |= InitErrorStat_6381(ePortIndex);

    return r;
}

AVL6381_ErrorCode AVL6381_AutoLock(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL6381_RunningLevel enumRunningLevel = AVL6381_RL_NORMAL;
    AVL_uint32  uiCnt = 0;

    r = Halt_6381(ePortIndex);
    if(r != AVL6381_EC_OK)
    {
        return(r);
    }

    while((enumRunningLevel != AVL6381_RL_HALT) && (uiCnt<10))
    {
        r = GetRunningLevel_6381(&enumRunningLevel, ePortIndex);
        if(r != AVL6381_EC_OK)
        {
            return(r);
        }
        AVL6381_IBSP_Delay(10);
        uiCnt++;
    }

    if(enumRunningLevel != AVL6381_RL_HALT)
    {
        return AVL6381_EC_RUNNING;
    }

    r |= AutoLockChannel_6381(ePortIndex);

    return r;
}

AVL6381_ErrorCode AVL6381_DTMBManualLock(AVL_DTMB_ManualLockInfo *pstDTMBManualLockInfo, Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if((pstDTMBManualLockInfo->m_Modulation == RX_DTMB_32QAM) ||
        ((pstDTMBManualLockInfo->m_Modulation == RX_DTMB_4QAM) && (pstDTMBManualLockInfo->m_MappingMode == RX_NR_ON)))
    {
        if(pstDTMBManualLockInfo->m_CodeRate != RX_FEC_4_5)
        {
            r |= AVL6381_EC_GENERAL_FAIL;
            return r;
        }
    }

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_get_config_from_tps_caddr, 0);//get channel info from preset parameters
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_fixed_header_caddr, 1);

    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_pn_chk_en_caddr, 1);
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_tps_manual_pn_caddr, pstDTMBManualLockInfo->m_HeaderMode);

    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_interleaver_mode_caddr, pstDTMBManualLockInfo->m_InterleaveMode);//interleaver mode
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_carrier_mode_caddr, pstDTMBManualLockInfo->m_CarrierMode);//carrier mode, single or multi-carrier
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_fec_rate_caddr, pstDTMBManualLockInfo->m_CodeRate);//code rate
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_nr_mode_caddr, pstDTMBManualLockInfo->m_MappingMode);//NR mapping mode
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_constellation_mode_caddr, pstDTMBManualLockInfo->m_Modulation);//constellation

    r |= DTMB_SetSpectrumPola_6381(pstDTMBManualLockInfo->m_Spectrum,ePortIndex);

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_demod_lock_status_iaddr, 0);//set the unlock status, if signal locked, the firmware will set it to 1
    
    r |= SendRxOP_6381(OP_RX_ACQUIRE, ePortIndex);

    return r;

}

AVL6381_ErrorCode AVL6381_DVBCManualLock(AVL_uint32 uiSymbolRateHz, AVL_DVBC_QAMMode enumQAMMode, Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_symbol_rate_Hz_iaddr, uiSymbolRateHz);
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_qam_mode_iaddr, enumQAMMode);

    r |= SendRxOP_6381(OP_RX_ACQUIRE, ePortIndex);

    return r;
}

AVL6381_ErrorCode AVL6381_GetLockStatus(AVL_puint32 puiLockStatus, Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    *puiLockStatus = 0;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_GetLockStatus_6381(puiLockStatus, ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_GetLockStatus_6381(puiLockStatus, ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode AVL6381_NoSignalDetection(AVL_uint32 *puiNoSignal, Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_NoSignalDetection_6381(puiNoSignal,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_NoSignalDetection_6381(puiNoSignal,ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode AVL6381_I2CBypassOn(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl, 
        BIT_REPEATER_WAIT_ACK|BIT_REPEATER_SEL|BIT_REPEATER_ENABLE);
    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl,
        BIT_REPEATER_WAIT_ACK|BIT_REPEATER_SEL|BIT_REPEATER_ENABLE);
    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl,
        BIT_REPEATER_WAIT_ACK|BIT_REPEATER_SEL|BIT_REPEATER_ENABLE);

    usleep(2000000);
    return r;
}

AVL6381_ErrorCode AVL6381_I2CBypassOff(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;


    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl, 
        BIT_REPEATER_WAIT_ACK|BIT_REPEATER_SEL|!BIT_REPEATER_ENABLE);
    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl, 
        BIT_REPEATER_WAIT_ACK|BIT_REPEATER_SEL|!BIT_REPEATER_ENABLE);
    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl, 
        BIT_REPEATER_WAIT_ACK|BIT_REPEATER_SEL|!BIT_REPEATER_ENABLE);

    return r;
}

AVL6381_ErrorCode AVL6381_GetChipID(Avl6381_PortIndex_t ePortIndex, AVL_uint16 usI2CAddr, AVL_puint32 puiChipID)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 ChipIDRegAddr = hw_AVL6381_chip_id;
    AVL_uchar pucBuffAddr[3] = {0};
    AVL_uchar pucBuffData[4]= {0};
    AVL_uint16 usAddrSize = 3;
    AVL_uint16 usDataSize = 4;

    ChunkAddr_6381(ChipIDRegAddr, pucBuffAddr);
    r = AVL6381_IBSP_I2C_Write(ePortIndex, usI2CAddr, pucBuffAddr, &usAddrSize);  
    r |= AVL6381_IBSP_I2C_Read(ePortIndex, usI2CAddr, pucBuffData, &usDataSize);
    *puiChipID = DeChunk32_6381(pucBuffData);

    return r;
}

AVL6381_ErrorCode AVL6381_GetSNR(AVL_puint32 puiSNR_db_x100,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    
    *puiSNR_db_x100 = 0;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r |= DTMB_GetSNR_6381(puiSNR_db_x100, ePortIndex);
    } 
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r |= DVBC_GetSNR_6381(puiSNR_db_x100, ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode AVL6381_GetSSI(AVL_puint32 puiSignalStrength,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiRFSignalLevel;
    AVL_int16  uiRFSignalDBM;
    AVL_uint32 i;
    
    *puiSignalStrength = 0;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_GetSignalLevel_6381(&uiRFSignalLevel,ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_GetSignalLevel_6381(&uiRFSignalLevel,ePortIndex);
    }
    
    if (AVL6381_EC_OK != r)
    {
        LogPrint("\r\n Get SignalLevel --- Failed! Err = %d \r\n",r);
    }
    else
    {
        for(i=0; i<47; i++)
        {
            if(uiRFSignalLevel<= DTMBSignalLevel[i].SignalLevel)   
            {
                //Calculate the corresponding DBM value.
                if((0==i)&&(uiRFSignalLevel< DTMBSignalLevel[i].SignalLevel))
                {   

                    LogPrint("RFSignalLevel is too weak !\n");
                    uiRFSignalDBM = -94;
                }
                else
                {
                    if (i > 0){
                        uiRFSignalDBM = (uiRFSignalLevel - DTMBSignalLevel[i-1].SignalLevel)*
                            (DTMBSignalLevel[i].SignalDBM - DTMBSignalLevel[i-1].SignalDBM)/
                            (DTMBSignalLevel[i].SignalLevel - DTMBSignalLevel[i-1].SignalLevel);
                        uiRFSignalDBM += DTMBSignalLevel[i-1].SignalDBM;
                    }
                    else{
                        uiRFSignalDBM=DTMBSignalLevel[i].SignalDBM;
                    }
                }
                break;
            } 
        }
    }

    *puiSignalStrength=uiRFSignalDBM;

    return r;
}


AVL6381_ErrorCode AVL6381_GetSQI(AVL_puint32 puiSignalQuality,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiLockStatus = 0;
    
    *puiSignalQuality = 0;

    r = AVL6381_GetLockStatus(&uiLockStatus,ePortIndex);
    if( uiLockStatus == 0)
    {
        return r;
    }

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_GetSignalQuality_6381(puiSignalQuality, 4,ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_GetSignalQuality_6381(puiSignalQuality, 4,ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode AVL6381_SetMode(AVL_uchar mode,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTimeoutCount = 20;
    AVL_uint32 uiDelayUnit = 10; /* Time-out window 10*20 ms */
    AVL_uint32 uiCounter = 0;
    AVL_uchar ucPLLIndex = 0;
    AVL_uint32 uiMaxRetries = 10;
    AVL_uint32 uiIndex = 0;
    AVL_uchar r_mode;

    r = GetMode_6381(&r_mode, ePortIndex);
    if(r_mode == mode)
    {
        return AVL6381_EC_OK;
    }
    
    gsAVL6381Chip[ePortIndex].ucStartupMode = mode;
    gsAVL6381Chip[ePortIndex].m_current_demod_mode = mode;
    
    //r = InitSemaphore_6381();
    if(gAVL6381freezeflag[ePortIndex] == 1)
    {
        r |= Halt_6381(ePortIndex);
        if(AVL6381_EC_OK == r )
        {
            while (AVL6381_EC_OK != GetRxOP_Status_6381(ePortIndex))
            {
                if (uiMaxRetries < uiIndex++)
                {
                    r |= AVL6381_EC_RUNNING;
                    return r;
                 }
                 AVL6381_IBSP_Delay(uiDelayUnit);
             }
         }
         r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_cpu_ddc_srst, 0);
         AVL6381_IBSP_Delay(1);
         r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_cpu_ddc_srst, 1);                                                        
         r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_core_ready_word_iaddr, 0x0);
         r |= SendRxOP_6381(OP_RX_CHANGE_MODE,ePortIndex);
         if(AVL6381_EC_OK == r )
         {
             uiIndex = 0;
             while (AVL6381_EC_OK != GetRxOP_Status_6381(ePortIndex))
             {
                 if ((10*uiMaxRetries) < uiIndex++)
                 {
                     r |= AVL6381_EC_RUNNING;
                     return r;
                 }
                 AVL6381_IBSP_Delay(uiDelayUnit);
             }
         }
         uiIndex = 0;

         while (AVL6381_EC_OK != CheckChipReady_6381(ePortIndex))
         {
             if (uiMaxRetries < uiIndex++)
             {
                 return AVL6381_EC_GENERAL_FAIL;
             }
             AVL6381_IBSP_Delay(uiDelayUnit);
         }
         if(mode == AVL_DEMOD_MODE_DTMB)
         {
             ucPLLIndex = gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig * 2;
         }
         else if(mode == AVL_DEMOD_MODE_DVBC)
         {
             ucPLLIndex = (gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig * 2) + 1;
         }
         r = I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_c306_top_srst, 1);
         r |= SetPLL_6381(&gAVL6381PLLConfig[ePortIndex][ucPLLIndex],ePortIndex);
         r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_core_ready_word_iaddr, 0x0);
         r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_c306_top_srst, 0);
    }
    else
    {
        if(mode == AVL_DEMOD_MODE_DTMB){
            ucPLLIndex = gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig * 2;
        }else{
            ucPLLIndex = gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig * 2 + 1;
        }
        r |= IBase_Initialize_6381(&(gAVL6381PLLConfig[ePortIndex][ucPLLIndex]),ePortIndex);
    }
    AVL6381_IBSP_Delay(uiDelayUnit);
    for(uiCounter = 0; uiCounter < uiTimeoutCount; uiCounter++)
    {
        r = CheckChipReady_6381 (ePortIndex);
        if(r == AVL6381_EC_OK)
        {
            break;
        }
        AVL6381_IBSP_Delay(uiDelayUnit);
    }
    
    /* Receiver Init */
    r |= IRx_Initialize_6381(ePortIndex);
    
    if(mode == AVL_DEMOD_MODE_DTMB)
    {
        r |= DTMB_SetSymbolRate_6381(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBSymbolRateHz,ePortIndex);
    }
    
    r |= SetMpegMode_6381(ePortIndex);
    r |= SetMpegSerialPin_6381(geAVL6381SerialPin[ePortIndex],ePortIndex);
    r |= SetMpegSerialOrder_6381(geAVL6381SerialOrder[ePortIndex],ePortIndex);
    r |= SetMpegSerialSyncPulse_6381(geAVL6381SerialSync[ePortIndex],ePortIndex);
    r |= SetMpegErrorBit_6381(geAVL6381ErrorBit[ePortIndex],ePortIndex);
    r |= SetMpegErrorPola_6381(geAVL6381ErrorPola[ePortIndex],ePortIndex);
    r |= SetMpegValidPola_6381(geAVL6381ValidPola[ePortIndex],ePortIndex);
    r |= SetMpegPacketLen_6381(geAVL6381PacketLen[ePortIndex],ePortIndex);
    
    if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumConti == AVL6381_MPEG_CONTINUOUS_ENABLE)
    {
        if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel == AVL6381_MPEG_SERIAL)
        {
            if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DTMB_EnableMpegContinuousMode_6381(guiDTMBSerialMPEGContinuousHz[ePortIndex],ePortIndex);
            }
            else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DVBC_EnableMpegContinuousMode_6381(guiDVBCSerialMPEGContinuousHz[ePortIndex],ePortIndex);
            }
        }
        else
        {
            if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DTMB_EnableMpegContinuousMode_6381(guiParallelMPEGContinuousHz[ePortIndex],ePortIndex);
            }
            else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r |= DVBC_EnableMpegContinuousMode_6381(guiParallelMPEGContinuousHz[ePortIndex],ePortIndex);
            }
        }
    }
    
    if(gucDisableMPEGOutput[ePortIndex] == 0)
    {
        r |= EnableMpegOutput_6381(ePortIndex);
    }

    /* Reset tuner I2C module */
    r |= TunerI2C_Initialize_6381(ePortIndex);

    /* Set AGC */
    r |= SetAGCPola_6381(geAVL6381AGCPola[ePortIndex],ePortIndex);
    if(gucDisableAGC[ePortIndex] == 0)
    {
        r |= EnableAGC_6381(ePortIndex);
    }

    /* Error statistics Setting */
    r |= InitErrorStat_6381(ePortIndex);

    return r;
}

AVL6381_ErrorCode AVL6381_Sleep(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar ucCommandRetry = 0;

    r = SetSleepClock_6381(ePortIndex); 
    
    r |= TunerI2C_Initialize_6381(ePortIndex);

    r |= SendRxOP_6381(OP_RX_SLEEP,ePortIndex);    

    while(ucCommandRetry < 10)
    {
        r = GetRxOP_Status_6381(ePortIndex); 
        if(r == AVL6381_EC_OK)
        {
            break;
        }
        ucCommandRetry++;
        AVL6381_IBSP_Delay(10);
    }

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_cpu_ddc_srst, 1); 
    
    return r;
}

AVL6381_ErrorCode AVL6381_Wakeup(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTimeDelay = 5;
    AVL_uint32 uiMaxRetries = 10;
    AVL_uchar ucCommandRetry = 0;
    AVL_uchar ucCurrentPllIndex = 0;
    AVL_uchar ucCurrentMode = AVL_DEMOD_MODE_UNKNOWN;
    AVL_uint32 uiIndex=0;

    r = SendRxOP_6381(OP_RX_WAKE_UP, ePortIndex); 
    
    while(ucCommandRetry < 10)
    {
        r = GetRxOP_Status_6381(ePortIndex); 
        if(r == AVL6381_EC_OK)
        {
            break;
        }
        ucCommandRetry++;
        AVL6381_IBSP_Delay(10);
    }

    if(r != AVL6381_EC_OK)
    {
        return r;
    }

    r |= GetMode_6381(&ucCurrentMode,ePortIndex); 
    switch(ucCurrentMode)
    {
        case AVL_DEMOD_MODE_DTMB:
            ucCurrentPllIndex = gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig * 2;
            break;
        case AVL_DEMOD_MODE_DVBC:
            ucCurrentPllIndex = (gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig * 2) + 1;
            break;
        default:
            r |= AVL6381_EC_GENERAL_FAIL;
            break;
    }

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_c306_top_srst, 1);
    r |= SetPLL_6381(&(gAVL6381PLLConfig[ePortIndex][ucCurrentPllIndex]),ePortIndex);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_core_ready_word_iaddr, 0x00000000);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_c306_top_srst, 0);

    while (AVL6381_EC_OK != CheckChipReady_6381(ePortIndex))
    {
        if (uiMaxRetries < uiIndex++)
        {
            return AVL6381_EC_GENERAL_FAIL;
        }
        AVL6381_IBSP_Delay(uiTimeDelay);
    }

    r |= IRx_Initialize_6381(ePortIndex);

    if(ucCurrentMode == AVL_DEMOD_MODE_DTMB)
    {
        r |= DTMB_SetSymbolRate_6381(guiDTMBSymbolRateSps[ePortIndex], ePortIndex);
    }

    r |= SetMpegMode_6381(ePortIndex);
    r |= SetMpegSerialPin_6381(geAVL6381SerialPin[ePortIndex],ePortIndex);
    r |= SetMpegSerialOrder_6381(geAVL6381SerialOrder[ePortIndex],ePortIndex);
    r |= SetMpegSerialSyncPulse_6381(geAVL6381SerialSync[ePortIndex],ePortIndex);
    r |= SetMpegErrorBit_6381(geAVL6381ErrorBit[ePortIndex],ePortIndex);
    r |= SetMpegErrorPola_6381(geAVL6381ErrorPola[ePortIndex],ePortIndex);
    r |= SetMpegValidPola_6381(geAVL6381ValidPola[ePortIndex],ePortIndex);
    r |= SetMpegPacketLen_6381(geAVL6381PacketLen[ePortIndex],ePortIndex);

    if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumConti == AVL6381_MPEG_CONTINUOUS_ENABLE)
    {
        if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel == AVL6381_MPEG_SERIAL)
        {
            if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r = DTMB_EnableMpegContinuousMode_6381(guiDTMBSerialMPEGContinuousHz[ePortIndex],ePortIndex);
            }
            else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r = DVBC_EnableMpegContinuousMode_6381(guiDVBCSerialMPEGContinuousHz[ePortIndex],ePortIndex);
            }
        }
        else
        {
            if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r = DTMB_EnableMpegContinuousMode_6381(guiParallelMPEGContinuousHz[ePortIndex],ePortIndex);
            }
            else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
            {
                r = DVBC_EnableMpegContinuousMode_6381(guiParallelMPEGContinuousHz[ePortIndex],ePortIndex);
            }
        }
    }
    
    if(gucDisableMPEGOutput[ePortIndex] == 0)
    {
        r |= EnableMpegOutput_6381(ePortIndex);
    }

    r |= TunerI2C_Initialize_6381(ePortIndex);

    r |= SetAGCPola_6381(geAVL6381AGCPola[ePortIndex],ePortIndex);
    if(gucDisableAGC[ePortIndex] == 0)
    {
        r |= EnableAGC_6381(ePortIndex);
    }


    r |= InitErrorStat_6381(ePortIndex);

    return r;
}

AVL6381_ErrorCode AVL6381_GetPER(AVL_puint32 puiPER_x1e9,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiHwCntPktErrors = 0;
    AVL_uint32 uiHwCntNumPkts = 0;
    AVL_uint32 uiTemp = 0;
    AVL_uchar ucTemp = 0;
    AVL_uint16 usTemp = 0;
    AVL6381_uint64 uiTemp64 = {0,0};

    r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_packet_err_cnt, &uiHwCntPktErrors);
    r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_packet_num, &uiHwCntNumPkts);
    
  //  LogPrint("uiHwCntPktErrors:%d uiHwCntNumPkts:%d ePortIndex:%d m_SlaveAddr:0x%x\n",uiHwCntPktErrors,uiHwCntNumPkts,ePortIndex,gsAVL6381Chip[ePortIndex].m_SlaveAddr);
    if(uiHwCntNumPkts > (1 << 31))
    {
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_esm_cntrl, &uiTemp);
        uiTemp |= 0x00000001;
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_esm_cntrl, uiTemp);
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_packet_err_cnt, &uiHwCntPktErrors);
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_packet_num, &uiHwCntNumPkts);
        uiTemp &= 0xFFFFFFFE;
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_esm_cntrl, uiTemp);
        WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_SwCntNumPkts, uiHwCntNumPkts);
        WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_SwCntPktErrors, uiHwCntPktErrors);
        uiHwCntNumPkts = 0;
        uiHwCntPktErrors = 0;
    }

    gsAVL6381ErrorStat[ePortIndex].m_NumPkts.m_HighWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntNumPkts.m_HighWord;
    gsAVL6381ErrorStat[ePortIndex].m_NumPkts.m_LowWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntNumPkts.m_LowWord;
    WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_NumPkts, uiHwCntNumPkts);
    gsAVL6381ErrorStat[ePortIndex].m_PktErrors.m_HighWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntPktErrors.m_HighWord;
    gsAVL6381ErrorStat[ePortIndex].m_PktErrors.m_LowWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntPktErrors.m_LowWord;
    WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_PktErrors, uiHwCntPktErrors);
    // Compute the PER
    //uiTemp = WVDivide64(gsAVL6381ErrorStat[ePortIndex].m_NumPkts, gsAVL6381ErrorStat[ePortIndex].m_PktErrors);
    //WVMultiply32(&uiTemp64, uiTemp, (AVL_uint32)1e9);
    //gsAVL6381ErrorStat[ePortIndex].m_PER = (uiTemp64.m_LowWord >> 30) + (uiTemp64.m_HighWord << 2);
    //
    WVMultiply32(&uiTemp64, gsAVL6381ErrorStat[ePortIndex].m_PktErrors.m_LowWord, (AVL_uint32)1e9);
    uiTemp = WVDivide64(gsAVL6381ErrorStat[ePortIndex].m_NumPkts, uiTemp64);
    gsAVL6381ErrorStat[ePortIndex].m_PER = uiTemp;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r |= I2C_Read8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_lost_lock_caddr, &ucTemp);
        usTemp = ucTemp;
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r |= I2C_Read16_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DVBC_lost_lock_saddr, &usTemp);
    }
    else
    {
        return AVL6381_EC_GENERAL_FAIL;
    }
    
    gsAVL6381ErrorStat[ePortIndex].m_LostLock = usTemp;

    *puiPER_x1e9 = gsAVL6381ErrorStat[ePortIndex].m_PER;

    return r;
}

AVL6381_ErrorCode AVL6381_TunerRead(Avl6381_PortIndex_t ePortIndex, AVL_uchar ucSlaveAddr, AVL_puchar pucRegAddr, AVL_uchar ucAddrSize, AVL_puchar pucBuff, AVL_uint16 usSize)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint16 usAddrSize = 0;
    AVL_uint16 us1 = 0, us2 = 0;
    AVL_uint16 usTempSize = 0;

   // r = AVL6381_IBSP_WaitSemaphore(&(gAVL6381I2CSem[ePortIndex]));

    if(ucAddrSize != 0)
    {
        usAddrSize = (AVL_uint16)ucAddrSize;//8bit to 16bit, safely
        r |= AVL6381_IBSP_I2C_Write(ePortIndex, ucSlaveAddr,pucRegAddr,(AVL_puint16)&usAddrSize);
    }

    if( AVL6381_EC_OK == r )
    {
        usTempSize = usSize;
        us2 = 0;
        while( usTempSize > MAX_II2C_READ_SIZE )
        {
            us1 = MAX_II2C_READ_SIZE;
            r |= AVL6381_IBSP_I2C_Read(ePortIndex, ucSlaveAddr, pucBuff+us2, &us1);
            us2 += MAX_II2C_READ_SIZE;
            usTempSize -= MAX_II2C_READ_SIZE;
        }

        if( 0 != usTempSize )
        {
            r |= AVL6381_IBSP_I2C_Read(ePortIndex, ucSlaveAddr, pucBuff+us2, &usTempSize);
        }
    }

    //r |= AVL6381_IBSP_ReleaseSemaphore(&(gAVL6381I2CSem[ePortIndex])); 

    return r;
}

AVL6381_ErrorCode AVL6381_TunerWrite(Avl6381_PortIndex_t ePortIndex, AVL_uchar ucSlaveAddr, AVL_puchar pucBuff, AVL_uint16 usSize)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint16 us1 = 0, us2 = 0, usMaxWriteLen = 0;
    AVL_uint16 usTempSize = 0;

    //r = AVL6381_IBSP_WaitSemaphore(&(gAVL6381I2CSem[ePortIndex]));
    if( AVL6381_EC_OK == r )
    {
        usTempSize = usSize;
        usMaxWriteLen = (MAX_II2C_WRITE_SIZE-3) & 0xfffe; 
        us2 = 0;
        while( usTempSize > usMaxWriteLen )
        {
            us1 = usMaxWriteLen;
            r |= AVL6381_IBSP_I2C_Write(ePortIndex, ucSlaveAddr, pucBuff+us2, &us1);
            us2 += usMaxWriteLen;
            usTempSize -= usMaxWriteLen;
        }
        us1 = usTempSize;
        r |= AVL6381_IBSP_I2C_Write(ePortIndex, ucSlaveAddr, pucBuff+us2, &us1);
        us2 += usTempSize;
    }
    //r |= AVL6381_IBSP_ReleaseSemaphore(&(gAVL6381I2CSem[ePortIndex])); 

    return r;
}

