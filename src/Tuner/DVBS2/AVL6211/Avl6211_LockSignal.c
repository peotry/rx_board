/*
 *           Copyright 2007-2009 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */


#include <stdio.h>

#include "appGlobal.h"

#include "IBSP.h"
#include "avl_dvbsx.h"
#include "IBase.h"
#include "IRx.h"
#include "AVL6211/ITuner.h"
#include "II2C.h"
#include "IDiseqc.h"
#include "IBlindScan.h"
#include "Avl6211_LockSignal.h"

#include "Avl6211Manager.h"
#include "RDA5815M.h"

#define AT_SleepMS(MS)          usleep(MS*1000)

//#define DEBUG_AVL6211_LOCK
#ifdef DEBUG_AVL6211_LOCK
#define Avl6211_ErrorPrint(x)        HTRACE_ERROR(x)
#define Avl6211_InfoPrint(x)         HTRACE_INFO(x)
#else
#define Avl6211_ErrorPrint(x)        printf x
#define Avl6211_InfoPrint(x)         printf x

#endif
extern AVL_uchar ucPatchData[]; //Defined in AVL6211_patch.c.
#define AVL6211_DEBUG
extern U16  SM_GetStatus(void);
#define AVL6211_DEBUG



//hawayi 2012.05.22 config for Sharp_7903
#if 0
static struct Signal_Level  SignalLevel [47]=
{
    {4525,  -92},
    {5420,  -90},
    {6240,  -88},
    {7190,  -86},
    {8500,  -84},
    {9730,  -82},
    {10900, -80},
    {12190, -78},
    {13450, -76},
    {14720, -74},
    {15910, -72},
    {16910, -70},
    {18100, -68},
    {19120, -66},
    {20610, -64},
    {22220, -62},
    {23530, -60},
    {24780, -58},
    {25740, -56},
    {26560, -54},
    {27360, -52},
    {28460, -50},
    {29460, -48},
    {30410, -46},
    {31560, -44},
    {32640, -42},
    {33540, -40},
    {34760, -38},
    {35890, -36},
    {37080, -34},
    {39160, -32},
    {40350, -30},
    {41350, -28},
    {42050, -26},
    {42760, -24},
    {43360, -22},
    {43990, -20},
    {44650, -18},
    {45440, -16},
    {46440, -14},
    {47640, -12},
    {49040, -10},
    {50360, -8},
    {51550, -6},
    {52770, -4},
    {53620, -2},
    {53950, -1}
};
#else //2012.11.16
static struct Signal_Level  SignalLevel [47]=
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
#endif

/// Users can remove unused elements from this array to reduce the foot print.
/// Please note that some compilers do not support the comma after the last element. 
/// Be careful for it when removing the last element.

//hawayi 2012.05.16 move here from file IBSP.c
static const struct AVL_DVBSx_PllConf pll_conf[] =
{
    // The following set of PLL configuration at different reference clock frequencies refer to demod operation
    // in standard performance mode.
     {503,  1, 7, 4, 2,  4000, 11200, 16800, 25200} ///< Reference clock 4 MHz,   Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{447,  1, 7, 4, 2,  4500, 11200, 16800, 25200} ///< Reference clock 4.5 MHz, Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{503,  4, 7, 4, 2, 10000, 11200, 16800, 25200} ///< Reference clock 10 MHz,  Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{503,  7, 7, 4, 2, 16000, 11200, 16800, 25200} ///< Reference clock 16 MHz,  Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{111,  2, 7, 4, 2, 27000, 11200, 16800, 25200} ///< Reference clock 27 MHz,  Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    
    // The following set of PLL configuration at different reference clock frequencies refer to demod operation
    // in high performance mode. 
    ,{566,  1, 7, 4, 2,  4000, 12600, 18900, 28350} /// < Reference clock 4 MHz,   Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{503,  1, 7, 4, 2,  4500, 12600, 18900, 28350} ///< Reference clock 4.5 MHz, Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{566,  4, 7, 4, 2, 10000, 12600, 18900, 28350} ///< Reference clock 10 MHz,  Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{566,  7, 7, 4, 2, 16000, 12600, 18900, 28350} ///< Reference clock 16 MHz,  Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{377,  8, 7, 4, 2, 27000, 12600, 18900, 28350} ///< Reference clock 27 MHz,  Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz

};

