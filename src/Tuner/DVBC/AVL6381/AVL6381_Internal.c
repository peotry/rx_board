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


#include "user_defined_data_type.h"
#include "user_defined_function.h"
#include "AVL6381_Internal.h"

#if defined(AVL_INTERNAL_USE)
#include "AVL6381_fw_for_gui.h"
#else
#include "AVL6381_fw_DTMB.h"
#include "AVL6381_fw_DVBC.h"
#include "AVL6381_freeze_DTMB.h"
#endif

//Avl6381_PortIndex_t gsPortIndex;

AVL6381_Chip gsAVL6381Chip[AVL6381_PORT_NUM] = 
{
    /*PORT 1*/
    {
    .m_SlaveAddr = 0,
    .ucStartupMode = 0,
    .m_current_demod_mode = 0,
    .m_pInitialData = 0,
    .m_DemodFrequency_Hz = 0,
    .m_SDRAMFrequency_Hz = 0,
    .m_FECFrequency_Hz = 0,
    .m_RefFrequency_Hz = 0,
    . m_ADCFrequency_Hz = 0,

    .m_MPEG_Info= {
              .enumSerialParallel = AVL6381_MPEG_PARALLEL,
              .enumRisingFalling =  AVL6381_MPEG_FALLING,
              .enumConti = AVL6381_MPEG_CONTINUOUS_ENABLE
                   },
    .m_InputSignal_Info = {
             .usDemodI2CAddr = 0,
             .ueRefConfig = AVL6381_Xtal_30M,
             .ueDTMBInputPath = ADC_IF_I,
             .ueDVBCInputPath = ADC_IF_I,
             .uiDTMBIFFreqHz = 0,
             .uiDVBCIFFreqHz = 0,
             .uiDTMBSymbolRateHz = 0,
             .uiDVBCSymbolRateHz = 0
               },
    .m_ErrorStat_Info = {
             .m_ErrorStat_Mode =  ERROR_STAT_AUTO,
             .m_AutoErrorStat_Type = ERROR_STAT_TIME,
             .m_BER_Test_Pattern = AVL6381_TEST_LFSR_23,
             .m_BER_FB_Inversion = AVL6381_LFSR_FB_INVERTED,
             .m_LFSR_Sync = 0,
             .m_Time_Threshold_Ms = 0,
             .m_Number_Threshold_Byte = 0
                        }
    },
    /*PORT 2*/
    {
    .m_SlaveAddr = 0,
    .ucStartupMode = 0,
    .m_current_demod_mode = 0,
    .m_pInitialData = 0,
    .m_DemodFrequency_Hz = 0,
    .m_SDRAMFrequency_Hz = 0,
    .m_FECFrequency_Hz = 0,
    .m_RefFrequency_Hz = 0,
    . m_ADCFrequency_Hz = 0,

    .m_MPEG_Info= {
              .enumSerialParallel = AVL6381_MPEG_PARALLEL,
              .enumRisingFalling =  AVL6381_MPEG_FALLING,
              .enumConti = AVL6381_MPEG_CONTINUOUS_ENABLE
                   },
    .m_InputSignal_Info = {
             .usDemodI2CAddr = 0,
             .ueRefConfig = AVL6381_Xtal_30M,
             .ueDTMBInputPath = ADC_IF_I,
             .ueDVBCInputPath = ADC_IF_I,
             .uiDTMBIFFreqHz = 0,
             .uiDVBCIFFreqHz = 0,
             .uiDTMBSymbolRateHz = 0,
             .uiDVBCSymbolRateHz = 0
               },
    .m_ErrorStat_Info = {
             .m_ErrorStat_Mode =  ERROR_STAT_AUTO,
             .m_AutoErrorStat_Type = ERROR_STAT_TIME,
             .m_BER_Test_Pattern = AVL6381_TEST_LFSR_23,
             .m_BER_FB_Inversion = AVL6381_LFSR_FB_INVERTED,
             .m_LFSR_Sync = 0,
             .m_Time_Threshold_Ms = 0,
             .m_Number_Threshold_Byte = 0
                        }
    },
    /*PORT 3*/
    {
    .m_SlaveAddr = 0,
    .ucStartupMode = 0,
    .m_current_demod_mode = 0,
    .m_pInitialData = 0,
    .m_DemodFrequency_Hz = 0,
    .m_SDRAMFrequency_Hz = 0,
    .m_FECFrequency_Hz = 0,
    .m_RefFrequency_Hz = 0,
    . m_ADCFrequency_Hz = 0,

    .m_MPEG_Info= {
              .enumSerialParallel = AVL6381_MPEG_PARALLEL,
              .enumRisingFalling =  AVL6381_MPEG_FALLING,
              .enumConti = AVL6381_MPEG_CONTINUOUS_ENABLE
                   },
    .m_InputSignal_Info = {
             .usDemodI2CAddr = 0,
             .ueRefConfig = AVL6381_Xtal_30M,
             .ueDTMBInputPath = ADC_IF_I,
             .ueDVBCInputPath = ADC_IF_I,
             .uiDTMBIFFreqHz = 0,
             .uiDVBCIFFreqHz = 0,
             .uiDTMBSymbolRateHz = 0,
             .uiDVBCSymbolRateHz = 0
               },
    .m_ErrorStat_Info = {
             .m_ErrorStat_Mode =  ERROR_STAT_AUTO,
             .m_AutoErrorStat_Type = ERROR_STAT_TIME,
             .m_BER_Test_Pattern = AVL6381_TEST_LFSR_23,
             .m_BER_FB_Inversion = AVL6381_LFSR_FB_INVERTED,
             .m_LFSR_Sync = 0,
             .m_Time_Threshold_Ms = 0,
             .m_Number_Threshold_Byte = 0
                        }
    },
    /*PORT 4*/
    {
    .m_SlaveAddr = 0,
    .ucStartupMode = 0,
    .m_current_demod_mode = 0,
    .m_pInitialData = 0,
    .m_DemodFrequency_Hz = 0,
    .m_SDRAMFrequency_Hz = 0,
    .m_FECFrequency_Hz = 0,
    .m_RefFrequency_Hz = 0,
    . m_ADCFrequency_Hz = 0,

    .m_MPEG_Info= {
              .enumSerialParallel = AVL6381_MPEG_PARALLEL,
              .enumRisingFalling =  AVL6381_MPEG_FALLING,
              .enumConti = AVL6381_MPEG_CONTINUOUS_ENABLE
                   },
    .m_InputSignal_Info = {
             .usDemodI2CAddr = 0,
             .ueRefConfig = AVL6381_Xtal_30M,
             .ueDTMBInputPath = ADC_IF_I,
             .ueDVBCInputPath = ADC_IF_I,
             .uiDTMBIFFreqHz = 0,
             .uiDVBCIFFreqHz = 0,
             .uiDTMBSymbolRateHz = 0,
             .uiDVBCSymbolRateHz = 0
               },
    .m_ErrorStat_Info = {
             .m_ErrorStat_Mode =  ERROR_STAT_AUTO,
             .m_AutoErrorStat_Type = ERROR_STAT_TIME,
             .m_BER_Test_Pattern = AVL6381_TEST_LFSR_23,
             .m_BER_FB_Inversion = AVL6381_LFSR_FB_INVERTED,
             .m_LFSR_Sync = 0,
             .m_Time_Threshold_Ms = 0,
             .m_Number_Threshold_Byte = 0
                        }
    }
};

AVL6381_ErrorStats gsAVL6381ErrorStat[AVL6381_PORT_NUM] = 
{
    {0,0,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},0,0},
    {0,0,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},0,0},    
    {0,0,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},0,0},
    {0,0,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},0,0}
};

AVL6381_PLL_Conf gAVL6381PLLConfig[AVL6381_PORT_NUM][8] = 
{
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,50, 3,  5, 300000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  30000000, 2,44, 3,  6, 220000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  16000000, 1,42, 3,  5, 268800000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  16000000, 1,42, 3,  6, 224000000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  24000000, 2,75, 3,  6, 300000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  24000000, 2,55, 3,  6, 220000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  27000000, 3,74, 3,  5, 266400000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000},
        {  27000000, 3,74, 3,  6, 222000000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000}
    },
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,50, 3,  5, 300000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  30000000, 2,44, 3,  6, 220000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  16000000, 1,42, 3,  5, 268800000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  16000000, 1,42, 3,  6, 224000000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  24000000, 2,75, 3,  6, 300000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  24000000, 2,55, 3,  6, 220000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  27000000, 3,74, 3,  5, 266400000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000},
        {  27000000, 3,74, 3,  6, 222000000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000}
    },
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,50, 3,  5, 300000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  30000000, 2,44, 3,  6, 220000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  16000000, 1,42, 3,  5, 268800000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  16000000, 1,42, 3,  6, 224000000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  24000000, 2,75, 3,  6, 300000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  24000000, 2,55, 3,  6, 220000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  27000000, 3,74, 3,  5, 266400000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000},
        {  27000000, 3,74, 3,  6, 222000000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000}
    },
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,50, 3,  5, 300000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  30000000, 2,44, 3,  6, 220000000,1,34, 3,  12,  25, 32,     0,        110,   15,  170000000,81600000,63750000},
        {  16000000, 1,42, 3,  5, 268800000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  16000000, 1,42, 3,  6, 224000000,1,54, 3,  10,  24, 27,     1,        110,   15,  172800000,72000000,64000000},
        {  24000000, 2,75, 3,  6, 300000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  24000000, 2,55, 3,  6, 220000000,1,35, 3,  10,  21, 28,     0,        110,   15,  168000000,80000000,60000000},
        {  27000000, 3,74, 3,  5, 266400000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000},
        {  27000000, 3,74, 3,  6, 222000000,1,32, 3,  10,  24, 27,     0,        110,   15,  172800000,72000000,64000000}
    }
    
};

AVL6381_PLL_Conf gAVL6381SleepPLLConfig[AVL6381_PORT_NUM][4] =
{
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,44, 3,  12,110000000,1,34, 3,  24,  32, 32,     0,        110,   15,  85000000,63750000,63750000},
        {  16000000, 1,42, 3,  12,112000000,1,54, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000},
        {  24000000, 2,55, 3,  12,110000000,1,35, 3,  20,  28, 28,     0,        110,   15,  84000000,60000000,60000000},
        {  27000000, 3,74, 3,  12,111000000,1,32, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000}
    },
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,44, 3,  12,110000000,1,34, 3,  24,  32, 32,     0,        110,   15,  85000000,63750000,63750000},
        {  16000000, 1,42, 3,  12,112000000,1,54, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000},
        {  24000000, 2,55, 3,  12,110000000,1,35, 3,  20,  28, 28,     0,        110,   15,  84000000,60000000,60000000},
        {  27000000, 3,74, 3,  12,111000000,1,32, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000}
    },
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,44, 3,  12,110000000,1,34, 3,  24,  32, 32,     0,        110,   15,  85000000,63750000,63750000},
        {  16000000, 1,42, 3,  12,112000000,1,54, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000},
        {  24000000, 2,55, 3,  12,110000000,1,35, 3,  20,  28, 28,     0,        110,   15,  84000000,60000000,60000000},
        {  27000000, 3,74, 3,  12,111000000,1,32, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000}
    },
    {
        //| Ref Clk |R|F|bwadj|OD|  Demod  |R|F|bwadj|OD1|OD2|OD3|use pll adc|DLL out|DLL R|   FEC   |  SDRAM  |   ADC  |
        {  30000000, 2,44, 3,  12,110000000,1,34, 3,  24,  32, 32,     0,        110,   15,  85000000,63750000,63750000},
        {  16000000, 1,42, 3,  12,112000000,1,54, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000},
        {  24000000, 2,55, 3,  12,110000000,1,35, 3,  20,  28, 28,     0,        110,   15,  84000000,60000000,60000000},
        {  27000000, 3,74, 3,  12,111000000,1,32, 3,  20,  27, 27,     0,        110,   15,  86400000,64000000,64000000}
    }
    
};


