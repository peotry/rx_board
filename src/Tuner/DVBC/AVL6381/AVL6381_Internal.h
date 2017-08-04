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



#ifndef AVL6381_INTERNAL_H
#define AVL6381_INTERNAL_H

#ifdef AVL_CPLUSPLUS
extern "C" {
#endif

#include "user_defined_data_type.h"
#include "Avl6381Manager.h"

#define AVL_min(x,y) (((x) < (y)) ? (x) : (y))
#define AVL_max(x,y) (((x) < (y)) ? (y) : (x))
#define AVL_floor(a) (((a) == (int)(a))? ((int)(a)) : (((a) < 0)? ((int)((a)-1)) : ((int)(a))))
#define AVL_ceil(a)  (((a) == (int)(a))? ((int)(a)) : (((a) < 0)? ((int)(a)) : ((int)((a)+1))))
#define AVL_abs(a)  (((a)>0) ? (a) : (-(a)))
    
#define AVL6381_API_VER_MAJOR   0x01
#define AVL6381_API_VER_MINOR   0x0E
#define AVL6381_API_VER_BUILD   0x1B


#define AVL_DEMOD_MODE_DTMB     0
#define AVL_DEMOD_MODE_DVBC     1
#define AVL_DEMOD_MODE_UNKNOWN  2

#define AVL6381_EC_OK                   0           // There is no error.
#define AVL6381_EC_WARNING              1           // There is warning.
#define AVL6381_EC_GENERAL_FAIL         2           // A general failure has occurred.
#define AVL6381_EC_I2C_FAIL             4           // An I2C operation failed during communication with the AVL6381 through the BSP.
#define AVL6381_EC_I2C_REPEATER_FAIL    8           // An error ocurred during communication between AVL6381 I2C master and tuner. This error code is defined by enum AVL6381_MessageType_II2C_Repeater_Status.                         
#define AVL6381_EC_RUNNING              16          // The AVL6381 device is busy processing a previous receiver/i2c repeater command.
#define AVL6381_EC_TIMEOUT              32          // Operation failed in a given time period
#define AVL6381_EC_SLEEP                64          // Demod in sleep mode
#define AVL6381_EC_NOT_SUPPORTED        128         // Specified operation isn't support in current senario
#define AVL6381_EC_BSP_ERROR1           256         // BSP Error 1, if it's used, need to be customized
#define AVL6381_EC_BSP_ERROR2           512         // BSP Error 2, if it's used, need to be customized

#define BIT_REPEATER_ENABLE     1
#define BIT_REPEATER_SEL        2
#define BIT_REPEATER_WAIT_ACK   4

#define OP_RX_IDLE              0x00
#define OP_RX_LD_DEFAULT        0x01
#define OP_RX_ACQUIRE           0x02
#define OP_RX_HALT              0x03
#define OP_RX_CMD_DEBUG         0x04    
#define OP_RX_SDRAM_TEST        0x05 
#define OP_RX_SLEEP             0x06
#define OP_RX_WAKE_UP           0x07
#define OP_RX_INIT_SDRAM        0x08
#define OP_RX_INIT_ADC          0x09
#define OP_RX_CHANGE_MODE       0x0A
#define OP_RX_ACQUIRE_DVBC_QM   0x0C

typedef struct AVL6381_PLL_Conf
{
    AVL_uint32 m_RefFrequency_Hz;           // The reference clock frequency in units of Hz.
    AVL_uchar m_Demod_PLL_DivR;           // PLL reference clock divider value
    AVL_uchar m_Demod_PLL_DivF;            // PLL feedback clock divider value
    AVL_uchar m_Demod_PLL_bwadj;
    AVL_uchar m_Demod_PLL_od;
    AVL_uint32 m_DemodFrequency_Hz;         // The internal demod clock frequency in units of Hz.
    AVL_uchar m_Aux_PLL_DivR;           // PLL reference clock divider value
    AVL_uchar m_Aux_PLL_DivF;            // PLL feedback clock divider value
    AVL_uchar m_Aux_PLL_bwadj;
    AVL_uchar m_Aux_PLL_od;                     // PLL output divider value
    AVL_uchar m_Aux_PLL_od2;                    // PLL output divider 2
    AVL_uchar m_Aux_PLL_od3;                    // PLL output divider 3
    AVL_uchar m_use_pll_adc;
    AVL_uint32 m_DDLL_out_phase;            // DDLL SDRAM OC Clock Phase Adjust value
    AVL_uint32 m_DDLL_read_phase;           // DDLL Read Clock Phase Adj Register
    AVL_uint32 m_FEC_Frequency_Hz;          // The FEC clock frequency in units of Hz.
    AVL_uint32 m_SDRAM_Frequency_Hz;        // The SDRAM clock frequency in units of Hz.
    AVL_uint32 m_ADC_Frequency_Hz;          // The FEC clock frequency in units of Hz.
}AVL6381_PLL_Conf;

// Used to configure the AVL PART device SDRAM controller. 
typedef struct AVL6381_SDRAM_Conf
{
    AVL_uint16 m_SDRAM_TRC;             // SDRAM active to active command period in ns. 
    AVL_uint16 m_SDRAM_TXSR;            // SDRAM exit self-refresh to active command period in ns.
    AVL_uint16 m_SDRAM_TRCAR;           // SDRAM auto refresh period in ns.  Minimum time between two auto refresh commands.
    AVL_uint16 m_SDRAM_TWR;             // SDRAM write recovery time in SDRAM clock cycles->  The delay from the last data in to the next precharge command.  Valid range is 1 to 4 clock cycles->
    AVL_uint16 m_SDRAM_TRP;             // SDRAM precharge period in ns.
    AVL_uint16 m_SDRAM_TRCD;            // SDRAM minimum delay between active and read/write commands in ns.
    AVL_uint16 m_SDRAM_TRAS;            // SDRAM minimum delay between active and precharge commands in ns.
    AVL_uint16 m_SDRAM_CAS_Latency;     // SDRAM delay between read command and availability of first data in SDRAM clock cycles->  Valid range is 1 to 4 clock cycles->
}AVL6381_SDRAM_Conf;

// Defines the AVL6381 device spectrum inversion mode
typedef enum AVL6381_SpectrumInversion
{
    SPECTRUM_NORMAL = 0,
    SPECTRUM_INVERTED = 1,
    SPECTRUM_AUTO = 2
}AVL6381_SpectrumInversion;
    
    // Defines the type of signal being input to the AVL6381 device.  The default value in the AVL6381 device is \a RX_COMPLEX_BASEBAND
typedef enum AVL6381_RxInput
{
    ADC_IF_I = 0,           // = 0 The input signal is a real IF signal routed to the AVL6381 device I channel input.
    ADC_IF_Q = 1,           // = 1 The input signal is a real IF signal routed to the AVL6381 device Q channel input.
    ADC_BB_IQ = 2       // = 2 The input signal is a complex baseband signal.
}AVL6381_RxInput;

typedef enum AVL6381_InputSelect
{
    ADC_INPUT = 0,
    DIGITAL_INPUT = 1
}AVL6381_InputSelect;

// Defines the polarity of the analog AGC control signal.  It must be set to match the requirement of the tuner being used.
// The default value in the AVL6381 device is AGC_INVERTED
typedef enum AVL6381_AGCPola
{
    AGC_NORMAL = 0,             // = 0  Normal. Used for a tuner whose gain increases with increased AGC voltage.
    AGC_INVERTED = 1            // = 1  Inverted. Used for tuner whose gain decreases with increased AGC voltage.
}AVL6381_AGCPola;

typedef enum AVL6381_MPEG_Mode
{
    AVL6381_MPEG_PARALLEL = 0,           //=0  Output parallel MPEG data
    AVL6381_MPEG_SERIAL = 1              // =1  Output serial MPEG data via D7.
}AVL6381_MPEG_Mode; 

typedef enum AVL6381_MPEG_ErrorBit
{
    AVL6381_MPEG_ERROR_BIT_DISABLE = 0,
    AVL6381_MPEG_ERROR_BIT_ENABLE = 1
}AVL6381_MPEG_ErrorBit;

typedef enum AVL6381_MPEG_SerialDataPin
{
    AVL6381_MPEG_SERIAL_D0 = 0,
    AVL6381_MPEG_SERIAL_D1 = 1
}AVL6381_MPEG_SerialDataPin;

typedef enum AVL6381_MPEG_Continuous
{
    AVL6381_MPEG_CONTINUOUS_ENABLE = 0,
    AVL6381_MPEG_CONTINUOUS_DISABLE = 1
}AVL6381_MPEG_Continuous;

typedef enum AVL6381_MPEG_Packet_Len
{
    AVL6381_MPEG_188 = 0,
    AVL6381_MPEG_204 = 1
}AVL6381_MPEG_Packet_Len;

typedef enum AVL6381_MPEG_ClockMode
{
    AVL6381_MPEG_FALLING = 0,   //  =0  MPEG data should be sampled on the falling edge of the clock.
    AVL6381_MPEG_RISING = 1     //  =1  MPEG data should be sampled on the rising edge of the clock.
}AVL6381_MPEG_ClockMode;

typedef enum AVL6381_MPEG_ErrorBitPola
{
    AVL6381_MPEG_ERROR_HIGH = 0,
    AVL6381_MPEG_ERROR_LOW = 1
}AVL6381_MPEG_ErrorBitPola;

typedef enum AVL6381_MPEG_ValidPola
{
    AVL6381_MPEG_VALID_HIGH = 0,
    AVL6381_MPEG_VALID_LOW = 1
}AVL6381_MPEG_ValidPola;

typedef enum AVL6381_MPEG_PARALLEL_PHASE
{
    AVL6381_MPEG_PARALLEL_PHASE_0 = 0,
    AVL6381_MPEG_PARALLEL_PHASE_1 = 1,
    AVL6381_MPEG_PARALLEL_PHASE_2 = 2,
    AVL6381_MPEG_PARALLEL_PHASE_3 = 3
}AVL6381_MPEG_PARALLEL_PHASE;

typedef enum AVL6381_MPEG_PARALLEL_ORDER
{
    AVL6381_MPEG_PARALLEL_ORDER_NORMAL = 0,
    AVL6381_MPEG_PARALLEL_ORDER_INVERT = 1
}AVL6381_MPEG_PARALLEL_ORDER;

typedef enum AVL6381_MPEG_SERIAL_ORDER
{
    AVL6381_MPEG_SERIAL_ORDER_MSB = 0,
    AVL6381_MPEG_SERIAL_ORDER_LSB = 1
}AVL6381_MPEG_SERIAL_ORDER;

typedef enum AVL6381_MPEG_SERIAL_SYNC
{
    AVL6381_MPEG_SERIAL_SYNC_8_PULSE = 0,
    AVL6381_MPEG_SERIAL_SYNC_1_PULSE = 1
}AVL6381_MPEG_SERIAL_SYNC;

typedef enum AVL6381_Clock_Config
{
    AVL6381_Xtal_30M = 0,
    AVL6381_Xtal_16M = 1,
    AVL6381_Xtal_24M = 2,
    AVL6381_Xtal_27M = 3
}AVL6381_Clock_Config;

typedef struct AVL6381_InputConfig
{
    AVL_uint16 usDemodI2CAddr;
    AVL6381_Clock_Config ueRefConfig;
    AVL6381_RxInput ueDTMBInputPath;
    AVL6381_RxInput ueDVBCInputPath;
    AVL_uint32 uiDTMBIFFreqHz;
    AVL_uint32 uiDVBCIFFreqHz;
    AVL_uint32 uiDTMBSymbolRateHz;
    AVL_uint32 uiDVBCSymbolRateHz;
}AVL6381_InputConfig;
    
typedef struct AVL6381_MPEGConfig
{
    AVL6381_MPEG_Mode enumSerialParallel;
    AVL6381_MPEG_ClockMode enumRisingFalling;
    AVL6381_MPEG_Continuous enumConti;
}AVL6381_MPEGConfig;

// Defines BER type
typedef enum AVL6381_BER_Type
{
    PRE_VITERBI_BER     =   0,                      // previous viterbi BER     will be acquired.
    POST_VITERBI_BER    =   1,                      // post viterbi BER will be acquired.
    PRE_LDPC_BER        =   2,                      // previous LDPC BER will be acquired.
    POST_LDPC_BER       =   3,                      // post LDPC BER will be acquired.
    FINAL_BER           =   4                       // final BER will be acquired.
}AVL6381_BER_Type;

// Defines Error statistics mode
typedef enum AVL6381_ErrorStat_Mode
{
    ERROR_STAT_MANUAL   =   0,                      // manual error statistics means application should reset the error statistics manually.
    ERROR_STAT_AUTO     =   1                       // auto error statistics which means error statistics will be reset automatically according to the setting of auto error statistics. Please refer to following data structure.
}AVL6381_ErrorStat_Mode;

// Defines the type of auto error statistics 
typedef enum AVL6381_AutoErrorStat_Type
{
    ERROR_STAT_BYTE     =   0,                      // error statistics will be reset according to the number of received bytes->
    ERROR_STAT_TIME     =   1                       // error statistics will be reset according to time interval.
}AVL6381_AutoErrorStat_Type;

// Defines the test pattern being used for BER/PER measurements.
typedef enum AVL6381_TestPattern
{
    AVL6381_TEST_LFSR_15    =   0,                  // BER test pattern is LFSR15
    AVL6381_TEST_LFSR_23    =   1                   // BER test pattern is LFSR23        
}AVL6381_TestPattern;

// Defines whether the feeback bit of the LFSR used to generate the BER/PER test pattern is inverted.
typedef enum AVL6381_LFSR_FbBit
{
    AVL6381_LFSR_FB_NOT_INVERTED    =   0,          // LFSR feedback bit isn't inverted
    AVL6381_LFSR_FB_INVERTED        =   1           // LFSR feedback bit is inverted
}AVL6381_LFSR_FbBit;

typedef struct AVL6381_ErrorStat_Info
{
    AVL6381_ErrorStat_Mode      m_ErrorStat_Mode;           // indicates the error statistics mode. 
    AVL6381_AutoErrorStat_Type  m_AutoErrorStat_Type;       // indicates the MPEG data sampling clock mode.
    AVL6381_TestPattern         m_BER_Test_Pattern;         // indicates the pattern of LFSR.
    AVL6381_LFSR_FbBit          m_BER_FB_Inversion;         // indicates LFSR feedback bit inversion.
    AVL_uint32                  m_LFSR_Sync;                // indicates the LFSR synchronization status.
    AVL_uint32                  m_Time_Threshold_Ms;        // used to set time interval for auto error statistics.
    AVL_uint32                  m_Number_Threshold_Byte;    // used to set the received byte number threshold for auto error statistics.
}AVL6381_ErrorStat_Info;

typedef struct AVL6381_Chip
{
    AVL_uint16 m_SlaveAddr;                         // Seven bit I2C slave address of the device. 
    AVL_uchar ucStartupMode;
    AVL_uchar m_current_demod_mode;                                 // Indicates the current standard mode that the device is configured to operate in. It is set each time a lock channel operation is requested on the part-supported standard.
    AVL_puchar m_pInitialData;                                      // Store the start address of patch array
    AVL_uint32 m_DemodFrequency_Hz;                                 // The demodulator internal clock frequency in Hz. 
    AVL_uint32 m_SDRAMFrequency_Hz;                                 // The SDRAM clock frequency in Hz. 
    AVL_uint32 m_FECFrequency_Hz;                                   // The FEC clock frequency in Hz. 
    AVL_uint32 m_RefFrequency_Hz;                                   // The reference clock frequency in units of Hz.
    AVL_uint32 m_ADCFrequency_Hz;                                   // ADC clock frequency from PLL
    AVL_semaphore m_semRx;                                          // A semaphore used to protect the receiver.
    AVL6381_MPEGConfig m_MPEG_Info;                            // Stores the MPEG configuration structure information  for the whole chip.
    AVL6381_InputConfig m_InputSignal_Info;            // The info of signal which is inputted into demod
    AVL6381_ErrorStat_Info m_ErrorStat_Info;
}AVL6381_Chip;

// Defines the AVL6381 device running levels.
typedef enum AVL6381_RunningLevel
{
    AVL6381_RL_HALT =  0,       // = 0 Halt running level.
    AVL6381_RL_NORMAL = 2       // = 2 Normal running level.
}AVL6381_RunningLevel;
    // Stores an unsigned 64-bit integer
typedef struct AVL6381_uint64
{
    AVL_uint32 m_HighWord;                  // The most significant 32-bits of the unsigned 64-bit integer
    AVL_uint32 m_LowWord;                   // The least significant 32-bits of the unsigned 64-bit integer
}AVL6381_uint64;

// Stores a signed 64-bit integer
typedef struct AVL6381_int64
{
    AVL_int32 m_HighWord;                   // The most significant 32-bits of the signed 64-bit integer
    AVL_uint32 m_LowWord;                   // The least significant 32-bits of the signed 64-bit integer
}AVL6381_int64;

// The Availink version structure.
typedef struct AVL_VerInfo {
    AVL_uchar m_Major;                  // The major version number
    AVL_uchar m_Minor;                  // The minor version number
    AVL_uint16 m_Build;                 // The build version number
}AVL_VerInfo;
    
// Stores AVL6381 device version information.
typedef struct AVL6381_VerInfo
{
    struct AVL_VerInfo m_Chip;          // Hardware version information. Should be X.X.X. 
    struct AVL_VerInfo m_API;           // SDK version information.
    struct AVL_VerInfo m_Patch;         // The version of the internal patch.
} AVL6381_VerInfo;

// Contains variables for storing error statistics used in the BER and PER calculations.
typedef  struct AVL6381_ErrorStats
{
    AVL_uint16 m_LFSR_Sync;                 // Indicates whether the receiver is synchronized with the transmitter generating the BER test pattern.
    AVL_uint16 m_LostLock;                  // Indicates whether the receiver has lost lock since the BER/PER measurement was started.
    AVL6381_uint64 m_SwCntNumBits;  // A software counter which stores the number of bits which have been received.
    AVL6381_uint64 m_SwCntBitErrors;    // A software counter which stores the number of bit errors which have been detected.
    AVL6381_uint64 m_NumBits;       // The total number of bits which have been received.
    AVL6381_uint64 m_BitErrors;     // The total number of bit errors which have been detected.
    AVL6381_uint64 m_SwCntNumPkts;  // A software counter which stores the number of packets which have been received.
    AVL6381_uint64 m_SwCntPktErrors;    // A software counter which stores the number of packet errors which have been detected.
    AVL6381_uint64 m_NumPkts;       // The total number of packets which have been received.
    AVL6381_uint64 m_PktErrors;     // The total number of packet errors which have been detected.
    AVL_uint32 m_BER;                       // The bit error rate scaled by 1e9.
    AVL_uint32 m_PER;                       // The packet error rate scaled by 1e9.
}AVL6381_ErrorStats;

typedef enum  AVL6381_PIN_DIRECTION
{
    PIN_OUTPUT,
    PIN_INPUT
}AVL6381_PIN_DIRECTION;

typedef enum  AVL6381_PIN_NUMBER
{
    PIN37,
    PIN38,
    PIN39,
    PIN41,
    PIN44
}AVL6381_PIN_NUMBER;

typedef enum  AVL6381_PIN_VALUE
{
    LOGIC_0,
    LOGIC_1
}AVL6381_PIN_VALUE;

typedef struct  AVL6381_PLL_Config
{
    AVL_uchar sDTMBPLLConfig;
    AVL_uchar sDVBCPLLConfig;
}AVL6381_PLL_Config;

void Chunk16_6381(AVL_uint16 uidata, AVL_puchar pBuff);
AVL_uint16 DeChunk16_6381(const AVL_puchar pBuff);
void Chunk32_6381(AVL_uint32 uidata, AVL_puchar pBuff);
AVL_uint32 DeChunk32_6381(const AVL_puchar pBuff);
void ChunkAddr_6381(AVL_uint32 uiaddr, AVL_puchar pBuff);
    

AVL6381_ErrorCode SetAGCPola_6381(AVL6381_AGCPola enumAGCPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode EnableAGC_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode CheckChipReady_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode InitSemaphore_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode IRx_Initialize_6381(Avl6381_PortIndex_t ePortIndex);
void WVAdd32To64(AVL6381_uint64 *pSum, AVL_uint32 uiAddend);
AVL_uint32 WVDivide64(AVL6381_uint64 y, AVL6381_uint64 x);
AVL_uint32 GreaterThanOrEqual64(AVL6381_uint64 a, AVL6381_uint64 b);
void Subtract64(AVL6381_uint64 *pA, AVL6381_uint64 b);
void WVMultiply32(AVL6381_uint64 *pDst, AVL_uint32 m1, AVL_uint32 m2);
void WVAddScaled32To64(AVL6381_uint64 *pDst, AVL_uint32 a);
void Multiply32By16(AVL6381_int64 *pY, AVL_int32 a, AVL_int16 b);
AVL6381_ErrorCode SetPLL_6381(AVL6381_PLL_Conf * pPLL_Conf,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetRxOP_Status_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode IBase_Initialize_6381(AVL6381_PLL_Conf * pPLL_Conf,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SendRxOP_6381(AVL_uchar ucOpCmd,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode TestSDRAM_6381(AVL_puint32 puiTestResult, AVL_puint32 puiTestPattern,Avl6381_PortIndex_t ePortIndex);
AVL_uint32 ConvertNsToSDRAM_Clocks(AVL_uint32 SDRAM_ClkFrequency_Hz, AVL_uint16 Value_ns);
AVL6381_ErrorCode I2C_Initialize_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode I2C_Read_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiOffset, AVL_puchar pucBuff, AVL_uint16 uiSize);
AVL6381_ErrorCode I2C_Read8_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puchar puiData);
AVL6381_ErrorCode I2C_Read16_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint16 puiData);
AVL6381_ErrorCode I2C_Read32_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint32 puiData);
AVL6381_ErrorCode I2C_Write_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize);
AVL6381_ErrorCode I2C_Write8_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uchar ucData);
AVL6381_ErrorCode I2C_Write16_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint16 uiData);
AVL6381_ErrorCode I2C_Write32_6381(Avl6381_PortIndex_t ePortIndex, AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint32 uiData);
AVL6381_ErrorCode I2C_Repeater_SendOP_6381(AVL_puchar pBuff, AVL_uchar ucSize);
AVL6381_ErrorCode I2C_Repeater_GetOPStatus_6381(void);
AVL6381_ErrorCode Halt_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetRunningLevel_6381(AVL6381_RunningLevel *pRunningLevel,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegMode_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode EnableMpegOutput_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DisableMpegOutput_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AutoLockChannel_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode TunerI2C_Initialize_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetCarrierFreqOffset_6381(AVL_pint32 piFreqOffsetHz,Avl6381_PortIndex_t ePortIndex);
void ResetFwData_6381(AVL_puchar pucFwData,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode ResetPER_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode ResetBER_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode ResetErrorStat_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetBER_6381(AVL_puint32 puiBER_x10e9, AVL6381_BER_Type enumBERType,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetSleepClock_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode EnableMpegContinuousMode_6381(AVL_uint32 uiByteClockFreq_Hz,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DisableMpegContinuousMode_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetMode_6381(AVL_puchar pucurrent_mode,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegSerialPin_6381(AVL6381_MPEG_SerialDataPin enumSerialPin,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegSerialOrder_6381(AVL6381_MPEG_SERIAL_ORDER enumSerialOrder,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegParalleOrder_6381(AVL6381_MPEG_PARALLEL_ORDER enumParallelOrder,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegSerialSyncPulse_6381(AVL6381_MPEG_SERIAL_SYNC enumSerialSyncPulse,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegParallelPhase_6381(AVL6381_MPEG_PARALLEL_PHASE enumParallelPhase,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegErrorBit_6381(AVL6381_MPEG_ErrorBit enumErrorBit,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegErrorPola_6381(AVL6381_MPEG_ErrorBitPola enumErrorPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode InitErrorStat_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegValidPola_6381(AVL6381_MPEG_ValidPola enumValidPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetVersion_6381(AVL6381_VerInfo * pVerInfo,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode EnableSerialSlowRate(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DisableSerialSlowRate(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetSpectrumPola_6381(AVL6381_SpectrumInversion enumSpectrumPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode InitSDRAM_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DigitalCoreReset_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetMpegPacketLen_6381(AVL6381_MPEG_Packet_Len enumPacketLen,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DisableAGC_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetSpectrumInversion_6381(AVL6381_SpectrumInversion *pSpectrumInversion,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetScatterData_6381(AVL_puint32 puiIQ_Data, AVL_puint16 puiSize,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetGPIODir_6381(AVL6381_PIN_NUMBER enumPin, AVL6381_PIN_DIRECTION enumDir,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode SetGPIOVal_6381(AVL6381_PIN_NUMBER enumPin, AVL6381_PIN_VALUE enumVal,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode GetGPIOVal_6381(AVL6381_PIN_NUMBER enumPin, AVL6381_PIN_VALUE *enumVal,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode InitADC_6381(Avl6381_PortIndex_t ePortIndex);

#if defined(AVL_INTERNAL_USE)
#include "top_addr_map.h"
#include "rc_AVL6381_DEMOD_config_addr_map.h"
#include "rs_AVL6381_DEMOD_status_addr_map.h"
#else

#define rc_AVL6381_DEMOD_config_regs_base (0x00000204)
#define rc_AVL6381_fw_command_iaddr                      (rc_AVL6381_DEMOD_config_regs_base + 0x00000000)
#define rc_AVL6381_sdram_test_return_iaddr               (rc_AVL6381_DEMOD_config_regs_base + 0x00000004)
#define rc_AVL6381_sdram_test_result_iaddr               (rc_AVL6381_DEMOD_config_regs_base + 0x00000008)
#define rc_AVL6381_sdram_config_iaddr                    (rc_AVL6381_DEMOD_config_regs_base + 0x0000000c)
#define rc_AVL6381_sdram_timing_config_0_iaddr           (rc_AVL6381_DEMOD_config_regs_base + 0x00000010)
#define rc_AVL6381_sdram_timing_config_1_iaddr           (rc_AVL6381_DEMOD_config_regs_base + 0x00000014)
#define rc_AVL6381_get_config_from_tps_caddr             (rc_AVL6381_DEMOD_config_regs_base + 0x0000001b)
#define rc_AVL6381_demod_mode_caddr                      (rc_AVL6381_DEMOD_config_regs_base + 0x00000022)


#define core_io__base 0x00108000
#define core_io__chip_id__offset                    0x00000004
#define core_io__mpeg_bus_tri_enb__offset           0x00000030
#define core_io__rfagc_tri_enb__offset              0x00000034

#define adc_wrapper__base                           0x00104000
#define adc_wrapper__adc_sel_control__offset        0x0000003c

#define clock_reset__base                           0x00100000
#define clock_reset__reset_register__offset         0x00000000
#define clock_reset__dll_init__offset               0x00000008
#define clock_reset__dll_deglitch_mode__offset      0x00000010
#define clock_reset__dll_dly_control_lin__offset    0x00000014
#define clock_reset__dll_out_phase__offset          0x00000018
#define clock_reset__dll_rd_phase__offset           0x0000001c
#define clock_reset__pll_aux_clkr__offset           0x00000080
#define clock_reset__pll_aux_clkf__offset           0x00000084
#define clock_reset__pll_aux_od__offset             0x00000088
#define clock_reset__pll_aux_od2__offset            0x0000008c
#define clock_reset__pll_aux_od3__offset            0x00000090
#define clock_reset__pll_aux_bwadj__offset          0x00000094
#define clock_reset__pll_aux_reg_mode__offset       0x0000009c
#define clock_reset__pll_demod_clkr__offset         0x000000c0
#define clock_reset__pll_demod_clkf__offset         0x000000c4
#define clock_reset__pll_demod_od__offset           0x000000c8
#define clock_reset__pll_demod_bwadj__offset        0x000000d4

#define c306_top__base                              0x00110800
#define c306_top__srst__offset                      0x00000040

#define osprey_esm__base                            0x00149100
#define osprey_esm__esm_cntrl__offset               0x00000004
#define osprey_esm__srst__offset                    0x00000000
#define osprey_esm__esm_cntrl__offset               0x00000004
#define osprey_esm__ber_err_cnt__offset             0x00000008
#define osprey_esm__byte_num__offset                0x0000000c
#define osprey_esm__packet_err_cnt__offset          0x00000010
#define osprey_esm__packet_num__offset              0x00000014
#define osprey_esm__ber_err_rollover_cnt__offset    0x00000018
#define osprey_esm__packet_err_rollover_cnt__offset 0x0000001c
#define osprey_esm__byte_rollover_num__offset       0x00000020
#define osprey_esm__packet_rollover_num__offset     0x00000024
#define osprey_esm__tick_clear_req__offset          0x00000028
#define osprey_esm__timetick1_bytetick0__offset     0x0000002c
#define osprey_esm__time_tick_low__offset           0x00000030
#define osprey_esm__time_tick_high__offset          0x00000034
#define osprey_esm__byte_tick_low__offset           0x00000038
#define osprey_esm__byte_tick_high__offset          0x0000003c
#define osprey_esm__ber_err_num_low__offset         0x00000040
#define osprey_esm__packet_err_num_low__offset      0x00000044
#define osprey_esm__byte_num_low__offset            0x00000048
#define osprey_esm__packet_num_low__offset          0x0000004c
#define osprey_esm__ber_err_num_high__offset        0x00000050
#define osprey_esm__packet_err_num_high__offset     0x00000054
#define osprey_esm__byte_num_high__offset           0x00000058
#define osprey_esm__packet_num_high__offset         0x0000005c
#define osprey_esm__auto1_manual0_mode__offset      0x00000060
#define osprey_esm__total_null_pkt_cnt__offset      0x00000064
#define osprey_esm__esm_cntrl_1__offset             0x00000068
#define osprey_esm__pid_mask__offset                0x0000006c

#define osprey_tuner_i2c__base                      0x00118000
#define osprey_tuner_i2c__tuner_i2c_srst__offset    0x00000000
#define osprey_tuner_i2c__tuner_i2c_cntrl__offset   0x00000004
#define osprey_tuner_i2c__tuner_hw_i2c_bit_rpt_clk_div__offset   0x00000018
#define osprey_tuner_i2c__tuner_hw_i2c_bit_rpt_cntrl__offset     0x0000001c

#define beryl_cpu__base                             0x00110000
#define beryl_cpu__cpu_ddc_srst_b_reg__offset       0x00000084
#define beryl_cpu__cpu_valu_srst_b_reg__offset      0x00000088

#define beryl_ddc__base                             0x00160000
#define beryl_ddc__rx_if_aagc_gain__offset          0x0000004c

#endif

#define hw_AVL6381_rom_ver                          0x00040000

#define hw_AVL6381_mpeg_bus_tri_enb                 (core_io__base + core_io__mpeg_bus_tri_enb__offset)
#define hw_AVL6381_chip_id                          (core_io__base + core_io__chip_id__offset)

#define hw_AVL6381_rfagc_tri_enb                    (core_io__base + core_io__rfagc_tri_enb__offset)

#define hw_AVL6381_c306_top_srst                    (c306_top__base + c306_top__srst__offset)

#define hw_AVL6381_adc_clock_sel                    (adc_wrapper__base + adc_wrapper__adc_sel_control__offset)

#define hw_AVL6381_clock_reset                      (clock_reset__base + clock_reset__reset_register__offset)
#define hw_AVL6381_dll_init                         (clock_reset__base + clock_reset__dll_init__offset)
#define hw_AVL6381_dll_dly_control_lin              (clock_reset__base + clock_reset__dll_dly_control_lin__offset)
#define hw_AVL6381_dll_out_phase                    (clock_reset__base + clock_reset__dll_out_phase__offset)
#define hw_AVL6381_dll_rd_phase                     (clock_reset__base + clock_reset__dll_rd_phase__offset)
#define hw_AVL6381_pll_aux_clkr                     (clock_reset__base + clock_reset__pll_aux_clkr__offset)
#define hw_AVL6381_pll_aux_clkf                     (clock_reset__base + clock_reset__pll_aux_clkf__offset)
#define hw_AVL6381_pll_aux_od                       (clock_reset__base + clock_reset__pll_aux_od__offset)
#define hw_AVL6381_pll_aux_od2                      (clock_reset__base + clock_reset__pll_aux_od2__offset)
#define hw_AVL6381_pll_aux_od3                      (clock_reset__base + clock_reset__pll_aux_od3__offset)
#define hw_AVL6381_pll_aux_bwadj                    (clock_reset__base + clock_reset__pll_aux_bwadj__offset)
#define hw_AVL6381_pll_demod_clkr                   (clock_reset__base + clock_reset__pll_demod_clkr__offset)
#define hw_AVL6381_pll_demod_clkf                   (clock_reset__base + clock_reset__pll_demod_clkf__offset)
#define hw_AVL6381_pll_demod_od                     (clock_reset__base + clock_reset__pll_demod_od__offset)
#define hw_AVL6381_pll_demod_bwadj                  (clock_reset__base + clock_reset__pll_demod_bwadj__offset)
#define hw_AVL6381_dll_deglitch_mode                (clock_reset__base + clock_reset__dll_deglitch_mode__offset)


#define hw_AVL6381_esm_cntrl                        (osprey_esm__base + osprey_esm__esm_cntrl__offset)
#define hw_AVL6381_byte_num                         (osprey_esm__base + osprey_esm__byte_num__offset)
#define hw_AVL6381_ber_err_cnt                      (osprey_esm__base + osprey_esm__ber_err_cnt__offset)
#define hw_AVL6381_packet_num                       (osprey_esm__base + osprey_esm__packet_num__offset)
#define hw_AVL6381_packet_err_cnt                   (osprey_esm__base + osprey_esm__packet_err_cnt__offset)
#define hw_AVL6381_auto1_manual0_mode               (osprey_esm__base + osprey_esm__auto1_manual0_mode__offset)
#define hw_AVL6381_tick_clear_req                   (osprey_esm__base + osprey_esm__tick_clear_req__offset)
#define hw_AVL6381_timetick1_bytetick0              (osprey_esm__base + osprey_esm__timetick1_bytetick0__offset)
#define hw_AVL6381_time_tick_low                    (osprey_esm__base + osprey_esm__time_tick_low__offset)
#define hw_AVL6381_time_tick_high                   (osprey_esm__base + osprey_esm__time_tick_high__offset)
#define hw_AVL6381_byte_tick_low                    (osprey_esm__base + osprey_esm__byte_tick_low__offset)
#define hw_AVL6381_byte_tick_high                   (osprey_esm__base + osprey_esm__byte_tick_high__offset)


#define hw_AVL6381_tuner_i2c_srst                   (osprey_tuner_i2c__base + osprey_tuner_i2c__tuner_i2c_srst__offset)
#define hw_AVL6381_tuner_hw_i2c_bit_rpt_clk_div     (osprey_tuner_i2c__base + osprey_tuner_i2c__tuner_hw_i2c_bit_rpt_clk_div__offset)
#define hw_AVL6381_tuner_hw_i2c_bit_rpt_cntrl       (osprey_tuner_i2c__base + osprey_tuner_i2c__tuner_hw_i2c_bit_rpt_cntrl__offset)
#define hw_AVL6381_tuner_i2c_cntrl                  (osprey_tuner_i2c__base + osprey_tuner_i2c__tuner_i2c_cntrl__offset)

#define hw_AVL6381_cpu_ddc_srst                     (beryl_cpu__base + beryl_cpu__cpu_ddc_srst_b_reg__offset)
#define hw_AVL6381_cpu_valu_srst                    (beryl_cpu__base + beryl_cpu__cpu_valu_srst_b_reg__offset)

#define hw_AVL6381_if_aagc_gain                     (beryl_ddc__base + beryl_ddc__rx_if_aagc_gain__offset)

#define rs_AVL6381_core_ready_word_iaddr            0x000000a0
#define rs_AVL6381_current_active_mode_iaddr        0x200
#define rc_mode_configuration_saddr                 0x2d0000
#define rc_mode_switch_saddr                        0x2d0002
#define hw_AVL6381_digital_core_reset               0x38fffc

#if defined(AVL_INTERNAL_USE)
#include "rc_AVL6381_DTMB_config_addr_map.h"
#include "rs_AVL6381_DTMB_status_addr_map.h"
#else

#define rc_AVL6381_DTMB_config_regs_base (0x00000300)
#define rc_AVL6381_DTMB_symbol_rate_Hz_iaddr             (rc_AVL6381_DTMB_config_regs_base + 0x00000000)
#define rc_AVL6381_DTMB_sample_rate_Hz_iaddr             (rc_AVL6381_DTMB_config_regs_base + 0x00000004)
#define rs_AVL6381_DTMB_fixed_header_caddr               (rs_AVL6381_DTMB_status_regs_base + 0x00000009)
#define rc_AVL6381_DTMB_ifagc_pol_caddr                  (rc_AVL6381_DTMB_config_regs_base + 0x0000000b)
#define rc_AVL6381_DTMB_iq_comp_enable_caddr             (rc_AVL6381_DTMB_config_regs_base + 0x00000019)
#define rc_AVL6381_DTMB_ddc_input_select_iaddr           (rc_AVL6381_DTMB_config_regs_base + 0x00000020)
#define rc_AVL6381_DTMB_tuner_type_caddr                 (rc_AVL6381_DTMB_config_regs_base + 0x00000020)
#define rc_AVL6381_DTMB_input_format_caddr               (rc_AVL6381_DTMB_config_regs_base + 0x00000021)
#define rc_AVL6381_DTMB_spectrum_invert_caddr            (rc_AVL6381_DTMB_config_regs_base + 0x00000022)
#define rc_AVL6381_DTMB_input_select_caddr               (rc_AVL6381_DTMB_config_regs_base + 0x00000023)
#define rc_AVL6381_DTMB_nom_carrier_freq_Hz_iaddr        (rc_AVL6381_DTMB_config_regs_base + 0x00000024)
#define rc_AVL6381_DTMB_equalizer_control_iaddr          (rc_AVL6381_DTMB_config_regs_base + 0x00000028)
#define rc_AVL6381_DTMB_equalizer_mode_caddr             (rc_AVL6381_DTMB_config_regs_base + 0x0000002b)
#define rc_AVL6381_DTMB_dmd_clk_Hz_iaddr                 (rc_AVL6381_DTMB_config_regs_base + 0x00000038)
#define rc_AVL6381_DTMB_sdram_clk_Hz_iaddr               (rc_AVL6381_DTMB_config_regs_base + 0x0000003c)
#define rc_AVL6381_DTMB_mpeg_seq_caddr                   (rc_AVL6381_DTMB_config_regs_base + 0x00000050)
#define rc_AVL6381_DTMB_serial_outpin_sel_caddr          (rc_AVL6381_DTMB_config_regs_base + 0x00000051)
#define rc_AVL6381_DTMB_mpeg_posedge_caddr               (rc_AVL6381_DTMB_config_regs_base + 0x00000052)
#define rc_AVL6381_DTMB_mpeg_serial_caddr                (rc_AVL6381_DTMB_config_regs_base + 0x00000053)
#define rc_AVL6381_DTMB_pkt_err_pol_caddr                (rc_AVL6381_DTMB_config_regs_base + 0x00000054)
#define rc_AVL6381_DTMB_ts0_tsp1_caddr                   (rc_AVL6381_DTMB_config_regs_base + 0x00000057)
#define rc_AVL6381_DTMB_mpeg_ts_error_bit_en_caddr       (rc_AVL6381_DTMB_config_regs_base + 0x00000078)
#define rs_AVL6381_DTMB_demod_status_caddr               (rs_AVL6381_DTMB_status_regs_base + 0x00000080)
#define rc_AVL6381_DTMB_fec_clk_Hz_iaddr                 (rc_AVL6381_DTMB_config_regs_base + 0x00000084)
#define rc_AVL6381_DTMB_cont_mpegclk_en_caddr            (rc_AVL6381_DTMB_config_regs_base + 0x0000008b)
#define rc_AVL6381_DTMB_cntns_pkt_para_rate_frac_n_iaddr (rc_AVL6381_DTMB_config_regs_base + 0x0000008c)
#define rc_AVL6381_DTMB_cntns_pkt_para_rate_frac_d_iaddr (rc_AVL6381_DTMB_config_regs_base + 0x00000090)
#define rc_AVL6381_DTMB_cntns_pkt_seri_rate_frac_n_iaddr (rc_AVL6381_DTMB_config_regs_base + 0x00000094)
#define rc_AVL6381_DTMB_cntns_pkt_seri_rate_frac_d_iaddr (rc_AVL6381_DTMB_config_regs_base + 0x00000098)
#define rs_AVL6381_DTMB_signal_strength_iaddr            (rs_AVL6381_DTMB_status_regs_base + 0x0000009c)
#define rc_AVL6381_DTMB_ADC_I0_Q1_caddr                  (rc_AVL6381_DTMB_config_regs_base + 0x000001d7)
#define rc_AVL6381_DTMB_mpeg_clk_phase_iaddr             (rc_AVL6381_DTMB_config_regs_base + 0x000001e0)
#define rc_AVL6381_DTMB_tps_manual_pn_caddr              (rc_AVL6381_DTMB_config_regs_base + 0x000001e4)
#define rc_AVL6381_DTMB_pn_chk_en_caddr                  (rc_AVL6381_DTMB_config_regs_base + 0x000001e5)
#define rc_AVL6381_DTMB_seri_sync_1_pulse_caddr          (rc_AVL6381_DTMB_config_regs_base + 0x000001e6)
#define rc_AVL6381_DTMB_mpeg_valid_inv_caddr             (rc_AVL6381_DTMB_config_regs_base + 0x000001e7)

#define rs_AVL6381_DTMB_status_regs_base (0x000000a4)
#define rs_AVL6381_DTMB_lock_status_iaddr                (rs_AVL6381_DTMB_status_regs_base + 0x00000000)
#define rs_AVL6381_DTMB_lost_lock_caddr                  (rs_AVL6381_DTMB_status_regs_base + 0x00000001)
#define rs_AVL6381_DTMB_fec_lock_caddr                   (rs_AVL6381_DTMB_status_regs_base + 0x00000002)
#define rs_AVL6381_DTMB_mode_status_iaddr                (rs_AVL6381_DTMB_status_regs_base + 0x00000004)
#define rs_AVL6381_DTMB_interleaver_mode_caddr           (rs_AVL6381_DTMB_status_regs_base + 0x00000004)
#define rs_AVL6381_DTMB_carrier_mode_caddr               (rs_AVL6381_DTMB_status_regs_base + 0x00000005)
#define rs_AVL6381_DTMB_fec_rate_caddr                   (rs_AVL6381_DTMB_status_regs_base + 0x00000006)
#define rs_AVL6381_DTMB_header_mode_caddr                (rs_AVL6381_DTMB_status_regs_base + 0x00000007)
#define rs_AVL6381_DTMB_modulation_status_iaddr          (rs_AVL6381_DTMB_status_regs_base + 0x00000008)
#define rs_AVL6381_DTMB_nr_mode_caddr                    (rs_AVL6381_DTMB_status_regs_base + 0x0000000a)
#define rs_AVL6381_DTMB_constellation_mode_caddr         (rs_AVL6381_DTMB_status_regs_base + 0x0000000b)
#define rs_AVL6381_DTMB_patch_ver_iaddr                  (rs_AVL6381_DTMB_status_regs_base + 0x0000004c)
#define rs_AVL6381_DTMB_carrier_freq_bin_Hz_iaddr        (rs_AVL6381_DTMB_status_regs_base + 0x00000050)
#define rs_AVL6381_DTMB_demod_lock_status_iaddr          (rs_AVL6381_DTMB_status_regs_base + 0x00000068)
#define rs_AVL6381_DTMB_snr_dB_x100_avg_saddr            (rs_AVL6381_DTMB_status_regs_base + 0x00000078)
#define rs_AVL6381_DTMB_num_early_reacq_saddr            (rs_AVL6381_DTMB_status_regs_base + 0x000000a2)



#endif

#define hw_AVL6381_DTMB_cfo_freq_est                    0x001600C0
#define hw_AVL6381_DTMB_scatter_data                    0x002fdc00

typedef enum AVL_DTMB_CarrierMode
{
    RX_MULTI_CARRIER = 0,
    RX_SINGLE_CARRIER = 1
}AVL_DTMB_CarrierMode;

typedef enum AVL_DTMB_FrameHeaderMode
{
    RX_PN420 = 1,
    RX_PN595 = 2,
    RX_PN945 = 3
}AVL_DTMB_FrameHeaderMode;

typedef enum AVL_DTMB_InterleaveMode
{
    RX_INTERLEAVE_240 = 0,
    RX_INTERLEAVE_720 = 1
}AVL_DTMB_InterleaveMode;

typedef enum AVL_DTMB_NR_Mode
{
    RX_NR_OFF = 0,
    RX_NR_ON = 1
}AVL_DTMB_NR_Mode;

typedef enum AVL_DTMB_FEC_Rate
{
    RX_FEC_2_5 = 0,
    RX_FEC_3_5 = 1,
    RX_FEC_4_5 = 2
}AVL_DTMB_FEC_Rate;

typedef enum AVL_DTMB_ModulationMode
{
    RX_DTMB_4QAM = 0,
    RX_DTMB_16QAM = 1,
    RX_DTMB_32QAM = 2,
    RX_DTMB_64QAM = 3,
}AVL_DTMB_ModulationMode;

typedef struct AVL_DTMB_ManualLockInfo
{
    AVL_DTMB_CarrierMode m_CarrierMode;
    AVL_DTMB_FEC_Rate m_CodeRate;
    AVL_DTMB_ModulationMode m_Modulation;
    AVL_DTMB_NR_Mode m_MappingMode;
    AVL_DTMB_InterleaveMode m_InterleaveMode;
    AVL_DTMB_FrameHeaderMode m_HeaderMode;
    AVL6381_SpectrumInversion m_Spectrum;
}AVL_DTMB_ManualLockInfo;

typedef struct AVL_DTMB_SignalInfo
{
    AVL_DTMB_CarrierMode m_CarrierMode;
    AVL_DTMB_FEC_Rate m_CodeRate;
    AVL_DTMB_ModulationMode m_Modulation;
    AVL_DTMB_NR_Mode m_MappingMode;
    AVL_DTMB_InterleaveMode m_InterleaveMode;
    AVL_DTMB_FrameHeaderMode m_HeaderMode;
}AVL_DTMB_SignalInfo;

#define MAX_LOCK_RETRIES            0x4

AVL6381_ErrorCode DTMB_Halt_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetRunningLevel_6381(AVL6381_RunningLevel *pRunningLevel,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetSpectrumInversion_6381(AVL6381_SpectrumInversion * pSpectrumInversion,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetLockStatus_6381(AVL_puint32 puiLockStatus,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetCarrierFreqOffset_6381( AVL_pint32 piFreqOffsetHz,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetScatterData_6381(AVL_puint32 puiIQ_Data, AVL_puint16 puiSize,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetSignalInfo_6381(AVL_DTMB_SignalInfo *pSignalInfo,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetSignalLevel_6381(AVL_puint32 puiSignalLevel,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetSNR_6381(AVL_puint32 puiSNR_db,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_InitRx_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_GetSignalQuality_6381( AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetModeforDTMB_6381( AVL_uint32 uiSetMode);
AVL6381_ErrorCode DTMB_SetAGCPola_6381(AVL6381_AGCPola enumAGCPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_EnableMpegContinuousMode_6381(AVL_uint32 uiByteClockFreq_Hz,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_DisableMpegContinuousMode_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegMode_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_AutoLockChannel_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetSymbolRate_6381(AVL_uint32 uiSymbolRateHz,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegSerialPin_6381(AVL6381_MPEG_SerialDataPin enumSerialPin,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegSerialOrder_6381(AVL6381_MPEG_SERIAL_ORDER enumSerialOrder,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegParallelOrder_6381(AVL6381_MPEG_PARALLEL_ORDER enumParallelOrder,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegSerialSyncPulse_6381(AVL6381_MPEG_SERIAL_SYNC enumSerialSyncPulse,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegParallelPhase_6381(AVL6381_MPEG_PARALLEL_PHASE enumParallelPhase,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegErrorBit_6381(AVL6381_MPEG_ErrorBit enumErrorBit,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegErrorPola_6381(AVL6381_MPEG_ErrorBitPola enumErrorPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegValidPola_6381(AVL6381_MPEG_ValidPola enumValidPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_NoSignalDetection_6381(AVL_uint32 *puiNoSignal,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetSpectrumPola_6381(AVL6381_SpectrumInversion enumSpectrumPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_InitADC_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DTMB_SetMpegPacketLen_6381(AVL6381_MPEG_Packet_Len enumPacketLen,Avl6381_PortIndex_t ePortIndex);

#if defined(AVL_INTERNAL_USE)
#include "rc_AVL6381_DVBC_config_addr_map.h"
#include "rs_AVL6381_DVBC_status_addr_map.h"
#else

#define rc_AVL6381_DVBC_config_regs_base (0x00000558)
#define rc_AVL6381_DVBC_symbol_rate_Hz_iaddr             (rc_AVL6381_DVBC_config_regs_base + 0x00000000)
#define rc_AVL6381_DVBC_sample_rate_Hz_iaddr             (rc_AVL6381_DVBC_config_regs_base + 0x00000004)
#define rc_AVL6381_DVBC_dmd_clk_Hz_iaddr                 (rc_AVL6381_DVBC_config_regs_base + 0x00000008)
#define rc_AVL6381_DVBC_sdram_clk_Hz_iaddr               (rc_AVL6381_DVBC_config_regs_base + 0x0000000c)
#define rc_AVL6381_DVBC_mpeg_seq_caddr                   (rc_AVL6381_DVBC_config_regs_base + 0x00000014)
#define rc_AVL6381_DVBC_serial_outpin_sel_caddr          (rc_AVL6381_DVBC_config_regs_base + 0x00000015)
#define rc_AVL6381_DVBC_mpeg_posedge_caddr               (rc_AVL6381_DVBC_config_regs_base + 0x00000016)
#define rc_AVL6381_DVBC_mpeg_serial_caddr                (rc_AVL6381_DVBC_config_regs_base + 0x00000017)
#define rc_AVL6381_DVBC_pkt_err_pol_caddr                (rc_AVL6381_DVBC_config_regs_base + 0x00000018)
#define rc_AVL6381_DVBC_ts0_tsp1_caddr                   (rc_AVL6381_DVBC_config_regs_base + 0x0000001b)
#define rc_AVL6381_DVBC_mpeg_ts_error_bit_en_caddr       (rc_AVL6381_DVBC_config_regs_base + 0x00000020)
#define rc_AVL6381_DVBC_ddc_input_select_iaddr           (rc_AVL6381_DVBC_config_regs_base + 0x00000024)
#define rc_AVL6381_DVBC_tuner_type_caddr                 (rc_AVL6381_DVBC_config_regs_base + 0x00000024)
#define rc_AVL6381_DVBC_input_format_caddr               (rc_AVL6381_DVBC_config_regs_base + 0x00000025)
#define rc_AVL6381_DVBC_spectrum_invert_caddr            (rc_AVL6381_DVBC_config_regs_base + 0x00000026)
#define rc_AVL6381_DVBC_input_select_caddr               (rc_AVL6381_DVBC_config_regs_base + 0x00000027)
#define rc_AVL6381_DVBC_if_freq_Hz_iaddr                 (rc_AVL6381_DVBC_config_regs_base + 0x00000028)
#define rc_AVL6381_DVBC_qam_mode_iaddr                   (rc_AVL6381_DVBC_config_regs_base + 0x0000002c)
#define rc_AVL6381_DVBC_ifagc_pol_caddr                  (rc_AVL6381_DVBC_config_regs_base + 0x00000047)
#define rc_AVL6381_DVBC_fec_clk_Hz_iaddr                 (rc_AVL6381_DVBC_config_regs_base + 0x00000050)
#define rc_AVL6381_DVBC_get_btr_crl_iaddr                (rc_AVL6381_DVBC_config_regs_base + 0x00000080)
#define rc_AVL6381_DVBC_cont_mpegclk_en_2_saddr          (rc_AVL6381_DVBC_config_regs_base + 0x00000096)
#define rc_AVL6381_DVBC_cntns_pkt_para_rate_frac_n_32bit_iaddr (rc_AVL6381_DVBC_config_regs_base + 0x00000098)
#define rc_AVL6381_DVBC_cntns_pkt_para_rate_frac_d_32bit_iaddr (rc_AVL6381_DVBC_config_regs_base + 0x0000009c)
#define rc_AVL6381_DVBC_cntns_pkt_seri_rate_frac_n_32bit_iaddr (rc_AVL6381_DVBC_config_regs_base + 0x000000a0)
#define rc_AVL6381_DVBC_cntns_pkt_seri_rate_frac_d_32bit_iaddr (rc_AVL6381_DVBC_config_regs_base + 0x000000a4)
#define rc_AVL6381_DVBC_ADC_I0_Q1_caddr                  (rc_AVL6381_DVBC_config_regs_base + 0x000001ef)
#define rc_AVL6381_DVBC_mpeg_clk_phase_iaddr             (rc_AVL6381_DVBC_config_regs_base + 0x000001f0)
#define rc_AVL6381_DVBC_seri_sync_1_pulse_caddr          (rc_AVL6381_DVBC_config_regs_base + 0x000001f6)
#define rc_AVL6381_DVBC_mpeg_valid_inv_caddr             (rc_AVL6381_DVBC_config_regs_base + 0x000001f7)

#define rs_AVL6381_DVBC_status_regs_base (0x000001a0)
#define rs_AVL6381_DVBC_lock_status_iaddr                (rs_AVL6381_DVBC_status_regs_base + 0x00000000)
#define rs_AVL6381_DVBC_lost_lock_saddr                  (rs_AVL6381_DVBC_status_regs_base + 0x00000002)
#define rs_AVL6381_DVBC_mode_status_iaddr                (rs_AVL6381_DVBC_status_regs_base + 0x00000004)
#define rs_AVL6381_DVBC_demod_lock_indicator_iaddr       (rs_AVL6381_DVBC_status_regs_base + 0x00000008)
#define rs_AVL6381_DVBC_snr_dB_x100_saddr                (rs_AVL6381_DVBC_status_regs_base + 0x0000000e)
#define rs_AVL6381_DVBC_patch_ver_iaddr                  (rs_AVL6381_DVBC_status_regs_base + 0x00000010)
#define rs_AVL6381_DVBC_carrier_freq_offset_Hz_iaddr     (rs_AVL6381_DVBC_status_regs_base + 0x00000014)
#define rs_AVL6381_DVBC_no_signal_input_iaddr            (rs_AVL6381_DVBC_status_regs_base + 0x00000018)
#define rs_AVL6381_DVBC_aagc_gain_iaddr                  (rs_AVL6381_DVBC_status_regs_base + 0x00000048)
#define rs_AVL6381_DVBC_berBeforRS_iaddr                 (rs_AVL6381_DVBC_status_regs_base + 0x0000004c)

#endif

#define hw_AVL6381_DVBC_scatter_data         0x002edd70

typedef enum AVL_DVBC_QAMMode
{
    RX_DVBC_16QAM = 0,
    RX_DVBC_32QAM = 1,
    RX_DVBC_64QAM = 2,
    RX_DVBC_128QAM = 3,
    RX_DVBC_256QAM = 4
}AVL_DVBC_QAMMode;

typedef struct AVL_DVBC_InputSignalInfo
{
    AVL_DVBC_QAMMode m_Modulation;
}AVL_DVBC_InputSignalInfo;
    

typedef struct AVL_DVBC_SignalInfo
{
    AVL_DVBC_QAMMode m_Modulation;
}AVL_DVBC_SignalInfo;

AVL6381_ErrorCode DVBC_Halt_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetRunningLevel_6381(AVL6381_RunningLevel *pRunningLevel,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetSpectrumInversion_6381(AVL6381_SpectrumInversion * pSpectrumInversion,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetLockStatus_6381(AVL_puint32 puiLockStatus,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetCarrierFreqOffset_6381(AVL_pint32 piFreqOffsetHz,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetScatterData_6381(AVL_puint32 puiIQ_Data, AVL_puint16 puiSize,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetSignalInfo_6381(AVL_DVBC_SignalInfo *pSignalInfo,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetSignalLevel_6381(AVL_puint32 puiSignalLevel,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetSNR_6381(AVL_puint32 puiSNR_db,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_InitRx_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetSignalQuality_6381(AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetAGCPola_6381(AVL6381_AGCPola enumAGCPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_EnableMpegContinuousMode_6381(AVL_uint32 uiByteClockFreq_Hz,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_DisableMpegContinuousMode_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegMode_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_AutoLockChannel_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_ManualLockChannel_6381(AVL_DVBC_QAMMode enumQAMMode);
AVL6381_ErrorCode DVBC_SetSymbolRate_6381(AVL_uint32 uiSymbolRateHz,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegSerialPin_6381(AVL6381_MPEG_SerialDataPin enumSerialPin,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegSerialOrder_6381(AVL6381_MPEG_SERIAL_ORDER enumSerialOrder,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegParallelOrder_6381(AVL6381_MPEG_PARALLEL_ORDER enumParallelOrder,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegSerialSyncPulse_6381(AVL6381_MPEG_SERIAL_SYNC enumSerialSyncPulse,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegParallelPhase_6381(AVL6381_MPEG_PARALLEL_PHASE enumParallelPhase,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegErrorBit_6381(AVL6381_MPEG_ErrorBit enumErrorBit,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegErrorPola_6381(AVL6381_MPEG_ErrorBitPola enumErrorPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegValidPola_6381(AVL6381_MPEG_ValidPola enumValidPola,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_NoSignalDetection_6381(AVL_uint32 *puiNoSignal,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_InitADC_6381(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_SetMpegPacketLen_6381(AVL6381_MPEG_Packet_Len enumPacketLen,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode DVBC_GetBER_BeforeRS_6381(AVL_puint32 puiBERBeforeRS_x10e9,Avl6381_PortIndex_t ePortIndex);

#ifdef AVL_CPLUSPLUS
}
#endif

#endif