static const AVL_uint16 pll_array_size = sizeof(pll_conf)/sizeof(struct AVL_DVBSx_PllConf);
//hawayi end

static AVL_uint32 _VerifySNRValue(AVL_uint32 uiSNR)
{   
    if (uiSNR < 470){
        return uiSNR;
    }
    else if (uiSNR < 570){
        return uiSNR-70;
    }
    else if (uiSNR < 610){
        return uiSNR-50;
    }
    else if (uiSNR < 640){
        return uiSNR-40;
    }
    else if (uiSNR < 770){
        return uiSNR-20;
    }
    else if (uiSNR < 1250){
        return uiSNR-0;
    }
    else if (uiSNR < 1400){
        return uiSNR+20;
    }
    else if (uiSNR < 1500){
        return uiSNR+30;
    }
    else if (uiSNR < 1600){
        return uiSNR+40;
    }
    else if (uiSNR < 1700){
        return uiSNR+50;
    }
    else if (uiSNR < 1800){
        return uiSNR+60;
    }
    else if (uiSNR < 1880){
        return uiSNR+70;
    }
    else if (uiSNR < 1950){
        return uiSNR+80;
    }
    else if (uiSNR < 2000){
        return uiSNR+120;
    }
    else if (uiSNR < 2100){
        return uiSNR+140;
    }
    else if (uiSNR < 2150){
        return uiSNR+170;
    }
    else if (uiSNR < 2220){
        return uiSNR+205;
    }
    else if (uiSNR < 2280){
        return uiSNR+245;
    }
    else if (uiSNR < 2320){
        return uiSNR+290;
    }
    else if (uiSNR < 2380){
        return uiSNR+340;
    }
    else if (uiSNR < 2420){
        return uiSNR+395;
    }
    else if (uiSNR < 2460){
        return uiSNR+455;
    }
    else if (uiSNR < 2500){
        return uiSNR+515;
    }
    else if (uiSNR <= 2530){
        return (3100-3000)*(uiSNR-2500)/(2530-2500) + 3000;
    }
    else if (uiSNR <= 2540){
        return (3200-3100)*(uiSNR-2530)/(2540-2530) + 3100;
    }
    else if (uiSNR <= 2560){
        return (3300-3200)*(uiSNR-2540)/(2560-2540) + 3200;
    }
    else if (uiSNR <= 2590){
        return (4000-3300)*(uiSNR-2560)/(2590-2560) + 3300;
    }
    else if (uiSNR <= 2610){
        return (4500-4000)*(uiSNR-2590)/(2610-2590) + 4000;
    }
    else if (uiSNR <= 2630){
        return (5000-4500)*(uiSNR-2610)/(2630-2610) + 4500;
    }
    else{
        return 5200; //max 52dB.
    }
}

static AVL_DVBSx_ErrorCode CPU_Halt(AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip)
{
     AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
     AVL_uint16 i= 0;
     
     r = AVL_DVBSx_IBase_SendRxOP(u32Port, OP_RX_HALT, pAVLChip);

     if(AVL_DVBSx_EC_OK == r)
     {
          while(i++<20)
          {
              r = AVL_DVBSx_IBase_GetRxOPStatus(u32Port, pAVLChip);
              if(AVL_DVBSx_EC_OK == r)
              {
                   break;
              }
              else
              {
                   AVL_DVBSx_IBSP_Delay(10);
              }
          }
     }
     
     return (r);
}

AVL_DVBSx_ErrorCode Avl6211_GetPacketErrorRate(AVL_uint32  u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_uint32 *uiPER)
{   
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r = AVL_DVBSx_IRx_ResetDVBSBER( u32Port, pAVLChip);
    if (AVL_DVBSx_EC_OK != r)
    {   
        Avl6211_ErrorPrint(("\r\n Avl6211_IDVBSxRx_ResetDVBSBER is failed! Err = %d \r\n",r));
        return r;
    }
    AVL_DVBSx_IBSP_Delay(1);     //Wait a while for SNR BER/PER calculation
    
    r = AVL_DVBSx_IRx_GetPER( u32Port, uiPER, pAVLChip);
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("==>[%s] is failed! Err = %d \r\n", __FUNCTION__, r));
        return r;
    }   
    else
    {
        Avl6211_InfoPrint(("PER=%.9f\n",(float)((*uiPER)*1.0e-9)));
    }
    
    return r;
}