AVL6381_SDRAM_Conf gAVL6381SDRAMConfig[AVL6381_PORT_NUM] = 
{
    {
        63, 84, 84, 1, 42, 21, 42, 2  
    },
    {
        63, 84, 84, 1, 42, 21, 42, 2  
    },
    {
        63, 84, 84, 1, 42, 21, 42, 2  
    },
    {
        63, 84, 84, 1, 42, 21, 42, 2  
    }
};

AVL_semaphore gAVL6381I2CSem[AVL6381_PORT_NUM];

AVL6381_AGCPola geAVL6381AGCPola[AVL6381_PORT_NUM] = {AGC_NORMAL,AGC_NORMAL,AGC_NORMAL,AGC_NORMAL};

AVL6381_MPEG_SerialDataPin geAVL6381SerialPin[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_SERIAL_D0,AVL6381_MPEG_SERIAL_D0, AVL6381_MPEG_SERIAL_D0,AVL6381_MPEG_SERIAL_D0};
        
AVL6381_MPEG_SERIAL_ORDER geAVL6381SerialOrder[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_SERIAL_ORDER_MSB,AVL6381_MPEG_SERIAL_ORDER_MSB,AVL6381_MPEG_SERIAL_ORDER_MSB,AVL6381_MPEG_SERIAL_ORDER_MSB};
        
AVL6381_MPEG_SERIAL_SYNC geAVL6381SerialSync[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_SERIAL_SYNC_1_PULSE,AVL6381_MPEG_SERIAL_SYNC_1_PULSE,AVL6381_MPEG_SERIAL_SYNC_1_PULSE,AVL6381_MPEG_SERIAL_SYNC_1_PULSE};
        
AVL6381_MPEG_ErrorBit geAVL6381ErrorBit[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_ERROR_BIT_DISABLE,AVL6381_MPEG_ERROR_BIT_DISABLE,AVL6381_MPEG_ERROR_BIT_DISABLE,AVL6381_MPEG_ERROR_BIT_DISABLE};
        
AVL6381_MPEG_ErrorBitPola geAVL6381ErrorPola[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_ERROR_HIGH,AVL6381_MPEG_ERROR_HIGH,AVL6381_MPEG_ERROR_HIGH,AVL6381_MPEG_ERROR_HIGH};
        
AVL6381_MPEG_ValidPola geAVL6381ValidPola[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_VALID_HIGH,AVL6381_MPEG_VALID_HIGH,AVL6381_MPEG_VALID_HIGH,AVL6381_MPEG_VALID_HIGH};
        
AVL6381_MPEG_Packet_Len geAVL6381PacketLen[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_188,AVL6381_MPEG_188,AVL6381_MPEG_188,AVL6381_MPEG_188};
        
AVL6381_MPEG_PARALLEL_ORDER geAVL6381ParallelOrder[AVL6381_PORT_NUM] 
        = {AVL6381_MPEG_PARALLEL_ORDER_NORMAL,AVL6381_MPEG_PARALLEL_ORDER_NORMAL,AVL6381_MPEG_PARALLEL_ORDER_NORMAL,AVL6381_MPEG_PARALLEL_ORDER_NORMAL};
        
AVL_uint32 guiDTMBSerialMPEGContinuousHz[AVL6381_PORT_NUM] 
        = {42500000,42500000,42500000,42500000};//42.5M
        
AVL_uint32 guiDVBCSerialMPEGContinuousHz[AVL6381_PORT_NUM] 
        = {85000000,85000000,85000000,85000000};//85M
        
AVL_uint32 guiParallelMPEGContinuousHz[AVL6381_PORT_NUM] 
        = {8500000,85000000,8500000,85000000};//8.5M
        
AVL_uint32 guiDTMBSymbolRateSps[AVL6381_PORT_NUM] 
        = {7560*1000,7560*1000,7560*1000,7560*1000};//DTMB symbol rate, default value 7.56Msps

AVL_uchar gucDisableAGC[AVL6381_PORT_NUM] = {0,0,0,0};

AVL_uchar gucDisableMPEGOutput[AVL6381_PORT_NUM] = {0,0,0,0};

static AVL_uchar gucAVL6381CustomizeFwData[AVL6381_PORT_NUM] = {0,0,0,0};

AVL_uchar gAVL6381freezeflag[AVL6381_PORT_NUM] = {0,0,0,0};


#if 0
AVL6381_ErrorCode AutoSetPortIndex_6381(Avl6381_PortIndex_t ePortIndex)
{
    gsPortIndex = ePortIndex;

    return AVL6381_EC_OK;
}
#endif

AVL6381_ErrorCode AutoLockChannel_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_AutoLockChannel_6381(ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_AutoLockChannel_6381(ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode SetMpegMode_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_SetMpegMode_6381(ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_SetMpegMode_6381(ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode EnableMpegOutput_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    gucDisableMPEGOutput[ePortIndex] = 0;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_mpeg_bus_tri_enb, 0xfff);

    return r;
}

AVL6381_ErrorCode DisableMpegOutput_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    gucDisableMPEGOutput[ePortIndex] = 1;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_mpeg_bus_tri_enb, 0);

    return r;
}

