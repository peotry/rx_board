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



#ifndef AVL6381_API_H
#define AVL6381_API_H

#ifdef AVL_CPLUSPLUS
extern "C" {
#endif

#include "user_defined_data_type.h"
#include "user_defined_function.h"
#include "AVL6381_Internal.h"

extern AVL6381_PLL_Conf gAVL6381PLLConfig[AVL6381_PORT_NUM][8];
extern AVL6381_SDRAM_Conf gAVL6381SDRAMConfig[AVL6381_PORT_NUM];
extern AVL6381_Chip gsAVL6381Chip[AVL6381_PORT_NUM];
extern AVL6381_ErrorStats gsAVL6381ErrorStat[AVL6381_PORT_NUM];
extern AVL_semaphore gAVL6381I2CSem[AVL6381_PORT_NUM];
extern AVL6381_AGCPola geAVL6381AGCPola[AVL6381_PORT_NUM];
extern AVL6381_MPEG_SerialDataPin geAVL6381SerialPin[AVL6381_PORT_NUM];
extern AVL6381_MPEG_SERIAL_ORDER geAVL6381SerialOrder[AVL6381_PORT_NUM];
extern AVL6381_MPEG_SERIAL_SYNC geAVL6381SerialSync[AVL6381_PORT_NUM];
extern AVL6381_MPEG_ErrorBit geAVL6381ErrorBit[AVL6381_PORT_NUM];
extern AVL6381_MPEG_ErrorBitPola geAVL6381ErrorPola[AVL6381_PORT_NUM];
extern AVL6381_MPEG_ValidPola geAVL6381ValidPola[AVL6381_PORT_NUM];
extern AVL6381_MPEG_Packet_Len geAVL6381PacketLen[AVL6381_PORT_NUM];
extern AVL_uint32 guiDTMBSerialMPEGContinuousHz[AVL6381_PORT_NUM];
extern AVL_uint32 guiDVBCSerialMPEGContinuousHz[AVL6381_PORT_NUM];
extern AVL_uint32 guiParallelMPEGContinuousHz[AVL6381_PORT_NUM];
extern AVL_uchar gucDisableAGC[AVL6381_PORT_NUM];
extern AVL_uchar gucDisableMPEGOutput[AVL6381_PORT_NUM];
extern AVL_uint32 guiDTMBSymbolRateSps[AVL6381_PORT_NUM];

extern AVL_uchar AVL6381_FwData_DTMB[];
extern AVL_uchar AVL6381_FwData_DVBC[];
extern AVL_uchar AVL6381_freezeData_DTMB[];
extern AVL_uchar gAVL6381freezeflag[AVL6381_PORT_NUM];
extern Avl6381_PortIndex_t gsPortIndex;

typedef enum AVL6381_LockMode
{
    LOCK_MODE_AUTO = 0,
    LOCK_MODE_MANUAL = 1
}AVL6381_LockMode;

typedef struct AVL6381_Para
{
    AVL_uchar sStartupMode;
    AVL6381_InputConfig sInputConfig;
    AVL6381_MPEGConfig sMPEGConfig;
}AVL6381_Para;

struct Signal_Level
{
    AVL_uint16 SignalLevel;
    AVL_int16 SignalDBM;
};
AVL6381_ErrorCode AVL6381_Initialize(AVL_uint16 DemodAddr, AVL6381_Para *Para, Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_AutoLock(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_DTMBManualLock(AVL_DTMB_ManualLockInfo *psDTMBManualLockInfo, Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_DVBCManualLock(AVL_uint32 uiSymbolRateHz, AVL_DVBC_QAMMode enumQAMMode, Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_GetLockStatus(AVL_puint32 puiLockStatus, Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_NoSignalDetection(AVL_uint32 *puiNoSignal, Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_I2CBypassOn(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_I2CBypassOff(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_GetSNR(AVL_puint32 puiSNR_db_x100,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_GetSSI(AVL_puint32 puiSignalStrength,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_GetSQI(AVL_puint32 puiSignalQuality,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_SetMode(AVL_uchar mode,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_Sleep(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_Wakeup(Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_GetPER(AVL_puint32 puiPER_x10e9,Avl6381_PortIndex_t ePortIndex);
AVL6381_ErrorCode AVL6381_GetChipID(Avl6381_PortIndex_t ePortIndex, AVL_uint16 usI2CAddr, AVL_puint32 puiChipID);
AVL6381_ErrorCode AVL6381_TunerRead(Avl6381_PortIndex_t ePortIndex, AVL_uchar ucSlaveAddr,AVL_puchar pRegAddr,AVL_uchar ucAddrSize, AVL_puchar pucBuff, AVL_uint16 uiSize);
AVL6381_ErrorCode AVL6381_TunerWrite(Avl6381_PortIndex_t ePortIndex, AVL_uchar ucSlaveAddr, AVL_puchar ucBuff, AVL_uint16 uiSize);

#ifdef AVL_CPLUSPLUS
}
#endif

#endif