AVL_DVBSx_ErrorCode Avl6211_GetFECCodeRate(AVL_uint32  u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_uchar *pCodeRate, AVL_int32 *pDemodState)
{   
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    struct AVL_DVBSx_SignalInfo SignalInfo; 



    //AVL_DVBSx_IBSP_Delay(500);        //Wait a while for SNR BER/PER calculation
    r=AVL_DVBSx_IRx_GetSignalInfo( u32Port, &SignalInfo, pAVLChip);
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("Avl6211_IDVBSxRx_GetSignalInfo is failed! Err = %d \r\n",r));
        return r;
    }



    // 根据显示需要,对CodeRate值做调整, 对应 WEB 的strFECs 以及 strFECs2字符串数组, 与D0900 tuner 一致
    // DVB-S
    if (0 <= SignalInfo.m_coderate && SignalInfo.m_coderate <= 5)
    {
        switch(SignalInfo.m_coderate)
        {
            case RX_DVBS_5_6:
                *pCodeRate = 4; 
                break; 
            case RX_DVBS_6_7:
                *pCodeRate = 5; 
                break; 
            case RX_DVBS_7_8:
                *pCodeRate = 6; 
                break; 
            default:
                *pCodeRate = SignalInfo.m_coderate;
                break; 
        }
        *pDemodState= 3; 
    }
    // DVB-S2
    else if (6 <= SignalInfo.m_coderate && SignalInfo.m_coderate <= 16)
    {
        *pDemodState= 2;
        
        if (SignalInfo.m_modulation == AVL_DVBSx_MM_QPSK)
        {
            *pCodeRate = SignalInfo.m_coderate - 5; 
        }
        else if (SignalInfo.m_modulation == AVL_DVBSx_MM_8PSK)
        {
            switch(SignalInfo.m_coderate)
            {
                case RX_DVBS2_3_5:
                    *pCodeRate = 12; 
                    break; 
                case RX_DVBS2_2_3:
                    *pCodeRate = 13;
                    break; 
                case RX_DVBS2_3_4:
                    *pCodeRate = 14;
                    break; 
                case RX_DVBS2_5_6:
                    *pCodeRate = 15;
                    break; 
                case RX_DVBS2_8_9:
                    *pCodeRate = 16; 
                    break; 
                case RX_DVBS2_9_10:
                    *pCodeRate = 17; 
                    break; 
                default:
                    *pCodeRate = 0; 
                    break; 
            }
        }
        else if (SignalInfo.m_modulation == AVL_DVBSx_MM_16APSK)
        {
            switch(SignalInfo.m_coderate)
            {
                case RX_DVBS2_2_3:
                    *pCodeRate = 18; 
                    break; 
                case RX_DVBS2_3_4:
                    *pCodeRate = 19; 
                    break; 
                case RX_DVBS2_4_5:
                    *pCodeRate = 20; 
                    break; 
                case RX_DVBS2_5_6:
                    *pCodeRate = 21; 
                    break; 
                case RX_DVBS2_8_9:
                    *pCodeRate = 22; 
                    break; 
                case RX_DVBS2_9_10:
                    *pCodeRate = 23; 
                    break; 
                default:
                    *pCodeRate = 0; 
                    break; 
            }
        }
        else if (SignalInfo.m_modulation == AVL_DVBSx_MM_32APSK)
        {
            switch(SignalInfo.m_coderate)
            {
                case RX_DVBS2_3_4:
                    *pCodeRate = 24; 
                    break; 
                case RX_DVBS2_4_5:
                    *pCodeRate = 25; 
                    break; 
                case RX_DVBS2_5_6:
                    *pCodeRate = 26; 
                    break; 
                case RX_DVBS2_8_9:
                    *pCodeRate = 27; 
                    break; 
                case RX_DVBS2_9_10:
                    *pCodeRate = 28; 
                    break; 
                default:
                    *pCodeRate = 0; 
                    break; 
            }
        }
    }

    Avl6211_InfoPrint(("*pCodeRate = %d.\n", *pCodeRate));
    
    return r;
}