AVL6381_ErrorCode DisableMpegContinuousMode_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumConti = AVL6381_MPEG_CONTINUOUS_DISABLE;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_DisableMpegContinuousMode_6381(ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_DisableMpegContinuousMode_6381(ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode EnableMpegContinuousMode_6381(AVL_uint32 uiClockFreq_Hz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        guiDTMBSerialMPEGContinuousHz[ePortIndex] = uiClockFreq_Hz;
        r = DTMB_EnableMpegContinuousMode_6381(uiClockFreq_Hz,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        guiDVBCSerialMPEGContinuousHz[ePortIndex] = uiClockFreq_Hz;
        r = DVBC_EnableMpegContinuousMode_6381(uiClockFreq_Hz,ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode SetMpegSerialPin_6381(AVL6381_MPEG_SerialDataPin enumSerialPin,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381SerialPin[ePortIndex] = enumSerialPin;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegSerialPin_6381(enumSerialPin,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegSerialPin_6381(enumSerialPin,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetMpegParalleOrder_6381(AVL6381_MPEG_PARALLEL_ORDER enumParallelOrder,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381ParallelOrder[ePortIndex] = enumParallelOrder;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegParallelOrder_6381(enumParallelOrder,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegParallelOrder_6381(enumParallelOrder,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetMpegSerialOrder_6381(AVL6381_MPEG_SERIAL_ORDER enumSerialOrder,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381SerialOrder[ePortIndex] = enumSerialOrder;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegSerialOrder_6381(enumSerialOrder,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegSerialOrder_6381(enumSerialOrder,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetMpegSerialSyncPulse_6381(AVL6381_MPEG_SERIAL_SYNC enumSerialSyncPulse,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381SerialSync[ePortIndex] = enumSerialSyncPulse;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegSerialSyncPulse_6381(enumSerialSyncPulse,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegSerialSyncPulse_6381(enumSerialSyncPulse,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetMpegParallelPhase_6381(AVL6381_MPEG_PARALLEL_PHASE enumParallelPhase,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegParallelPhase_6381(enumParallelPhase,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegParallelPhase_6381(enumParallelPhase,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetMpegErrorBit_6381(AVL6381_MPEG_ErrorBit enumErrorBit,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381ErrorBit[ePortIndex] = enumErrorBit;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegErrorBit_6381(enumErrorBit,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegErrorBit_6381(enumErrorBit,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetMpegErrorPola_6381(AVL6381_MPEG_ErrorBitPola enumErrorPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381ErrorPola[ePortIndex] = enumErrorPola;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegErrorPola_6381(enumErrorPola,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegErrorPola_6381(enumErrorPola,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetMpegValidPola_6381(AVL6381_MPEG_ValidPola enumValidPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381ValidPola[ePortIndex] = enumValidPola;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegValidPola_6381(enumValidPola,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegValidPola_6381(enumValidPola,ePortIndex);

    }

    return r;
}

AVL6381_ErrorCode SetAGCPola_6381(AVL6381_AGCPola enumAGCPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    
    geAVL6381AGCPola[ePortIndex] = enumAGCPola;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_SetAGCPola_6381(enumAGCPola,ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_SetAGCPola_6381(enumAGCPola,ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode EnableAGC_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    gucDisableAGC[ePortIndex] = 0;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_rfagc_tri_enb, 1);

    return r;
}

AVL6381_ErrorCode DisableAGC_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    gucDisableAGC[ePortIndex] = 1;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_rfagc_tri_enb, 0);

    return r;
}

AVL6381_ErrorCode GetSpectrumInversion_6381(AVL6381_SpectrumInversion *pSpectrumInversion,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_GetSpectrumInversion_6381(pSpectrumInversion,ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_GetSpectrumInversion_6381(pSpectrumInversion,ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode GetScatterData_6381(AVL_puint32 puiIQ_Data, AVL_puint16 puiSize,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_GetScatterData_6381(puiIQ_Data, puiSize,ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_GetScatterData_6381(puiIQ_Data, puiSize,ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode GetVersion_6381(AVL6381_VerInfo * pVerInfo,Avl6381_PortIndex_t ePortIndex)
{
    AVL_uint32 uiTemp;
    AVL_uchar ucBuff[4];
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r =  I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_rom_ver, &uiTemp);

    if( AVL6381_EC_OK == r )
    {
        Chunk32_6381(uiTemp, ucBuff);
        pVerInfo->m_Chip.m_Major = ucBuff[0];
        pVerInfo->m_Chip.m_Minor = ucBuff[1];
        pVerInfo->m_Chip.m_Build = ucBuff[2];
        pVerInfo->m_Chip.m_Build = ((AVL_uint16)((pVerInfo->m_Chip.m_Build)<<8)) + ucBuff[3];
        if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
        {
            r |=  I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_patch_ver_iaddr, &uiTemp);
        }
        else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
        {
            r |=  I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DVBC_patch_ver_iaddr, &uiTemp);
        }
        if( AVL6381_EC_OK == r )
        {
            Chunk32_6381(uiTemp, ucBuff);
            pVerInfo->m_Patch.m_Major = ucBuff[0];
            pVerInfo->m_Patch.m_Minor = ucBuff[1];
            pVerInfo->m_Patch.m_Build = ucBuff[2];
            pVerInfo->m_Patch.m_Build = ((AVL_uint16)((pVerInfo->m_Patch.m_Build)<<8)) + ucBuff[3];

            pVerInfo->m_API.m_Major = AVL6381_API_VER_MAJOR;
            pVerInfo->m_API.m_Minor = AVL6381_API_VER_MINOR;
            pVerInfo->m_API.m_Build = AVL6381_API_VER_BUILD;
        }
    }
    
    return r;
}

AVL6381_ErrorCode InitSemaphore_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = AVL6381_IBSP_InitSemaphore(&(gsAVL6381Chip[ePortIndex].m_semRx));
    r |= I2C_Initialize_6381(ePortIndex); // there is internal protection to assure the II2C will be initialized only once.

    return r;
}

AVL6381_ErrorCode IRx_Initialize_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_InitRx_6381(ePortIndex);
        r |= DTMB_InitADC_6381(ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_InitRx_6381(ePortIndex);
        r |= DVBC_InitADC_6381(ePortIndex);
    }

    r |= InitSDRAM_6381(ePortIndex);

    return r;
}

void WVAdd32To64(AVL6381_uint64 *pSum, AVL_uint32 uiAddend)
{
    AVL_uint32 uiTemp;

    uiTemp = pSum->m_LowWord;
    pSum->m_LowWord += uiAddend;
    pSum->m_LowWord &= 0xFFFFFFFF;

    if (pSum->m_LowWord < uiTemp)
    {
        pSum->m_HighWord++;
    }
}

AVL_uint32 WVDivide64(AVL6381_uint64 y, AVL6381_uint64 x)
{
    AVL_uint32 uFlag = 0x0;
    AVL_uint32 uQuto = 0x0;
    AVL_uint32 i = 0;
    AVL_uint32 dividend_H = x.m_HighWord;
    AVL_uint32 dividend_L = x.m_LowWord;
    AVL_uint32 divisor_H = y.m_HighWord; 
    AVL_uint32 divisor_L = y.m_LowWord; 

    if(((divisor_H == 0x0) && (divisor_L == 0x0)) || (dividend_H/divisor_L))
    {
        return 0;
    }
    else if((divisor_H == 0x0)&&(dividend_H == 0x0))
    {
        return  dividend_L / divisor_L;
    }
    else 
    {  
        if(divisor_H != 0)
        {
            while(divisor_H)
            {
                dividend_L /= 2;
                if(dividend_H % 2)
                {    
                    dividend_L += 0x80000000;
                }
                dividend_H /= 2;

                divisor_L /= 2;
                if(divisor_H %2)
                {    
                    divisor_L += 0x80000000;
                }
                divisor_H /= 2;
            }
        }   
        for   (i = 0; i <= 31; i++) 
        { 

            uFlag = (AVL_int32)dividend_H >> 31;

            dividend_H = (dividend_H << 1)|(dividend_L >> 31);
            dividend_L <<= 1; 

            uQuto <<= 1;
            if((dividend_H|uFlag) >= divisor_L)
            { 
                dividend_H -= divisor_L;   
                uQuto++;   
            }   
        } 
        return uQuto;
    }
}

AVL_uint32 GreaterThanOrEqual64(AVL6381_uint64 a, AVL6381_uint64 b)
{
    AVL_uint32 result;

    result = 0;

    if((a.m_HighWord == b.m_HighWord) && (a.m_LowWord == b.m_LowWord))
    {
        result = 1;
    }
    if(a.m_HighWord > b.m_HighWord)
    {
        result = 1;
    }
    else if(a.m_HighWord == b.m_HighWord)
    {
        if(a.m_LowWord > b.m_LowWord)
        {
            result = 1;
        }
    }

    return result;
}

void Subtract64(AVL6381_uint64 *pA, AVL6381_uint64 b)
{
    struct AVL6381_uint64 a;
    struct AVL6381_uint64 temp;

    a.m_HighWord = pA->m_HighWord;
    a.m_LowWord = pA->m_LowWord;

    temp.m_HighWord = a.m_HighWord - b.m_HighWord;
    if(a.m_LowWord >= b.m_LowWord)
    {
        temp.m_LowWord = a.m_LowWord - b.m_LowWord;
    }
    else
    {
        temp.m_LowWord = b.m_LowWord - a.m_LowWord;
        temp.m_HighWord >>= 1;
    }

    pA->m_HighWord = temp.m_HighWord;
    pA->m_LowWord = temp.m_LowWord;
}

void WVMultiply32(AVL6381_uint64 *pDst, AVL_uint32 m1, AVL_uint32 m2)
{
    pDst->m_LowWord = (m1 & 0xFFFF) * (m2 & 0xFFFF);
    pDst->m_HighWord = 0;

    WVAddScaled32To64(pDst, (m1 >> 16) * (m2 & 0xFFFF));
    WVAddScaled32To64(pDst, (m2 >> 16) * (m1 & 0xFFFF));

    pDst->m_HighWord += (m1 >> 16) * (m2 >> 16);
}

void WVAddScaled32To64(AVL6381_uint64 *pDst, AVL_uint32 a)
{
    AVL_uint32 saved;

    saved = pDst->m_LowWord;
    pDst->m_LowWord += (a << 16);

    pDst->m_LowWord &= 0xFFFFFFFF;
    pDst->m_HighWord += ((pDst->m_LowWord < saved) ? 1 : 0) + (a >> 16);
}

void Multiply32By16(AVL6381_int64 *pY, AVL_int32 a, AVL_int16 b)
{
    AVL_uint16 uiLswA;
    AVL_int16 iMswA;
    AVL_int32 iMswAB;
    AVL_uint32 uiLswAB;
    struct AVL6381_int64 iTemp;

    // (mswA * 2^16 + lswA) * b = (mswA * b * 2^16) + (lswA * b)
    // Compute (mswA * b * 2^16):
    uiLswA = (AVL_uint16)(a & 0x0000FFFF);
    iMswA = (AVL_int16)((a >> 16) & 0x0000FFFF);
    iMswAB = (AVL_int32)(iMswA * b);
    iTemp.m_HighWord = iMswAB;
    iTemp.m_HighWord >>= 16;
    iTemp.m_LowWord = (AVL_uint32)(iMswAB << 16);
    // Compute (lswA * b):
    uiLswAB = (AVL_uint32)(uiLswA * b);

    // Add the results
    WVAdd32To64((struct AVL6381_uint64 *)&iTemp, uiLswAB);

    pY->m_HighWord = iTemp.m_HighWord;
    pY->m_LowWord = iTemp.m_LowWord;
}

AVL_uint32 ConvertNsToSDRAM_Clocks(AVL_uint32 SDRAM_ClkFrequency_Hz, AVL_uint16 Value_ns)
{
    AVL_uint32 SDRAM_Freq_100kHz;
    AVL_uint32 temp;
    AVL_uint32 uiValue_clks;

    SDRAM_Freq_100kHz = (SDRAM_ClkFrequency_Hz/100000);
    temp = (SDRAM_Freq_100kHz * 419 * Value_ns) - 41943;
    uiValue_clks = (temp >> 22);
    if((temp & 0x003FFFFF) != 0)
    {
        uiValue_clks++;
    }

    return uiValue_clks;
}

AVL6381_ErrorCode SetPLL_6381(AVL6381_PLL_Conf *pPLL_Conf,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,  hw_AVL6381_pll_demod_clkr, pPLL_Conf->m_Demod_PLL_DivR-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_demod_clkf, pPLL_Conf->m_Demod_PLL_DivF-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_demod_bwadj, pPLL_Conf->m_Demod_PLL_bwadj);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_demod_od, pPLL_Conf->m_Demod_PLL_od-1);

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_clkr, pPLL_Conf->m_Aux_PLL_DivR-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_clkf, pPLL_Conf->m_Aux_PLL_DivF-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_bwadj, pPLL_Conf->m_Aux_PLL_bwadj);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_od, pPLL_Conf->m_Aux_PLL_od-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_od2, pPLL_Conf->m_Aux_PLL_od2-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_od3, pPLL_Conf->m_Aux_PLL_od3-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_clock_reset , 0);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_clock_reset, 1);
    AVL6381_IBSP_Delay(1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_out_phase, pPLL_Conf->m_DDLL_out_phase);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_rd_phase, pPLL_Conf->m_DDLL_read_phase);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_deglitch_mode, 1);//actually, this is used to enable DLL normal mode
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_init, 1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_init, 0);

    if(pPLL_Conf->m_use_pll_adc == 0)
    {
        gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz = pPLL_Conf->m_RefFrequency_Hz;
    }
    else
    {
        gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz = pPLL_Conf->m_ADC_Frequency_Hz/2;
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_adc_clock_sel, 0xc);
    }

    gsAVL6381Chip[ePortIndex].m_RefFrequency_Hz = pPLL_Conf->m_RefFrequency_Hz;
    gsAVL6381Chip[ePortIndex].m_DemodFrequency_Hz = pPLL_Conf->m_DemodFrequency_Hz;
    gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz = pPLL_Conf->m_SDRAM_Frequency_Hz;
    gsAVL6381Chip[ePortIndex].m_FECFrequency_Hz = pPLL_Conf->m_FEC_Frequency_Hz;

    return r;
}

AVL6381_ErrorCode GetRxOP_Status_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiCmd = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_fw_command_iaddr, &uiCmd);

    if( AVL6381_EC_OK == r )
    {
        if( 0 != uiCmd )
        {
            r = AVL6381_EC_RUNNING;
        }
    }

    return(r);
}

AVL6381_ErrorCode IBase_Initialize_6381(AVL6381_PLL_Conf *pPLL_Conf,Avl6381_PortIndex_t ePortIndex)
{
    AVL_uint32 uiSize = 0;
    AVL_uint32 i1 = 0;
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_puchar pInitialData;

    r = I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_c306_top_srst, 1);

    // Configure the PLL
    r |= SetPLL_6381(pPLL_Conf, ePortIndex);

    if(gucAVL6381CustomizeFwData[ePortIndex] == 0)
    {
        if(gAVL6381freezeflag[ePortIndex] == 1)
        {
            pInitialData = AVL6381_freezeData_DTMB;
        }else{
            if(gsAVL6381Chip[ePortIndex].ucStartupMode==AVL_DEMOD_MODE_DTMB)
            {
                pInitialData = AVL6381_FwData_DTMB; 
            }
            else
            {
                pInitialData = AVL6381_FwData_DVBC;
            }
        }
    }
    else
    {
        pInitialData = gsAVL6381Chip[ePortIndex].m_pInitialData;
    }

    if( AVL6381_EC_OK == r )
    {
        //load patch into the chip
        i1 = 0;
        
        i1 += 4;//skip the firmware version
        
        while(1)
        {
            uiSize = DeChunk32_6381(pInitialData+i1);
            if(0 == uiSize)
            {
                break;
            }
            // Increment index past the size field (4 bytes) and past first byte of address field (always zero)
            i1 += 5;

            // Send the address (3 bytes) + data (uiSize bytes)
            r |= I2C_Write_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), pInitialData+i1, (AVL_uint16)(uiSize+3));

            // Increment to the next burst of data to send
            i1 += (3 + uiSize);
        }

        //
        if(gAVL6381freezeflag[ePortIndex] == 1)
        {
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x0228, 0x00280000);
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x022c, 0x002d0008);
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x0230, 0x0028cb00);
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x0234, 0x002f2c08);
        }
        else
        {
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x0228, 0x00280000);
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x022c, 0x002d4000);
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x0230, 0x0028a000);
            r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x0234, 0x002e8000);
        }
        r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), 0x0225, 1);
        if(gsAVL6381Chip[ePortIndex].ucStartupMode==AVL_DEMOD_MODE_DTMB){
            //
            r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_demod_mode_caddr, 0);
        }else{
            r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_demod_mode_caddr, 1);
        }
        

        //startup 
        r |= I2C_Write16_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_mode_configuration_saddr, gsAVL6381Chip[ePortIndex].ucStartupMode);
        r |= I2C_Write16_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_mode_switch_saddr, 0 );
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_core_ready_word_iaddr, 0x00000000);
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_c306_top_srst, 0);
    }

    return r;
}

AVL6381_ErrorCode InitADC_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_InitADC_6381(ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_InitADC_6381(ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode InitSDRAM_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 latencyValue = 0;
    AVL_uint32 uiTemp = 0;

    uiTemp = (((AVL_uint32)7) << 16);
    uiTemp |= (((AVL_uint32)10) << 8);
    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_sdram_config_iaddr, uiTemp);
        
    latencyValue = ConvertNsToSDRAM_Clocks(gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz, gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_TRC);
    uiTemp = ((latencyValue - 1) << 24);
    latencyValue = ConvertNsToSDRAM_Clocks(gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz, gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_TXSR);
    uiTemp |= ((latencyValue - 1) << 16);
    latencyValue = ConvertNsToSDRAM_Clocks(gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz, gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_TRCAR);
    uiTemp |= ((latencyValue - 1) << 8);
    uiTemp |= (AVL_uint32)(gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_TWR - 1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_sdram_timing_config_0_iaddr, uiTemp);
    latencyValue = ConvertNsToSDRAM_Clocks(gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz, gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_TRP);
    uiTemp = ((latencyValue - 1) << 24);
    latencyValue = ConvertNsToSDRAM_Clocks(gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz, gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_TRCD);
    uiTemp |= ((latencyValue - 1) << 16);
    latencyValue = ConvertNsToSDRAM_Clocks(gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz, gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_TRAS);
    uiTemp |= ((latencyValue - 1) << 8);
    uiTemp |= (AVL_uint32)(gAVL6381SDRAMConfig[ePortIndex].m_SDRAM_CAS_Latency - 1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_sdram_timing_config_1_iaddr, uiTemp); 

    //send command to init sdram
    r |= SendRxOP_6381(OP_RX_INIT_SDRAM, ePortIndex);

    return r;
}