AVL_DVBSx_ErrorCode Avl6211_GetBitErrorRate(AVL_uint32  u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_uint32 *uiBER)
{   
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r = AVL_DVBSx_IRx_ResetDVBSBER( u32Port, pAVLChip);
    if (AVL_DVBSx_EC_OK != r)
    {   
        Avl6211_ErrorPrint(("\r\n Avl6211_IDVBSxRx_ResetDVBSBER is failed! Err = %d \r\n",r));
        return r;
    }
    

    //add by tiantian 
    AVL_DVBSx_IBSP_Delay(1);     //Wait a while for SNR BER/PER calculation
    
    /*This function can be called to read back the current BER calculation
    result after function Avl6211_IDVBSxRx_ResetErrorStat called.*/
    r = AVL_DVBSx_IRx_GetDVBSBER( u32Port, uiBER, pAVLChip);
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\nAvl6211_IDVBSxRx_GetDVBSBER is failed! Err = %d \r\n",r));
        return r;
    }   
    else
    {
        Avl6211_InfoPrint(("BER=%.9f\n",(float)((*uiBER)*1.0e-9)));
    }
    
    return r;
}

AVL_DVBSx_ErrorCode Avl6211_GetSignalQuality(AVL_uint32  u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_int32 *SignalQuality)
{   
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    struct AVL_DVBSx_SignalInfo SignalInfo; 
//  AVL_uint32 uiBER = 0x7fffffff;
//  AVL_uint32 uiPER = 0x7fffffff;
    AVL_uint32 uiSNR = 0;

    r=AVL_DVBSx_IRx_GetSignalInfo( u32Port, &SignalInfo, pAVLChip);
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("Avl6211_IDVBSxRx_GetSignalInfo is failed! Err = %d \r\n",r));
        return r;
    }
    
    r = AVL_DVBSx_IRx_GetSNR( u32Port, &uiSNR, pAVLChip);//the unit is db*100;exp:27.58db ,uiSNR=2758;
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\nGet SNR is failed! Err = %d \r\n",r));
    }
    else
    {   
        Avl6211_InfoPrint(("\nSNR=%d\n",uiSNR));    
    }
#if 1 //hawayi 2012.11.16
    //if (pAVLChip->m_SlaveAddr == AVL_DVBSx_SA_0)
    {
        //*SignalQuality = uiSNR/10; //it must be divided 10 again when display
        *SignalQuality = _VerifySNRValue(uiSNR)/10;
        return r;
    }
#endif
    
    switch(SignalInfo.m_modulation)
    {
        case AVL_DVBSx_MM_QPSK:
        switch(SignalInfo.m_coderate)
        {
            case RX_DVBS_1_2:
            case RX_DVBS2_1_2:
            *SignalQuality=((AVL_int32)uiSNR-420)*70/250+30;
            break;
        
            case RX_DVBS_2_3:
            case RX_DVBS2_3_5:
            case RX_DVBS2_2_3:
            *SignalQuality=((AVL_int32)uiSNR-500)*70/250+30;
            break;
        
            case RX_DVBS_3_4:
            case RX_DVBS2_3_4:
            case RX_DVBS2_4_5:  
            *SignalQuality=((AVL_int32)uiSNR-580)*70/250+30;
            break;
        
            case RX_DVBS_5_6:
            case RX_DVBS_6_7:
            case RX_DVBS2_5_6:
            *SignalQuality=((AVL_int32)uiSNR-660)*70/250+30;
            break;
        
            case RX_DVBS_7_8:
            case RX_DVBS2_8_9:
            case RX_DVBS2_9_10:
            *SignalQuality=((AVL_int32)uiSNR-740)*70/250+30;
            break;
        
            default:
            break;
        }
        break;

        case AVL_DVBSx_MM_8PSK:
        switch(SignalInfo.m_coderate)
        {
            case RX_DVBS2_1_2:          
            case RX_DVBS2_3_5:
            *SignalQuality=((AVL_int32)uiSNR-550)*70/250+30;
            break;
            
            case RX_DVBS2_2_3:
            *SignalQuality=((AVL_int32)uiSNR-650)*70/250+30;
            break;
            
            case RX_DVBS2_3_4:
            case RX_DVBS2_4_5:
            *SignalQuality=((AVL_int32)uiSNR-780)*70/250+30;
            break;
            
            case RX_DVBS2_5_6:
            *SignalQuality=((AVL_int32)uiSNR-920)*70/250+30;
            break;
            
            case RX_DVBS2_8_9:
            *SignalQuality=((AVL_int32)uiSNR-1060)*70/250+30;
            break;  
            
            case RX_DVBS2_9_10:
            *SignalQuality=((AVL_int32)uiSNR-1100)*70/250+30;
            default:
            break;          
        }
        break;

        case AVL_DVBSx_MM_16APSK:
        case AVL_DVBSx_MM_32APSK:
        default:
        break;
    }

    Avl6211_InfoPrint(("\nSignalQuality=%d\n",*SignalQuality));
    
    if(*SignalQuality>100)
    {
        *SignalQuality=100;
    }
    if(*SignalQuality<0)
    {
        *SignalQuality=0;
    }
        
    return r;
}   