AVL6381_ErrorCode TestSDRAM_6381(AVL_puint32 puiTestResult, AVL_puint32 puiTestPattern,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    const AVL_uint16 uiTimeDelay = 100;
    const AVL_uint16 uiMaxRetries = 200;
    AVL_uint32 i=0;

    r = SendRxOP_6381(OP_RX_SDRAM_TEST,ePortIndex);
    if(AVL6381_EC_OK == r )
    {

        while (AVL6381_EC_OK != GetRxOP_Status_6381(ePortIndex))
        {
            if (uiMaxRetries < i++)
            {
                r |= AVL6381_EC_RUNNING;
                break;
            }
            AVL6381_IBSP_Delay(uiTimeDelay);
        }
        
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_sdram_test_return_iaddr, puiTestPattern);
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_sdram_test_result_iaddr, puiTestResult);
    }

    return r;
}

AVL6381_ErrorCode SendRxOP_6381(AVL_uchar ucOpCmd,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pucBuff[4];
    AVL_uint32 uiTemp;
    const AVL_uint16 uiTimeDelay = 10;
    const AVL_uint16 uiMaxRetries = 20;
    AVL_uint32 i = 0;

   // r = AVL6381_IBSP_WaitSemaphore(&(gsAVL6381Chip[ePortIndex].m_semRx));

    while (AVL6381_EC_OK != GetRxOP_Status_6381(ePortIndex))
    {
        if (uiMaxRetries < i++)
        {
            r |= AVL6381_EC_RUNNING;
            break;
        }
        AVL6381_IBSP_Delay(uiTimeDelay);
    }
    if( AVL6381_EC_OK == r )
    {
        pucBuff[0] = ucOpCmd;
        pucBuff[1] = 0;
        pucBuff[2] = 0;
        pucBuff[3] = 0;
        uiTemp = DeChunk32_6381(pucBuff);
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_fw_command_iaddr, uiTemp);
    }

    r |= AVL6381_IBSP_ReleaseSemaphore(&(gsAVL6381Chip[ePortIndex].m_semRx));

    return r;
}

AVL6381_ErrorCode Halt_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = SendRxOP_6381(OP_RX_HALT,ePortIndex);
    //This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.
    r |= AVL6381_IBSP_Delay(2);
    
    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_Halt_6381(ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_Halt_6381(ePortIndex);
    }
    
    return r;
}

AVL6381_ErrorCode GetRunningLevel_6381(AVL6381_RunningLevel *pRunningLevel,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_GetRunningLevel_6381(pRunningLevel, ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_GetRunningLevel_6381(pRunningLevel, ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode TunerI2C_Initialize_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 bit_rpt_divider = 0;
    AVL_uint32 uiTemp = 0;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_i2c_srst, 1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl, 0x6);
    r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_i2c_cntrl, &uiTemp);
    uiTemp = (uiTemp&0xFFFFFFFE);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_i2c_cntrl, uiTemp);
    bit_rpt_divider = (0x2A)*(gsAVL6381Chip[ePortIndex].m_DemodFrequency_Hz/1000)/(240*1000);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_hw_i2c_bit_rpt_clk_div, bit_rpt_divider);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_tuner_i2c_srst, 0);

    return r;
}

AVL6381_ErrorCode I2C_Initialize_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    static AVL_uchar gAVL6381I2CSem_inited[AVL6381_PORT_NUM] = {0};

    if( 0 == gAVL6381I2CSem_inited[ePortIndex] )
    {
        gAVL6381I2CSem_inited[ePortIndex] = 1;
        r = AVL6381_IBSP_InitSemaphore(&gAVL6381I2CSem[ePortIndex]);
    }
    
    return r;
}

AVL6381_ErrorCode I2C_Read_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiOffset, AVL_puchar pucBuff, AVL_uint16 uiSize)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pucBuffTemp[3];
    AVL_uint16 ui1 = 0, ui2 = 0;
    AVL_uint16 iSize = 0;

    //r = AVL6381_IBSP_WaitSemaphore(&(gAVL6381I2CSem[ePortIndex]));
    if( AVL6381_EC_OK == r )
    {
        ChunkAddr_6381(uiOffset, pucBuffTemp);
        ui1 = 3;
        r = AVL6381_IBSP_I2C_Write(ePortIndex, uiSlaveAddr, pucBuffTemp, &ui1);  
        if( AVL6381_EC_OK == r )
        {
            iSize = uiSize;
            ui2 = 0;
            while( iSize > MAX_II2C_READ_SIZE )
            {
                ui1 = MAX_II2C_READ_SIZE;
                r |= AVL6381_IBSP_I2C_Read(ePortIndex, uiSlaveAddr, pucBuff+ui2, &ui1);
                ui2 += MAX_II2C_READ_SIZE;
                iSize -= MAX_II2C_READ_SIZE;
            }

            if( 0 != iSize )
            {
                r |= AVL6381_IBSP_I2C_Read(ePortIndex, uiSlaveAddr, pucBuff+ui2, &iSize);
            }
        }
     }
   // r |= AVL6381_IBSP_ReleaseSemaphore(&(gAVL6381I2CSem[ePortIndex])); 
    
    return r;
}

AVL6381_ErrorCode I2C_Read8_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puchar puiData)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar Data = 0;

    r = I2C_Read_6381(ePortIndex, uiSlaveAddr, uiAddr, &Data, 1);
    if( AVL6381_EC_OK == r )
    {
        *puiData = Data;
    }
    
    return r;
}

AVL6381_ErrorCode I2C_Read16_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint16 puiData)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pBuff[2];

    r = I2C_Read_6381(ePortIndex, uiSlaveAddr, uiAddr, pBuff, 2);
    if( AVL6381_EC_OK == r )
    {
        *puiData = DeChunk16_6381(pBuff);
    }
    
    return r;
}

AVL6381_ErrorCode I2C_Read32_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint32 puiData)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pBuff[4];

    r = I2C_Read_6381(ePortIndex, uiSlaveAddr, uiAddr, pBuff, 4);
    if( AVL6381_EC_OK == r )
    {
        *puiData = DeChunk32_6381(pBuff);
    }
    
    return r;
}

AVL6381_ErrorCode I2C_Write_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pucBuffTemp[5];
    AVL_uint16 ui1 = 0, ui2 = 0, uTemp = 0;
    AVL_uint16 iSize = 0;
    AVL_uint32 uAddr = 0;
    
    if( uiSize<3 )
    {
        return(AVL6381_EC_GENERAL_FAIL);     //at least 3 bytes
    }

    uiSize -= 3;            //actual data size
    //r = AVL6381_IBSP_WaitSemaphore(&(gAVL6381I2CSem[ePortIndex]));
    if( AVL6381_EC_OK == r )
    {
        //dump address
        uAddr = pucBuff[0];
        uAddr = uAddr<<8;
        uAddr += pucBuff[1];
        uAddr = uAddr<<8;
        uAddr += pucBuff[2];

        iSize = uiSize;

        uTemp = (MAX_II2C_WRITE_SIZE-3) & 0xfffe; //how many bytes data we can transfer every time

        ui2 = 0;
        while( iSize > uTemp )
        {
            ui1 = uTemp+3;
            //save the data
            pucBuffTemp[0] = pucBuff[ui2];
            pucBuffTemp[1] = pucBuff[ui2+1];
            pucBuffTemp[2] = pucBuff[ui2+2];
            ChunkAddr_6381(uAddr, pucBuff+ui2);
            r |= AVL6381_IBSP_I2C_Write(ePortIndex, uiSlaveAddr, pucBuff+ui2, &ui1);
            //restore data
            pucBuff[ui2] = pucBuffTemp[0];
            pucBuff[ui2+1] = pucBuffTemp[1];
            pucBuff[ui2+2] = pucBuffTemp[2];
            uAddr += uTemp;
            ui2 += uTemp;
            iSize -= uTemp;
        }
        ui1 = iSize+3;
        //save the data
        pucBuffTemp[0] = pucBuff[ui2];
        pucBuffTemp[1] = pucBuff[ui2+1];
        pucBuffTemp[2] = pucBuff[ui2+2];
        ChunkAddr_6381(uAddr, pucBuff+ui2);
        r |= AVL6381_IBSP_I2C_Write(ePortIndex, uiSlaveAddr, pucBuff+ui2, &ui1);
        //restore data
        pucBuff[ui2] = pucBuffTemp[0];
        pucBuff[ui2+1] = pucBuffTemp[1];
        pucBuff[ui2+2] = pucBuffTemp[2];
        uAddr += iSize;
        ui2 += iSize;
    }
    r |= AVL6381_IBSP_ReleaseSemaphore(&(gAVL6381I2CSem[ePortIndex])); 

    return r;
}

AVL6381_ErrorCode I2C_Write8_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uchar ucData)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pBuff[4];

    ChunkAddr_6381(uiAddr, pBuff);
    pBuff[3] = ucData;

    r = I2C_Write_6381(ePortIndex, uiSlaveAddr, pBuff, 4);
    
    return r;
}

AVL6381_ErrorCode I2C_Write16_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint16 uiData)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pBuff[5];

    ChunkAddr_6381(uiAddr, pBuff);
    Chunk16_6381(uiData, pBuff+3);

    r = I2C_Write_6381(ePortIndex, uiSlaveAddr, pBuff, 5);
    
    return r;
}

AVL6381_ErrorCode I2C_Write32_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint32 uiData)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar pBuff[7];

    ChunkAddr_6381(uiAddr, pBuff);
    Chunk32_6381(uiData, pBuff+3);
    r = I2C_Write_6381(ePortIndex, uiSlaveAddr, pBuff, 7);
    
    return r;
}

AVL6381_ErrorCode CheckChipReady_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiCoreReadyWord = 0;
    AVL_uint32 uiCoreRunning = 0;

    r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_c306_top_srst, &uiCoreRunning);
    r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_core_ready_word_iaddr, &uiCoreReadyWord);

    if(AVL6381_EC_OK == r)
    {
        if( (1 == uiCoreRunning) || (uiCoreReadyWord != 0x5aa57ff7) )
        {
            r = AVL6381_EC_GENERAL_FAIL;
        }
    }
    
    return r;
}

void ChunkAddr_6381(AVL_uint32 uiaddr, AVL_puchar pBuff)
{
    pBuff[0] =(AVL_uchar)(uiaddr>>16);
    pBuff[1] =(AVL_uchar)(uiaddr>>8);
    pBuff[2] =(AVL_uchar)(uiaddr);
    
    return;
}

void Chunk16_6381(AVL_uint16 uidata, AVL_puchar pBuff)
{
    pBuff[0] = (AVL_uchar)(uidata>>8);
    pBuff[1] = (AVL_uchar)(uidata & 0xff);
    return;
}

AVL_uint16 DeChunk16_6381(const AVL_puchar pBuff)
{
    AVL_uint16 uiData = 0;
    uiData = pBuff[0];
    uiData = (AVL_uint16)(uiData << 8) + pBuff[1];
    
    return uiData;
}

void Chunk32_6381(AVL_uint32 uidata, AVL_puchar pBuff)
{
    pBuff[0] = (AVL_uchar)(uidata>>24);
    pBuff[1] = (AVL_uchar)(uidata>>16);
    pBuff[2] = (AVL_uchar)(uidata>>8);
    pBuff[3] = (AVL_uchar)(uidata);
    
    return;
}

AVL_uint32 DeChunk32_6381(const AVL_puchar pBuff)
{
    AVL_uint32 uiData = 0;
    uiData = pBuff[0];
    uiData = (uiData << 8) + pBuff[1];
    uiData = (uiData << 8) + pBuff[2];
    uiData = (uiData << 8) + pBuff[3];
    
    return uiData;
}

AVL6381_ErrorCode GetCarrierFreqOffset_6381(AVL_pint32 piFreqOffsetHz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
    {
        r = DTMB_GetCarrierFreqOffset_6381(piFreqOffsetHz,ePortIndex);
    }
    else if(gsAVL6381Chip[ePortIndex].m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
    {
        r = DVBC_GetCarrierFreqOffset_6381(piFreqOffsetHz,ePortIndex);
    }

    return r;
}

AVL6381_ErrorCode SetGPIODir_6381( AVL6381_PIN_NUMBER enumPin, AVL6381_PIN_DIRECTION enumDir,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    switch(enumPin)
    {
        case PIN37:
            if(enumDir == PIN_OUTPUT)
            {    
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178000, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x2;
                r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178000, uiTemp);
            }
            else if(enumDir == PIN_INPUT)
            {
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178000, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x0;
                r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178000, uiTemp);
            }
            else
            {
                return AVL6381_EC_GENERAL_FAIL;
            }
            break;
        case PIN38:
            if(enumDir == PIN_OUTPUT)
            {    
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178008, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x2;
                r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178008, uiTemp);
            }
            else if(enumDir == PIN_INPUT)
            {
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178008, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x0;
                r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178008, uiTemp);
            }
            else
            {
                return AVL6381_EC_GENERAL_FAIL;
            } 
            break;
        case PIN39:
            if(enumDir == PIN_OUTPUT)
            {    
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178010, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x2;
                r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178010, uiTemp);
            }
            else if(enumDir == PIN_INPUT)
            {
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178010, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x0;
                r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178010, uiTemp);
            }
            else
            {
                return AVL6381_EC_GENERAL_FAIL;
            } 

            break;
        case PIN41:
            if(enumDir == PIN_OUTPUT)
            {    
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178018, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x2;
                r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178018, uiTemp);
            }
            else if(enumDir == PIN_INPUT)
            {
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178018, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x0;
                r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178018, uiTemp);
            }
            else
            {
                return AVL6381_EC_GENERAL_FAIL;
            } 
            break;
        case PIN44:
            if(enumDir == PIN_OUTPUT)
            {    
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178020, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x2;
                r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178020, uiTemp);
            }
            else if(enumDir == PIN_INPUT)
            {
                r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178020, &uiTemp);
                uiTemp = uiTemp & 0xFFFFFFFC;
                uiTemp = uiTemp | 0x0;
                r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178020, uiTemp);
            }
            else
            {
                return AVL6381_EC_GENERAL_FAIL;
            } 
            break;
        default:
            break;
    }

    return r;
}

AVL6381_ErrorCode SetGPIOVal_6381( AVL6381_PIN_NUMBER enumPin, AVL6381_PIN_VALUE enumVal,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;
    
    switch(enumPin)
    {
        case PIN37:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178000, &uiTemp);
            uiTemp = uiTemp & 0xFFFFFFFC;
            uiTemp = uiTemp | 0x2 | enumVal;
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178000, uiTemp);
            break;
        case PIN38:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178008, &uiTemp);
            uiTemp = uiTemp & 0xFFFFFFFC;
            uiTemp = uiTemp | 0x2 | enumVal;
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178008, uiTemp);
            break;
        case PIN39:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178010, &uiTemp);
            uiTemp = uiTemp & 0xFFFFFFFC;
            uiTemp = uiTemp | 0x2 | enumVal;
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178010, uiTemp);
            break;
        case PIN41:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178018, &uiTemp);
            uiTemp = uiTemp & 0xFFFFFFFC;
            uiTemp = uiTemp | 0x2 | enumVal;
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178018, uiTemp);
            break;
        case PIN44:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178020, &uiTemp);
            uiTemp = uiTemp & 0xFFFFFFFC;
            uiTemp = uiTemp | 0x2 | enumVal;
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178020, uiTemp);
            break;
        default:
            break;
    }

    return r;
}

AVL6381_ErrorCode GetGPIOVal_6381(AVL6381_PIN_NUMBER enumPin, AVL6381_PIN_VALUE *enumVal,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiVal = 0;
    AVL_uint32 uiTemp = 0;

    switch(enumPin)
    {
        case PIN37:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178000, &uiTemp);
            uiTemp = (uiTemp >> 2) & 0x1;
            *enumVal = (AVL6381_PIN_VALUE)uiTemp;
            break;
        case PIN38:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178008, &uiTemp);
            uiTemp = (uiTemp >> 2) & 0x1;
            *enumVal = (AVL6381_PIN_VALUE)uiTemp;
            break;
        case PIN39:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178010, &uiTemp);
            uiTemp = (uiTemp >> 2) & 0x1;
            *enumVal = (AVL6381_PIN_VALUE)uiTemp;

            break;
        case PIN41:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178018, &uiTemp);
            uiTemp = (uiTemp >> 2) & 0x1;
            *enumVal = (AVL6381_PIN_VALUE)uiTemp;
            break;
        case PIN44:
            r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x178020, &uiTemp);
            uiTemp = (uiTemp >> 2) & 0x1;
            *enumVal = (AVL6381_PIN_VALUE)uiTemp;
            break;
        default:
            break;
    }

    *enumVal = (AVL6381_PIN_VALUE)uiVal;

    return r;
}

void ResetFwData_6381(AVL_puchar pucFwData,Avl6381_PortIndex_t ePortIndex)
{
    gucAVL6381CustomizeFwData[ePortIndex] = 1;
    gsAVL6381Chip[ePortIndex].m_pInitialData = pucFwData;
}

AVL6381_ErrorCode InitErrorStat_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    
    AVL6381_uint64 time_tick_num = {0,0};

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_auto1_manual0_mode,ERROR_STAT_AUTO);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_timetick1_bytetick0,ERROR_STAT_TIME);

    WVMultiply32(&time_tick_num, gsAVL6381Chip[ePortIndex].m_FECFrequency_Hz/1000, 1000);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_time_tick_low,time_tick_num.m_LowWord);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_time_tick_high,time_tick_num.m_HighWord);

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_byte_tick_low,gsAVL6381Chip[ePortIndex].m_ErrorStat_Info.m_Number_Threshold_Byte);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_byte_tick_high,0);//high 32-bit is not used

    r |= ResetErrorStat_6381(ePortIndex);

    return r;
}

AVL6381_ErrorCode ResetBER_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL_uint16 uiLFSR_Sync;
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;
    AVL_uint32 uiCnt = 0;
    AVL_uint32 uiByteCnt = 0;
    AVL_uint32 uiBER_FailCnt = 0;
    AVL_uint32 uiBitErrors = 0;

    uiLFSR_Sync = 0;
    uiBER_FailCnt = 0;

    gsAVL6381ErrorStat[ePortIndex].m_LFSR_Sync = 0;
    gsAVL6381ErrorStat[ePortIndex].m_LostLock = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntBitErrors.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntBitErrors.m_LowWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntNumBits.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntNumBits.m_LowWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_BitErrors.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_BitErrors.m_LowWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_NumBits.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_NumBits.m_LowWord = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, &uiTemp);
    uiTemp |= 0x00000002;
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_lost_lock_caddr, 0);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r |= I2C_Write16_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_lost_lock_saddr, 0);
    }
    else
    {
        return AVL6381_EC_GENERAL_FAIL;
    }

    r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, &uiTemp);
    uiTemp &= 0xFFFFFFCF;
    uiTemp |= ((((AVL_uint32)gsAVL6381Chip[ePortIndex].m_ErrorStat_Info.m_BER_Test_Pattern) << 5) | (((AVL_uint32)gsAVL6381Chip[ePortIndex].m_ErrorStat_Info.m_BER_FB_Inversion) << 4));
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

    uiTemp &= 0xFFFFFE3F;
    uiTemp |= (1<<6);//For SFU or other standard, the start position of LFSR is 1, just follow the 0x47 sync word
    //uiTemp |= (1<<8);//For DEKTEC CARD, the standard position of LFSR is 4
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

    while(!uiLFSR_Sync)
    {
        uiTemp |= 0x00000006;
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);
        uiTemp &= 0xFFFFFFFD;
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

        uiCnt = 0;
        uiByteCnt = 0;
        while((uiByteCnt < 1000) && (uiCnt < 200))
        {
            r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_byte_num, &uiByteCnt);
            uiCnt++;
        }

        uiTemp |= 0x00000006;
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);
        uiTemp &= 0xFFFFFFF9;
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

        uiCnt = 0;
        uiByteCnt = 0;
        while((uiByteCnt < 10000) && (uiCnt < 200))
        {
            uiCnt++;
            r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_byte_num, &uiByteCnt);
        }

        uiTemp &= 0xFFFFFFF9;
        uiTemp |= 0x00000002;
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

        r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_byte_num, &uiByteCnt);
        r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_ber_err_cnt, &uiBitErrors);
        if(uiCnt == 200)
        {
            break;
        }
        else if((uiByteCnt << 3) < (10 * uiBitErrors))
        {
            uiBER_FailCnt++;
            if(uiBER_FailCnt > 10)
            {
                break;
            }
        }
        else
        {
            uiLFSR_Sync = 1;
        }
    }

    if(uiLFSR_Sync == 1)
    {
        uiTemp &= 0xFFFFFFF9;
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);
    }

    gsAVL6381ErrorStat[ePortIndex].m_LFSR_Sync = uiLFSR_Sync;

    return r;
}

AVL6381_ErrorCode ResetPER_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    gsAVL6381ErrorStat[ePortIndex].m_LostLock = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntPktErrors.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntPktErrors.m_LowWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntNumPkts.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_SwCntNumPkts.m_LowWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_PktErrors.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_PktErrors.m_LowWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_NumPkts.m_HighWord = 0;
    gsAVL6381ErrorStat[ePortIndex].m_NumPkts.m_LowWord = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, &uiTemp);
    uiTemp |= 0x00000001;
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_lost_lock_caddr, 0);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r |= I2C_Write16_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_lost_lock_saddr, 0);
    }
    else
    {
        return AVL6381_EC_GENERAL_FAIL;
    }

    r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, &uiTemp);
    uiTemp |= 0x00000008;
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);
    uiTemp |= 0x00000001;
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);
    uiTemp &= 0xFFFFFFFE;
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_esm_cntrl, uiTemp);

    return r;
}

AVL6381_ErrorCode ResetErrorStat_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_auto1_manual0_mode,&uiTemp);
    if(uiTemp == ERROR_STAT_AUTO)//auto mode
    {
        //reset auto error stat
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_tick_clear_req,0);
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_tick_clear_req,1);
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_tick_clear_req,0);
    }

    //only reset PER, since currently, final BER isn't meaningful
    r |= ResetPER_6381(ePortIndex);

    return r;
}