AVL_DVBSx_ErrorCode Avl6211_GetSignalStrength(AVL_uint32 u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_int32 *SignalStrength)
{   
    AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
    AVL_uint16 uiRFSignalLevel;
    AVL_int16  uiRFSignalDBM;
    AVL_uint32 i;
    
    r=AVL_DVBSx_IRx_GetSignalLevel( u32Port, &uiRFSignalLevel, pAVLChip);
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Get SignalLevel --- Failed! Err = %d \r\n",r));
    }
    else
    {
        for(i=0; i<47; i++)
        {
            if(uiRFSignalLevel<= SignalLevel[i].SignalLevel)   
            {
                //Calculate the corresponding DBM value.
                if((0==i)&&(uiRFSignalLevel< SignalLevel[i].SignalLevel))
                {   

                    Avl6211_ErrorPrint(("RFSignalLevel is too weak !\n"));
                    uiRFSignalDBM = -94;
                }
                else
                {
                    if (i > 0){
                        uiRFSignalDBM = (uiRFSignalLevel - SignalLevel[i-1].SignalLevel)*
                            (SignalLevel[i].SignalDBM - SignalLevel[i-1].SignalDBM)/
                            (SignalLevel[i].SignalLevel - SignalLevel[i-1].SignalLevel);
                        uiRFSignalDBM += SignalLevel[i-1].SignalDBM;
                    }
                    else{
                        uiRFSignalDBM=SignalLevel[i].SignalDBM;
                    }
                }
                break;
            } 
        }
    }  
    
    *SignalStrength=uiRFSignalDBM;

    return r;
}   


BOOL RDA5815Init(struct AVL_Tuner * pTuner, Avl6211_PortIndex_t ePortIndex)
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r = AVL_DVBSx_II2C_Write16(ePortIndex, pTuner->m_pAVLChip, rc_tuner_slave_addr_addr, pTuner->m_uiSlaveAddress);
    r |= AVL_DVBSx_II2C_Write16(ePortIndex, pTuner->m_pAVLChip, rc_tuner_use_internal_control_addr, 0);
    r |= AVL_DVBSx_II2C_Write16(ePortIndex, pTuner->m_pAVLChip, rc_tuner_LPF_margin_100kHz_addr, 0);    //clean up the LPF margin for blind scan. for external driver, this must be zero.
    r |= AVL_DVBSx_II2C_Write16(ePortIndex, pTuner->m_pAVLChip, rc_tuner_max_LPF_100kHz_addr, 320); //set up the right LPF for blind scan to regulate the freq_step. This field should corresponding the flat response part of the LPF.
    r |= AVL_DVBSx_II2CRepeater_Initialize(ePortIndex, pTuner->m_uiI2CBusClock_kHz, pTuner->m_pAVLChip);

    return FALSE;
}



AVL_DVBSx_ErrorCode Avl6211_Initialize(Avl6211_PortIndex_t ePortIndex, struct AVL_DVBSx_Chip * pAVLChip, struct AVL_Tuner * pTuner)
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    
    struct AVL_DVBSx_Diseqc_Para sDiseqcPara;
    struct AVL_DVBSx_MpegInfo sMpegMode;
    
    /* This function should be called after bsp initialized to initialize the chip object.*/
    r = Init_AVL_DVBSx_ChipObject(pAVLChip, 0); 
    if( AVL_DVBSx_EC_OK !=r ) 
    {
        Avl6211_ErrorPrint(("\r\n InitAvl6211ChipObject failed! Err = %d \r\n",r));;
        return (r);
    }

    //Setup tuner parameters for tuner initialization.
    pTuner->m_uiSlaveAddress = TUNER_ADDRESS;        
    pTuner->m_uiI2CBusClock_kHz = 100;//Tuner_I2Cbus_Clock;       
    pTuner->m_pParameters = 0;          
    pTuner->m_uiSymbolRate_Hz = 45000000; 
    pTuner->m_pAVLChip = pAVLChip;

    
    pTuner->m_pInitializeFunc = &RDA5815Init;     
    pTuner->m_pLockFunc = &RDA5815Lock; 
    //pTuner->m_pGetLockStatusFunc = ;


    //add by tiantian 
    AVL_DVBSx_IBSP_Delay(100);
    
    r = AVL_DVBSx_IBase_Initialize(ePortIndex, (struct AVL_DVBSx_PllConf *)(pll_conf+4), ucPatchData, pAVLChip); 

    if( AVL_DVBSx_EC_OK !=r ) 
    {
        Avl6211_ErrorPrint(("\r\n IBase Initialization failed! Err = %d \r\n",r));
        return (r);
    }

    AVL_DVBSx_IBSP_Delay(100);    //Wait 100 ms to assure that the AVL6211 chip boots up.This function should be implemented by customer.
 
    //This function should be called to verify the AVL6211 chip has completed its initialization procedure.
    r = AVL_DVBSx_IBase_GetStatus(ePortIndex, pAVLChip);

   if( AVL_DVBSx_EC_OK != r ) 
    {
        Avl6211_ErrorPrint(("\r\n Booted failed! Err = %d \r\n",r));
        return (r);
    }

    //This function should be called after IBase initialized to initialize the demod.
    r = AVL_DVBSx_IRx_Initialize( ePortIndex, pAVLChip);


    if(AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Avl6211_IDVBSxRx_Initialize failed! Err = %d \r\n",r));
        return (r);
    }

    //modify rx_config.aagc_ref
    r |= AVL_DVBSx_II2C_Write16(ePortIndex, pAVLChip, rc_aagc_ref_addr, 48);
    r |= AVL_DVBSx_IRx_DriveRFAGC( ePortIndex, pAVLChip);

    //This function should be called after demod initialized to set RF AGC polar.
    //User does not need to setup this for Sharp tuner since it is the default value. But for other tuners, user may need to do it here.
    r = AVL_DVBSx_IRx_SetRFAGCPola( ePortIndex, AVL_DVBSx_RA_Invert, pAVLChip);
    if(AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Avl6211_IDVBSxRx_SetRFAGCPola failed! Err = %d \r\n",r));
        return (r);
    }
    
    //This function should be called after demod initialized to set spectrum polar.
    r = AVL_DVBSx_IBase_SetSpectrumPolarity(ePortIndex, AVL_DVBSx_Spectrum_Invert, pAVLChip); //<--AVL_DVBSx_Spectrum_Normal //hawayi 2012.05.18
    if(AVL_DVBSx_EC_OK != r)
    {
        printf("Set Spectrum Polar failed !\n");
        return (r);
    }

    sMpegMode.m_MpegFormat = AVL_DVBSx_MPF_TSP;
    sMpegMode.m_MpegMode = AVL_DVBSx_MPM_Serial;
    sMpegMode.m_MpegClockPolarity = AVL_DVBSx_MPCP_Rising;



    //This function should be called after demod initialized to set MPEG mode.
    r = AVL_DVBSx_IRx_SetMpegMode( ePortIndex, &sMpegMode,pAVLChip );
    if(AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Avl6211_IDVBSxRx_SetMpegMode failed! Err = %d \r\n",r));
        return (r);
    }


    AVL_DVBSx_IRx_SetMpegSerialPin( ePortIndex, pAVLChip, AVL_DVBSx_MPSP_DATA0);


    // Enable the MPEG output (this function call has no effect for the AVL2108LG and AVL2108LGa devices)
    r = AVL_DVBSx_IRx_DriveMpegOutput( ePortIndex, pAVLChip);
    
    //This function should be called after IBase initialized to initialize the tuner.  
    r = pTuner->m_pInitializeFunc(pTuner, ePortIndex);  // no need for RDA5815M

    if(AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Tuner Initialization failed! Err = %d \r\n",r));
        return (r);
    }
 
    //Setup Diseqc parameters for Diseqc initialization.
    sDiseqcPara.m_RxTimeout = AVL_DVBSx_DRT_150ms;
    sDiseqcPara.m_RxWaveForm = AVL_DVBSx_DWM_Normal;
    sDiseqcPara.m_ToneFrequency_kHz = Diseqc_Tone_Frequency;        
    sDiseqcPara.m_TXGap = AVL_DVBSx_DTXG_15ms;
    sDiseqcPara.m_TxWaveForm = AVL_DVBSx_DWM_Normal;

    //The Diseqc should be initialized if AVL6211 need to supply power to LNB. This function should be called after IBase initialized to initialize the Diseqc.
    r = AVL_DVBSx_IDiseqc_Initialize(ePortIndex, &sDiseqcPara, pAVLChip);
    if(AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Diseqc Initialization failed! Err = %d \r\n",r));
    }
    return (r);
}