AVL6381_ErrorCode GetBER_6381(AVL_puint32 puiBER_x10e9, AVL6381_BER_Type enumBERType,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiHwCntBitErrors = 0;
    AVL_uint32 uiHwCntNumBits = 0;
    AVL_uint32 uiTemp = 0;
    AVL_uchar ucTemp = 0;
    AVL_uint16 usTemp = 0;
    AVL6381_uint64 uiTemp64 = {0,0};

    r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_ber_err_cnt, &uiHwCntBitErrors);
    r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_byte_num, &uiHwCntNumBits);
    uiHwCntNumBits <<= 3;

    if(uiHwCntNumBits > (1 << 31))
    {
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_esm_cntrl, &uiTemp);
        uiTemp |= 0x00000002;
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_esm_cntrl, uiTemp);
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_ber_err_cnt, &uiHwCntBitErrors);
        r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_byte_num, &uiHwCntNumBits);
        uiTemp &= 0xFFFFFFFD;
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_esm_cntrl, uiTemp);
        uiHwCntNumBits <<= 3;
        WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_SwCntNumBits, uiHwCntNumBits);
        WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_SwCntBitErrors, uiHwCntBitErrors);
        uiHwCntNumBits = 0;
        uiHwCntBitErrors = 0;
    }

    gsAVL6381ErrorStat[ePortIndex].m_NumBits.m_HighWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntNumBits.m_HighWord;
    gsAVL6381ErrorStat[ePortIndex].m_NumBits.m_LowWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntNumBits.m_LowWord;
    WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_NumBits, uiHwCntNumBits);
    gsAVL6381ErrorStat[ePortIndex].m_BitErrors.m_HighWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntBitErrors.m_HighWord;
    gsAVL6381ErrorStat[ePortIndex].m_BitErrors.m_LowWord = gsAVL6381ErrorStat[ePortIndex].m_SwCntBitErrors.m_LowWord;
    WVAdd32To64(&gsAVL6381ErrorStat[ePortIndex].m_BitErrors, uiHwCntBitErrors);

    // Compute the BER
    //uiTemp = WVDivide64(gsAVL6381ErrorStat[ePortIndex].m_NumBits, gsAVL6381ErrorStat[ePortIndex].m_BitErrors);
    //WVMultiply32(&uiTemp64, uiTemp, (AVL_uint32)1e9);
    //gsAVL6381ErrorStat[ePortIndex].m_BER = (uiTemp64.m_LowWord >> 30) + (uiTemp64.m_HighWord << 2);
    //*puiBER_x10e9 = gsAVL6381ErrorStat[ePortIndex].m_BER;
    //
    WVMultiply32(&uiTemp64, gsAVL6381ErrorStat[ePortIndex].m_BitErrors.m_LowWord, (AVL_uint32)1e9);
    uiTemp = WVDivide64(gsAVL6381ErrorStat[ePortIndex].m_NumBits, uiTemp64);
    gsAVL6381ErrorStat[ePortIndex].m_BER = uiTemp;
    *puiBER_x10e9 = uiTemp;

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
        return (AVL6381_EC_GENERAL_FAIL);
    }
    
    gsAVL6381ErrorStat[ePortIndex].m_LostLock = usTemp;

    return r;
}

AVL6381_ErrorCode SetSleepClock_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiMaxRetries = 10;
    AVL_uint32 delay_unit_ms = 20;//the time out window is 10*20=200ms
    AVL_uint32 i = 0;
    AVL_uchar ucSleepPLLIndex = 0;

    ucSleepPLLIndex = (AVL_uchar)(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueRefConfig);

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_c306_top_srst, 1);

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_demod_clkr, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Demod_PLL_DivR-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_demod_clkf, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Demod_PLL_DivF-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_demod_bwadj, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Demod_PLL_bwadj);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_demod_od, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Demod_PLL_od-1);

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_clkr, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Aux_PLL_DivR-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_clkf, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Aux_PLL_DivF-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_bwadj, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Aux_PLL_bwadj);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_od, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Aux_PLL_od-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_od, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Aux_PLL_od2-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_pll_aux_od, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_Aux_PLL_od3-1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,hw_AVL6381_clock_reset , 0);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_clock_reset, 1);
    AVL6381_IBSP_Delay(1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_out_phase, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_DDLL_out_phase);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_rd_phase, gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_DDLL_read_phase);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_init, 0);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_init, 1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_dll_init, 0);

    if(gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_use_pll_adc == 0)
    {
        gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz = gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_RefFrequency_Hz;
    }
    else
    {
        gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz = gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_ADC_Frequency_Hz;
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_adc_clock_sel, 0xc);
    }

    gsAVL6381Chip[ePortIndex].m_RefFrequency_Hz = gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_RefFrequency_Hz;
    gsAVL6381Chip[ePortIndex].m_DemodFrequency_Hz = gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_DemodFrequency_Hz;
    gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz = gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_SDRAM_Frequency_Hz;
    gsAVL6381Chip[ePortIndex].m_FECFrequency_Hz = gAVL6381SleepPLLConfig[ePortIndex][ucSleepPLLIndex].m_FEC_Frequency_Hz;

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_core_ready_word_iaddr, 0x00000000);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_c306_top_srst, 0);

    while (AVL6381_EC_OK != CheckChipReady_6381(ePortIndex))
    {
        if (uiMaxRetries <= i++)
        {
            r |= AVL6381_EC_GENERAL_FAIL;
            break;
        }
        AVL6381_IBSP_Delay(delay_unit_ms);
    }

    return r;
}

AVL6381_ErrorCode GetMode_6381(AVL_puchar pucurrent_mode,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 mode_data;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_current_active_mode_iaddr, &mode_data);
    *pucurrent_mode = (AVL_uchar)mode_data;
    return r;
}

AVL6381_ErrorCode EnableSerialSlowRate(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108050, 1);//clk
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108054, 1);//data 0
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108058, 1);//sync
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x10805C, 1);//error
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108060, 1);//valid

    return r;
}

AVL6381_ErrorCode DisableSerialSlowRate(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108050, 0);//clk
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108054, 0);//data 0
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108058, 0);//sync
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x10805C, 0);//error
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, 0x108060, 0);//valid

    return r;
}

AVL6381_ErrorCode SetSpectrumPola_6381(AVL6381_SpectrumInversion enumSpectrumPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetSpectrumPola_6381(enumSpectrumPola,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        /* DVBC always detects spectrum automatically */
    }

    return r;
}

AVL6381_ErrorCode DigitalCoreReset_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_digital_core_reset, 0);
    AVL6381_IBSP_Delay(1);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, hw_AVL6381_digital_core_reset, 1);
    
    return r;
}

AVL6381_ErrorCode SetMpegPacketLen_6381(AVL6381_MPEG_Packet_Len enumPacketLen,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    geAVL6381PacketLen[ePortIndex] = enumPacketLen;

    if(AVL_DEMOD_MODE_DTMB == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DTMB_SetMpegPacketLen_6381(enumPacketLen,ePortIndex);
    }
    else if(AVL_DEMOD_MODE_DVBC == gsAVL6381Chip[ePortIndex].m_current_demod_mode)
    {
        r = DVBC_SetMpegPacketLen_6381(enumPacketLen,ePortIndex);
    }

    return r;
}



AVL6381_ErrorCode DTMB_InitRx_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    // Load the default configuration
    r = SendRxOP_6381(OP_RX_LD_DEFAULT,ePortIndex);

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_dmd_clk_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_DemodFrequency_Hz);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_fec_clk_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_FECFrequency_Hz);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_sdram_clk_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_SDRAMFrequency_Hz);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_sample_rate_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz);

    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_input_format_caddr, 1);//always offset binary
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_input_select_caddr, 1);//always analog input

    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_iq_comp_enable_caddr, 0);//disable IQ compensation
        
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_equalizer_mode_caddr, 1);//data aided

    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_fec_lock_caddr, 0);

    r |= DTMB_SetSpectrumPola_6381(SPECTRUM_NORMAL, ePortIndex);// default spectrum is normal

    return r;
}

AVL6381_ErrorCode DTMB_InitADC_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    // ADC I path or ADC Q path
    if(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDTMBInputPath == ADC_IF_I || gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDTMBInputPath == ADC_IF_Q)
    {
        r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_tuner_type_caddr, 0);//IF input

        if(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDTMBInputPath== ADC_IF_I)//I
        {
            r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_ADC_I0_Q1_caddr, 0); //For ADC Init
        }
        else if(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDTMBInputPath == ADC_IF_Q)//Q
        {
            r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_ADC_I0_Q1_caddr, 1); //For ADC Init
        }
    }
    else
    {
        r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_tuner_type_caddr, 1);//BB input, both I&Q ADCs are used
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_nom_carrier_freq_Hz_iaddr, 0);
    }

    r |= SendRxOP_6381(OP_RX_INIT_ADC, ePortIndex);
    
    return r;
}

AVL6381_ErrorCode DTMB_AutoLockChannel_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_get_config_from_tps_caddr, 1);//get channel info from TPS

    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_fixed_header_caddr, 0);//not fixed header
    
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_demod_lock_status_iaddr, 0);//set the unlock status

    r |= SendRxOP_6381(OP_RX_ACQUIRE,ePortIndex);

    return r;
}

AVL6381_ErrorCode DTMB_GetSNR_6381(AVL_puint32 puiSNR_db,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint16 uiTemp = 0;

    r = I2C_Read16_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_snr_dB_x100_avg_saddr, &uiTemp);

    *puiSNR_db = (AVL_uint32)uiTemp;

    return r;
}

AVL6381_ErrorCode DTMB_Halt_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_lock_status_iaddr, &uiTemp);
    uiTemp &= 0xFFFFFEFF;
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_lock_status_iaddr, uiTemp);

    return r;
}

AVL6381_ErrorCode DTMB_GetRunningLevel_6381(AVL6381_RunningLevel *pRunningLevel,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar uiState = 0;

    r = I2C_Read8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_demod_status_caddr, &uiState);

    if(uiState == 0)
    {
        *pRunningLevel = AVL6381_RL_HALT;
    }
    else
    {
        *pRunningLevel = AVL6381_RL_NORMAL;
    }

    return r;
}

AVL6381_ErrorCode DTMB_GetSpectrumInversion_6381(AVL6381_SpectrumInversion *pSpectrumInversion,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;
    AVL_int32 iFrequencyValue = 0;

    if(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBIFFreqHz == 0)
    {
        r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_ddc_input_select_iaddr, &uiTemp);
        uiTemp = ((uiTemp >> 8) & 0x00000001);
        *pSpectrumInversion = (enum AVL6381_SpectrumInversion)uiTemp;
    }
    else
    {
        r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_nom_carrier_freq_Hz_iaddr, (AVL_puint32)&iFrequencyValue);

        //if(iFrequencyValue == (gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBIFFreqHz - gsAVL6381Chip[ePortIndex].m_RefFrequency_Hz))
        if(iFrequencyValue == ((AVL_int32)gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBIFFreqHz - (AVL_int32)gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz))
        {
            *pSpectrumInversion = SPECTRUM_NORMAL;
        }
        else
        {
            *pSpectrumInversion = SPECTRUM_INVERTED;
        }
    }

    return r;
}

AVL6381_ErrorCode DTMB_GetLockStatus_6381(AVL_puint32 pucLockStatus,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uchar ucLockStatus = 0;

    r = I2C_Read8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DTMB_fec_lock_caddr, &ucLockStatus);
	//TODO
	//printf("DTMB_GetLockStatus_6381 : %u\n", ucLockStatus);

    *pucLockStatus = ucLockStatus;

    return r;
}


AVL6381_ErrorCode DTMB_GetCarrierFreqOffset_6381(AVL_pint32 piFreqOffsetHz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;
    AVL_int32 iTemp = 0;
    AVL6381_int64 iProduct = {0,0};
    AVL_uint32 uiBinOffset = 0;
    AVL_uint32 uiRefFreqHz = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DTMB_sample_rate_Hz_iaddr, &uiRefFreqHz);
    r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_DTMB_cfo_freq_est, &uiTemp);
    iTemp = (AVL_int32)(uiTemp << 2);
    iTemp >>= 2;
    Multiply32By16(&iProduct, iTemp, (AVL_int16)(uiRefFreqHz/(AVL_int16)10000));
    iTemp = (AVL_int32)(((iProduct.m_HighWord & 0x000001FF) << 23) + (iProduct.m_LowWord >> 9));
    Multiply32By16(&iProduct, iTemp, 625);
    iTemp = (AVL_int32)(((iProduct.m_HighWord & 0x0001FFFF) << 15) + (iProduct.m_LowWord >> 17));
    r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_carrier_freq_bin_Hz_iaddr, &uiBinOffset);
    iTemp -= (AVL_int32)uiBinOffset;

    *piFreqOffsetHz = iTemp;

    return r;
}

AVL6381_ErrorCode DTMB_GetScatterData_6381(AVL_puint32 puiIQ_Data, AVL_puint16 puiSize,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint16 uiSize = 0;
    AVL_uint32 i = 0;
    AVL_puchar pucData = 0;
    AVL_uint32 uiTemp = 0;

    uiSize = *puiSize;
    if(uiSize > 256)
    {
        uiSize = 256;
    }
    pucData = (AVL_puchar)puiIQ_Data;

    r |= I2C_Read_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_DTMB_scatter_data, pucData, (uiSize << 2));

    for(i=0; i<uiSize; i++)
    {
        uiTemp = DeChunk32_6381(pucData);
        *puiIQ_Data = uiTemp;
        puiIQ_Data++;
        pucData += 4;
    }
    

    if(r != AVL6381_EC_OK)
    {
        uiSize = 0;
    }
    *puiSize = uiSize;

    return r;
}

AVL6381_ErrorCode DTMB_GetSignalInfo_6381(AVL_DTMB_SignalInfo *pSignalInfo,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_mode_status_iaddr, &uiTemp);
    pSignalInfo->m_HeaderMode = (AVL_DTMB_FrameHeaderMode)(uiTemp & 0x00000003);
    pSignalInfo->m_CodeRate = (AVL_DTMB_FEC_Rate)((uiTemp >> 8) & 0x00000003);
    pSignalInfo->m_CarrierMode = (AVL_DTMB_CarrierMode)((uiTemp >> 16) & 0x00000001);
    pSignalInfo->m_InterleaveMode = (AVL_DTMB_InterleaveMode)((uiTemp >> 24) & 0x00000001);

    r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_modulation_status_iaddr, &uiTemp);
    pSignalInfo->m_Modulation = (AVL_DTMB_ModulationMode)(uiTemp & 0x00000003);
    pSignalInfo->m_MappingMode = (AVL_DTMB_NR_Mode)((uiTemp >> 8) & 0x00000001);

    return r;
}

AVL6381_ErrorCode DTMB_GetSignalLevel_6381(AVL_puint32 puiSignalLevel,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_signal_strength_iaddr, &uiTemp);

    if( AVL6381_EC_OK == r)
    {
        uiTemp += 0x2000000;
        uiTemp &= 0x3ffffff;
        *puiSignalLevel = uiTemp >> 10;
    }

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegMode_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_mpeg_posedge_caddr,gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumRisingFalling);
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_mpeg_serial_caddr,gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegPacketLen_6381(AVL6381_MPEG_Packet_Len enumPacketLen,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_ts0_tsp1_caddr,enumPacketLen);

    return r;
}

AVL6381_ErrorCode DTMB_DisableMpegContinuousMode_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r =I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_cont_mpegclk_en_caddr, 0);

    return r;
}

AVL6381_ErrorCode DTMB_EnableMpegContinuousMode_6381(AVL_uint32 uiClockFreq_Hz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiMpegRefClkFreq_Hz = 0;

    r =I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_cont_mpegclk_en_caddr, 1 );
    r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_fec_clk_Hz_iaddr, &uiMpegRefClkFreq_Hz);

    if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel == AVL6381_MPEG_SERIAL)
    {
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_cntns_pkt_seri_rate_frac_n_iaddr, uiClockFreq_Hz);        
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_cntns_pkt_seri_rate_frac_d_iaddr, uiMpegRefClkFreq_Hz);
    }
    else
    {
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_cntns_pkt_para_rate_frac_n_iaddr,uiClockFreq_Hz);     
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_cntns_pkt_para_rate_frac_d_iaddr, uiMpegRefClkFreq_Hz);
    }

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegSerialPin_6381(AVL6381_MPEG_SerialDataPin enumSerialPin,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_serial_outpin_sel_caddr,enumSerialPin);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegSerialOrder_6381(AVL6381_MPEG_SERIAL_ORDER enumSerialOrder,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_mpeg_seq_caddr,enumSerialOrder);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegParallelOrder_6381(AVL6381_MPEG_PARALLEL_ORDER enumParallelOrder,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_mpeg_seq_caddr,enumParallelOrder);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegSerialSyncPulse_6381(AVL6381_MPEG_SERIAL_SYNC enumSerialSyncPulse,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_seri_sync_1_pulse_caddr,enumSerialSyncPulse);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegParallelPhase_6381(AVL6381_MPEG_PARALLEL_PHASE enumParallelPhase,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_mpeg_clk_phase_iaddr,enumParallelPhase);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegErrorBit_6381(AVL6381_MPEG_ErrorBit enumErrorBit,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_mpeg_ts_error_bit_en_caddr,enumErrorBit);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegErrorPola_6381(AVL6381_MPEG_ErrorBitPola enumErrorPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_pkt_err_pol_caddr,enumErrorPola);

    return r;
}

AVL6381_ErrorCode DTMB_SetMpegValidPola_6381(AVL6381_MPEG_ValidPola enumValidPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DTMB_mpeg_valid_inv_caddr,enumValidPola);

    return r;
}

AVL6381_ErrorCode DTMB_SetAGCPola_6381(AVL6381_AGCPola enumAGCPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_ifagc_pol_caddr, enumAGCPola);

    return r;
}

AVL_uint32 AVL6381_Table_420 [4][3] = {
                                {15,     35,     56},
                                {73,     96,    130},
                                {0,       0,    155},
                                {121,   164,    187}
                              };

AVL_uint32 AVL6381_Table_945 [4][3] =  {
                                 {14,       34,      55},
                                 {72,       96,     130},
                                 {0,        0,      155},
                                 {120,      164,    190}
                               };

AVL_uint32 AVL6381_Table_595 [4][3] = {
                                 {15,       35,     71},
                                 {73,       96,     130},
                                 {0,        0,      159},
                                 {122,     153,     198}
                              };

#define THRESHOLD_NR_595    25

AVL6381_ErrorCode DTMB_GetSignalQuality_6381(AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiEstimated_SNR = 0;
    AVL_DTMB_SignalInfo SignalInfo;
    AVL_int32 iThreshold_SNR = 0, window = 0;
    AVL_int32 iSignal_Quality = 0,iEstimated_SNR = 0;

    if ((iNormalization_Factor < 4) || (iNormalization_Factor >6))
    {
        iNormalization_Factor = 4;
    }

    r = DTMB_GetSNR_6381(&uiEstimated_SNR,ePortIndex);

    if(r != AVL6381_EC_OK)
    {
        *puiSignal_Quality = 0;
        return r;
    }

    r |= DTMB_GetSignalInfo_6381(&SignalInfo,ePortIndex);

    if(r != AVL6381_EC_OK)
    {
        *puiSignal_Quality = 0;
        return r;
    }

    iEstimated_SNR = uiEstimated_SNR;

    if (SignalInfo.m_MappingMode == RX_NR_ON)
        iThreshold_SNR = THRESHOLD_NR_595;
    else if (SignalInfo.m_HeaderMode == RX_PN420)
        iThreshold_SNR   =  AVL6381_Table_420[SignalInfo.m_Modulation][SignalInfo.m_CodeRate];
    else if (SignalInfo.m_HeaderMode == RX_PN595)
        iThreshold_SNR   =  AVL6381_Table_595[SignalInfo.m_Modulation][SignalInfo.m_CodeRate];
    else if (SignalInfo.m_HeaderMode == RX_PN945)
        iThreshold_SNR   =  AVL6381_Table_945[SignalInfo.m_Modulation][SignalInfo.m_CodeRate];


    if ((iThreshold_SNR == 0) || (iEstimated_SNR == 0))
    {
         *puiSignal_Quality = 0;
    }
    else
    {

        if ((iThreshold_SNR * 2) < 100)
        {
            window = iThreshold_SNR * 2;
        }
        else
        {
            window = 100;
        }

        iSignal_Quality = (iEstimated_SNR - (((iThreshold_SNR * 10) - (iNormalization_Factor * window))))/(window/10);

        if (iSignal_Quality > 100)
        {
            *puiSignal_Quality = 100;
        }
        else if (iSignal_Quality < 0)
        {
            *puiSignal_Quality = 0;
        }
        else
        {
            *puiSignal_Quality = iSignal_Quality;
        }
    }

    return r;
}

AVL6381_ErrorCode DTMB_SetSymbolRate_6381(AVL_uint32 uiSymbolRateHz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    guiDTMBSymbolRateSps[ePortIndex] = uiSymbolRateHz;
    r = I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DTMB_symbol_rate_Hz_iaddr, uiSymbolRateHz);

    return r;
}

AVL6381_ErrorCode DTMB_NoSignalDetection_6381(AVL_uint32 *puiNoSignal,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint16 usTemp = 0;

    *puiNoSignal = 0;

    r = I2C_Read16_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DTMB_num_early_reacq_saddr, &usTemp);
    
    if(usTemp >= MAX_LOCK_RETRIES)
    {
        *puiNoSignal = 1;
    }

    return r;
}

AVL6381_ErrorCode DTMB_SetSpectrumPola_6381(AVL6381_SpectrumInversion enumSpectrumPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_int32 iNomCarrierFreqHz = 0;

    if(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBIFFreqHz > gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz)
    {
        iNomCarrierFreqHz = gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBIFFreqHz - gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz;
    }
    else
    {
        iNomCarrierFreqHz = gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDTMBIFFreqHz;
    }

    switch(enumSpectrumPola)
    {
        case SPECTRUM_NORMAL:
            r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_spectrum_invert_caddr, 0);
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_nom_carrier_freq_Hz_iaddr, iNomCarrierFreqHz);
            break;
        case SPECTRUM_INVERTED:
            r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_spectrum_invert_caddr, 0);
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_nom_carrier_freq_Hz_iaddr, -iNomCarrierFreqHz);
            break;
        case SPECTRUM_AUTO:
            r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_spectrum_invert_caddr, 2);
            r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DTMB_nom_carrier_freq_Hz_iaddr, iNomCarrierFreqHz);//start from normal spectrum      
            break;
        default:
            break;
    }

    return r;
}



typedef struct AVL6381_DVBC_SQI_CN_Table_Element
{
    AVL_DVBC_QAMMode modulation;
    AVL_uint32 CN_Test_Result_x100_db;
}AVL6381_DVBC_SQI_CN_Table_Element;

AVL6381_DVBC_SQI_CN_Table_Element AVL6381_DVBC_CN_Table[]=
{
    {RX_DVBC_16QAM  , 1700}, 
    {RX_DVBC_32QAM  , 1980},
    {RX_DVBC_64QAM  , 2300}, 
    {RX_DVBC_128QAM , 2600}, 
    {RX_DVBC_256QAM , 2920} 
};

AVL6381_ErrorCode DVBC_InitRx_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;  

    // Load the default configuration
    r = SendRxOP_6381(OP_RX_LD_DEFAULT,ePortIndex);

    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_dmd_clk_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_DemodFrequency_Hz);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_fec_clk_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_FECFrequency_Hz);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_sample_rate_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_ADCFrequency_Hz);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_if_freq_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDVBCIFFreqHz);
    r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_symbol_rate_Hz_iaddr, gsAVL6381Chip[ePortIndex].m_InputSignal_Info.uiDVBCSymbolRateHz);

    return r;
}