AVL_DVBSx_ErrorCode Avl6211_LockSignal(AVL_uint32 u32Port,
                                 AVL_uint32 TunerFrequency,
                                 AVL_uint32 SignalSymbolRate,
                                 AVL_uint32 LNBFrequency,
                                 AVL_uint32 *pNewFrequency,
                                 struct AVL_DVBSx_Chip * pAvl6211Chip,
                                 struct AVL_Tuner * pTuner)
{    
    AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
    AVL_uint16 uiLockStatus=0;
    AVL_int16 iRFOff;
    AVL_uint32 uiTimeth; 
    AVL_uint16 uiCounter;
    struct AVL_DVBSx_Channel Channel; 
    
    //AVL_uint32 u32Freq = 0;
    //struct AVL_DVBSx_Chip * pAvl6211Chip = &Avl6211Chip;      
    //struct AVL_Tuner * pTuner =&Tuner;   

    AVL_uint32 u32FrequencyDValue = 0;
    
#if 0 //def Avl6211_DEBUG
    printf("\nAvl6211_LockSignal start !!!\n");
    printf("\n----------------CHIP---------------------------\n");
    printf("|    TunerFrequency     :   %d !\n",TunerFrequency);
    printf("|    SignalSymbolRate   :   %d !\n",SignalSymbolRate);
    printf("\n------------------------------------------------\n");
#endif
#if 0 //def Avl6211_DEBUG
    printf("\nAvl6211_LockSignal start !!!\n");
    printf("\n----------------CHIP---------------------------\n");
    printf("|    pAvl6211Chip->m_SlaveAddr=0x%x !\n",pAvl6211Chip->m_SlaveAddr);
    printf("|    pAvl6211Chip->m_StdBuffIndex=%d !\n",pAvl6211Chip->m_StdBuffIndex);
    printf("|    pAvl6211Chip->m_DemodFrequency_10kHz=%d !\n",pAvl6211Chip->m_DemodFrequency_10kHz);
    printf("|    pAvl6211Chip->m_FecFrequency_10kHz=%d !\n",pAvl6211Chip->m_FecFrequency_10kHz);
    printf("|    pAvl6211Chip->m_MpegFrequency_10kHz=%d !\n",pAvl6211Chip->m_MpegFrequency_10kHz);
    printf("\n------------------------------------------------\n");
#endif
 
    //Change the value defined by macro and go back here when we want to lock a new channel.
    pTuner->m_uiFrequency_100kHz = TunerFrequency/100;      
    //pTuner->m_uiLPF_100kHz = 340;
   
    AVL_DVBSx_ITuner_CalculateLPF((AVL_uint16)(SignalSymbolRate/10), pTuner);

#if 0 //def Avl6211_DEBUG
    printf("\n----------------Tuner---------------------------\n");
    printf("|    pTuner->m_uiSlaveAddress=0x%x !\n",pTuner->m_uiSlaveAddress);
    printf("|    pTuner->m_uiI2CBusClock_kHz=%d !\n",pTuner->m_uiI2CBusClock_kHz);
    printf("|    pTuner->m_uiFrequency_100kHz=%d !\n",pTuner->m_uiFrequency_100kHz);
    printf("|    pTuner->m_uiLPF_100kHz=%d !\n",pTuner->m_uiLPF_100kHz);
    printf("|    pTuner->m_pParameters=%d !\n",pTuner->m_pParameters);
    printf("\n------------------------------------------------\n");
#endif
    
    //Halt CPU to improve the tuner locking speed. It's the optional operation before locking tuner.
    r=CPU_Halt(u32Port, pAvl6211Chip);
    if(AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n CPU halt failed! Err = %d \r\n",r));
        return (r);
    }
    