AVL6381_ErrorCode DVBC_InitADC_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;  

    r = I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_input_format_caddr, 1);//always offset binary
    r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_input_select_caddr, 1);//always analog input
    
    if(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDVBCInputPath == ADC_IF_I || gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDVBCInputPath == ADC_IF_Q)// ADC I path or ADC Q path
    {
        r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_tuner_type_caddr, 0);
    }
    else
    {
        r |= I2C_Write8_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_tuner_type_caddr, 1);//BB input, both I&Q ADCs are used
    }

    if(gsAVL6381Chip[ePortIndex].m_InputSignal_Info.ueDVBCInputPath == ADC_IF_I)
    {
        r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_ADC_I0_Q1_caddr, 0);
    }
    else
    {
        r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_ADC_I0_Q1_caddr, 1);
    }

    return r;
}


AVL6381_ErrorCode DVBC_AutoLockChannel_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = SendRxOP_6381(OP_RX_ACQUIRE_DVBC_QM, ePortIndex);

    return r;
}

AVL6381_ErrorCode DVBC_Halt_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_lock_status_iaddr, &uiTemp);
    uiTemp &= 0xFFFFFEFF;
    r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_lock_status_iaddr, uiTemp);

    return r;
}

AVL6381_ErrorCode DVBC_GetRunningLevel_6381(AVL6381_RunningLevel *pRunningLevel,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiState = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_mode_status_iaddr, &uiState);

    if(uiState == 0)
    {
        *pRunningLevel = AVL6381_RL_HALT;
    }
    else
    {
        *pRunningLevel = AVL6381_RL_NORMAL;
    }

    return r;
}

AVL6381_ErrorCode DVBC_GetSpectrumInversion_6381(AVL6381_SpectrumInversion *pSpectrumInversion,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_demod_lock_indicator_iaddr, &uiTemp);

    if((uiTemp & 0xff) == 0)
    {
        *pSpectrumInversion = SPECTRUM_NORMAL;
    }
    else
    {
        *pSpectrumInversion = SPECTRUM_INVERTED;
    }
    
    return r;
}

AVL6381_ErrorCode DVBC_GetLockStatus_6381(AVL_puint32 puiLockStatus,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp1 = 0;

    *puiLockStatus = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_mode_status_iaddr, &uiTemp1);
	//printf("DVBC_GetLockStatus_6381 : %u\n", uiTemp1);
	
    if((r == AVL6381_EC_OK) && (0x15 == ((uiTemp1)&0xff)))
    {
        *puiLockStatus = 1;
    }

    return r;
}


AVL6381_ErrorCode DVBC_GetCarrierFreqOffset_6381(AVL_pint32 piFreqOffsetHz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;
    AVL_uint32 uiRefFreqHz = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_get_btr_crl_iaddr, &uiTemp);
    if(0x00 == uiTemp)
    {
        r |= I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_carrier_freq_offset_Hz_iaddr, &uiRefFreqHz);
        *piFreqOffsetHz = uiRefFreqHz;
        r |= I2C_Write32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_get_btr_crl_iaddr, 0x01);
        AVL6381_IBSP_Delay(50);
    }
    else
    {
        r = AVL6381_EC_RUNNING;
    }

    return r;
}



AVL6381_ErrorCode DVBC_GetScatterData_6381(AVL_puint32 puiIQ_Data, AVL_puint16 puiSize,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint16 uiSize = 0;
    AVL_uint32 i = 0;
    AVL_puchar pucData = 0;
    AVL_uint32 uiTemp = 0;

    uiSize = *puiSize;  
    if(uiSize > 512)
    {
        uiSize = 512;
    }
    pucData = (AVL_puchar)puiIQ_Data;

    r = I2C_Read_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_DVBC_scatter_data, pucData, (uiSize << 2));

    for(i=0; i<uiSize; i++)
    {
        uiTemp = DeChunk32_6381(pucData);
        *puiIQ_Data = uiTemp;
        puiIQ_Data++;
        pucData += 4;
    }
    
    if(r != AVL6381_EC_OK)
    {
        uiSize = 0;
    }
    *puiSize = uiSize;
    
    return r;
}

AVL6381_ErrorCode DVBC_GetSignalInfo_6381(AVL_DVBC_SignalInfo *pSignalInfo,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_qam_mode_iaddr, &uiTemp);

    pSignalInfo->m_Modulation = (AVL_DVBC_QAMMode)(uiTemp & 0x00000007);

    return r;
}

AVL6381_ErrorCode DVBC_GetSignalLevel_6381(AVL_puint32 puiSignalLevel,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiTemp = 0;

    r = I2C_Read32_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), hw_AVL6381_if_aagc_gain, &uiTemp);

    if( AVL6381_EC_OK == r)
    {
        uiTemp += 0x2000000;
        uiTemp &= 0x3ffffff;
        *puiSignalLevel = uiTemp >> 10;
    }

    return r;
}

AVL6381_ErrorCode DVBC_GetSNR_6381(AVL_puint32 puiSNR_db,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint16 uiSNR_x100db = 0;
    AVL_uint32 uiTemp = 0;

   // r = I2C_Read32_6381((AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_get_btr_crl_iaddr, &uiTemp);
    if(0x00 == uiTemp)
    {
        r = I2C_Read16_6381(ePortIndex, (AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rs_AVL6381_DVBC_snr_dB_x100_saddr, &uiSNR_x100db);
        *puiSNR_db = (AVL_uint32)uiSNR_x100db;
     //   r |= I2C_Write32_6381((AVL_uint16)(gsAVL6381Chip[ePortIndex].m_SlaveAddr), rc_AVL6381_DVBC_get_btr_crl_iaddr, 0x01);
        AVL6381_IBSP_Delay(50);
    }
    else
    {
        r = AVL6381_EC_RUNNING;
    }

    return r;
}

AVL6381_ErrorCode DVBC_GetSignalQuality_6381(AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiEstimated_SNR = 0;
    AVL_uint32 CN_Ref_x100_db = 0;
    AVL_int32  iTempIndex = 0;
    AVL_int32  iTempRange = 0;
    AVL_uint32 uiLockStatus;
    AVL_DVBC_SignalInfo DVBCSignalInfo;
    AVL_DVBC_QAMMode modulation;

    r = DVBC_GetLockStatus_6381(&uiLockStatus,ePortIndex);
    if (uiLockStatus == 1)
    {
        //get the signal modulation mode
        r |= DVBC_GetSignalInfo_6381(&DVBCSignalInfo,ePortIndex);
        modulation = DVBCSignalInfo.m_Modulation;
        
        //get the signal reference CN for CN table
        iTempRange = sizeof(AVL6381_DVBC_CN_Table) / sizeof(AVL6381_DVBC_SQI_CN_Table_Element);
        for(iTempIndex=0; iTempIndex < iTempRange; iTempIndex++)
        {
            if(AVL6381_DVBC_CN_Table[iTempIndex].modulation == modulation)
            {
                CN_Ref_x100_db = AVL6381_DVBC_CN_Table[iTempIndex].CN_Test_Result_x100_db;
                break;
            }
        }

        //get the signal estimate SNR
        r |= DVBC_GetSNR_6381(&uiEstimated_SNR,ePortIndex);

        //SQI is 100 when the CN higher than threshold 2dB
        if (uiEstimated_SNR > CN_Ref_x100_db + 200)
        {
            *puiSignal_Quality = 100;
        }
        else if (uiEstimated_SNR < CN_Ref_x100_db -100)
        {
            *puiSignal_Quality = 0;
        }
        else
        {
            *puiSignal_Quality = (uiEstimated_SNR - CN_Ref_x100_db + 100) / 3;    
        }
        
        return r;
    }
    else
    {
        *puiSignal_Quality = 0;
        return r;
    }
}

AVL6381_ErrorCode DVBC_GetBER_BeforeRS_6381(AVL_puint32 puiBERBeforeRS_x10e9,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DVBC_berBeforRS_iaddr, puiBERBeforeRS_x10e9);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegMode_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_mpeg_posedge_caddr,gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumRisingFalling);
    r |= I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_mpeg_serial_caddr,gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegPacketLen_6381(AVL6381_MPEG_Packet_Len enumPacketLen,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_ts0_tsp1_caddr,enumPacketLen);

    return r;
}

AVL6381_ErrorCode DVBC_DisableMpegContinuousMode_6381(Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write16_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_cont_mpegclk_en_2_saddr,0);
    
    return r;
}

AVL6381_ErrorCode DVBC_EnableMpegContinuousMode_6381(AVL_uint32 uiClockFreq_Hz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;
    AVL_uint32 uiMpegRefClkFreq_Hz = 0;

    r = I2C_Write16_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_cont_mpegclk_en_2_saddr,1);

    r |= I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_fec_clk_Hz_iaddr, &uiMpegRefClkFreq_Hz);
    if(gsAVL6381Chip[ePortIndex].m_MPEG_Info.enumSerialParallel == AVL6381_MPEG_SERIAL)
    {
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_cntns_pkt_seri_rate_frac_n_32bit_iaddr, uiClockFreq_Hz);      
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_cntns_pkt_seri_rate_frac_d_32bit_iaddr, uiMpegRefClkFreq_Hz);
    }
    else
    {
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_cntns_pkt_para_rate_frac_n_32bit_iaddr,uiClockFreq_Hz);       
        r |= I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_cntns_pkt_para_rate_frac_d_32bit_iaddr, uiMpegRefClkFreq_Hz);
    }
    
    return r;
}

AVL6381_ErrorCode DVBC_SetMpegSerialPin_6381(AVL6381_MPEG_SerialDataPin enumSerialPin,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_serial_outpin_sel_caddr,enumSerialPin);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegSerialOrder_6381(AVL6381_MPEG_SERIAL_ORDER enumSerialOrder,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_mpeg_seq_caddr,enumSerialOrder);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegParallelOrder_6381(AVL6381_MPEG_PARALLEL_ORDER enumParallelOrder,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_mpeg_seq_caddr,enumParallelOrder);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegSerialSyncPulse_6381(AVL6381_MPEG_SERIAL_SYNC enumSerialSyncPulse,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_seri_sync_1_pulse_caddr,enumSerialSyncPulse);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegParallelPhase_6381(AVL6381_MPEG_PARALLEL_PHASE enumParallelPhase,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_mpeg_clk_phase_iaddr,enumParallelPhase);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegErrorBit_6381(AVL6381_MPEG_ErrorBit enumErrorBit,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_mpeg_ts_error_bit_en_caddr,enumErrorBit);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegErrorPola_6381(AVL6381_MPEG_ErrorBitPola enumErrorPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_pkt_err_pol_caddr,enumErrorPola);

    return r;
}

AVL6381_ErrorCode DVBC_SetMpegValidPola_6381(AVL6381_MPEG_ValidPola enumValidPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr,rc_AVL6381_DVBC_mpeg_valid_inv_caddr,enumValidPola);

    return r;
}

AVL6381_ErrorCode DVBC_SetAGCPola_6381(AVL6381_AGCPola enumAGCPola,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write8_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_ifagc_pol_caddr, enumAGCPola);

    return r;
}

AVL6381_ErrorCode DVBC_SetSymbolRate_6381(AVL_uint32 uiSymbolRateHz,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    r = I2C_Write32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rc_AVL6381_DVBC_symbol_rate_Hz_iaddr, uiSymbolRateHz);
   
    return r;
}

AVL6381_ErrorCode DVBC_NoSignalDetection_6381(AVL_uint32 *puiNoSignal,Avl6381_PortIndex_t ePortIndex)
{
    AVL6381_ErrorCode r = AVL6381_EC_OK;

    *puiNoSignal = 0;

    r = I2C_Read32_6381(ePortIndex, gsAVL6381Chip[ePortIndex].m_SlaveAddr, rs_AVL6381_DVBC_no_signal_input_iaddr, puiNoSignal);
    
    return r;
}