    u32FrequencyDValue = (TunerFrequency <= LNBFrequency) ? (LNBFrequency - TunerFrequency) : (TunerFrequency - LNBFrequency);

    /*This function should be called after initialization to lock the tuner.*/
    r = pTuner->m_pLockFunc(u32Port, u32FrequencyDValue, SignalSymbolRate, pAvl6211Chip);
    if(AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Tuner lock function failed! Err = %d \r\n",r));
        return (r);
    } 

    AVL_DVBSx_IBSP_Delay(150);      //Wait a while for tuner to lock in certain frequency.


    /*This function should be called to check the lock status of the tuner.*/

   /* 
    r = pTuner->m_pGetLockStatusFunc(pTuner);
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Tuner GetLock failed! Err = %d \r\n",r));
        return (r);
    }
    */
   
    Channel.m_uiSymbolRate_Hz = SignalSymbolRate*1000;      //Change the value defined by macro when we want to lock a new channel.
    Channel.m_Flags = (CI_FLAG_IQ_NO_SWAPPED) << CI_FLAG_IQ_BIT;    //Normal IQ
    Channel.m_Flags |= (CI_FLAG_IQ_AUTO_BIT_AUTO) << CI_FLAG_IQ_AUTO_BIT;   //Enable automatic IQ swap detection
    Channel.m_Flags |= (CI_FLAG_DVBS2_UNDEF) << CI_FLAG_DVBS2_BIT;          //Enable automatic standard detection

#if 0 //def Avl6211_DEBUG
    printf("\n----------------Channel-------------------------\n");
    printf("|    Channel.m_uiSymbolRate_Hz=%d !\n",Channel.m_uiSymbolRate_Hz);
    printf("|    Channel.m_Flags=0x%x !\n",Channel.m_Flags);
    printf("\n------------------------------------------------\n");
#endif
    #if 1 // added by hjh
    AVL_DVBSx_IRx_SetChannelLockMode( &Channel, AVL_DVBSx_LOCK_MODE_ADAPTIVE);
    #endif
 
    //This function should be called after tuner locked to lock the channel.
    r = AVL_DVBSx_IRx_LockChannel( u32Port, &Channel, pAvl6211Chip);
    if (AVL_DVBSx_EC_OK != r)
    {
        Avl6211_ErrorPrint(("\r\n Avl6211_IDVBSxRx_LockChannel failed! Err = %d \r\n",r));
        return (r);
    }
    
    //Channel lock time increase while symbol rate decrease.Give the max waiting time for different symbolrates.
    if(SignalSymbolRate<5000)
    {
        uiTimeth=5000*2;       //The max waiting time is 5000ms,considering the IQ swapped status the time should be doubled.
    }
    else if(SignalSymbolRate<10000)
    {
        uiTimeth=600*2;        //The max waiting time is 600ms,considering the IQ swapped status the time should be doubled.
    }
    else
    {
        uiTimeth=250*2;        //The min waiting time is 250ms,considering the IQ swapped status the time should be doubled.
    } 
    uiCounter=uiTimeth/10;
    
    do
    {
        //This function should be called to check the lock status of the demod.
        r = AVL_DVBSx_IRx_GetLockStatus( u32Port, &uiLockStatus, pAvl6211Chip);
        if ((AVL_DVBSx_EC_OK == r)&&(1 == uiLockStatus))
        {
            break;
        }
        AVL_DVBSx_IBSP_Delay(10);    //Wait 10ms for demod to lock the channel.
    }while(--uiCounter);
    
    if(0==uiCounter)
    {
        //Avl6211_ErrorPrint(("\r\n Time out...Channel unlock !\n"));
        return (AVL_DVBSx_EC_TimeOut);
    }
    else
    {   
    #if 1 //def Avl6211_DEBUG
        printf("Channel locked !\nWait a few seconds to get the signal information...\n");
    #endif
    }
    //This function can be called to get the RF frequency offset after the channel locked.
    r=AVL_DVBSx_IRx_GetRFOffset( u32Port, &iRFOff, pAvl6211Chip);
    if (AVL_DVBSx_EC_OK != r)
    {
         Avl6211_ErrorPrint(("\r\n Get RFOffset --- Failed! Err = %d \r\n",r));
    }

    *pNewFrequency = TunerFrequency+(AVL_uint32)(iRFOff*100);
    
    return (r);
}
/*eof*/

